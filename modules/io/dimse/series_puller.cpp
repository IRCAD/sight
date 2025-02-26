/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "series_puller.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/os/temp_path.hpp>

#include <data/series_set.hpp>

#include <io/dimse/exceptions/base.hpp>
#include <io/dimse/helper/series.hpp>
#include <io/dimse/series_enquirer.hpp>

#include <service/extension/config.hpp>

#include <sstream>

namespace sight::module::io::dimse
{

static const core::com::signals::key_t PROGRESSED_SIG       = "progressed";
static const core::com::signals::key_t STARTED_PROGRESS_SIG = "progress_started";
static const core::com::signals::key_t STOPPED_PROGRESS_SIG = "progress_stopped";

static const core::com::slots::key_t REMOVE_SERIES_SLOT = "removeSeries";

static const std::string DICOM_READER_CONFIG = "dicomReader";
static const std::string READER_CONFIG       = "readerConfig";

series_puller::series_puller() noexcept :
    service::notifier(m_signals)
{
    m_slot_store_instance = this->new_slot(
        sight::io::dimse::series_retriever::PROGRESS_CALLBACK_SLOT,
        &series_puller::store_instance_callback,
        this
    );

    m_sig_progressed       = this->new_signal<progressed_signal_t>(PROGRESSED_SIG);
    m_sig_progress_started = this->new_signal<progress_started_signal_t>(STARTED_PROGRESS_SIG);
    m_sig_progress_stopped = this->new_signal<progress_stopped_signal_t>(STOPPED_PROGRESS_SIG);

    new_slot(REMOVE_SERIES_SLOT, &series_puller::remove_series, this);
}

//------------------------------------------------------------------------------

void series_puller::configuring()
{
    const config_t config = this->get_config().get_child("config.<xmlattr>");

    m_dicom_reader_implementation = config.get(DICOM_READER_CONFIG, m_dicom_reader_implementation);
    SIGHT_ERROR_IF("'" + DICOM_READER_CONFIG + "' attribute not set", m_dicom_reader_implementation.empty())

    m_reader_config = config.get(READER_CONFIG, m_reader_config);
}

//------------------------------------------------------------------------------

void series_puller::starting()
{
    // Create the worker.
    m_request_worker = core::thread::worker::make();

    // Create the DICOM reader.
    m_series_set = std::make_shared<data::series_set>();

    m_dicom_reader = this->register_service<sight::io::service::reader>(m_dicom_reader_implementation);
    SIGHT_ASSERT("Unable to create a reader of type '" + m_dicom_reader_implementation + "'", m_dicom_reader);
    m_dicom_reader->set_worker(m_request_worker);
    m_dicom_reader->set_inout(m_series_set, sight::io::service::DATA_KEY);
    if(!m_reader_config.empty())
    {
        const auto reader_config =
            service::extension::config::get_default()->get_service_config(
                m_reader_config,
                "sight::io::service::reader"
            );

        SIGHT_ASSERT(
            "No service configuration " << m_reader_config << " for sight::io::service::reader",
            !reader_config.empty()
        );

        m_dicom_reader->set_config(reader_config);
    }

    m_dicom_reader->configure();
    m_dicom_reader->start().wait();
    SIGHT_ASSERT("'" + m_dicom_reader_implementation + "' is not started", m_dicom_reader->started());
}

//------------------------------------------------------------------------------

void series_puller::updating()
{
    const auto selected_series = m_selected_series.lock();

    if(selected_series->empty())
    {
        this->notifier::info("No series selected");
    }
    else
    {
        m_request_worker->post([this](auto&& ...){pull_series();});
    }
}

//------------------------------------------------------------------------------

void series_puller::stopping()
{
    {
        const auto series_set     = m_dest_series_set.lock();
        const auto scoped_emitter = series_set->scoped_emit();

        // Delete old series from the series_set.
        series_set->clear();
    }
    // Unregister the DICOM reader.
    this->unregister_services();

    // Stop the worker.
    m_request_worker->stop();
    m_request_worker.reset();
}

//------------------------------------------------------------------------------

void series_puller::pull_series()
{
    // Set pulling boolean to true.
    bool success = true;

    // Clear map of Dicom series being pulled.
    m_pulling_dicom_series_map.clear();

    // Reset Counters
    m_instance_count = 0;

    // Retrieve data.
    const auto selected_series = m_selected_series.lock();

    // Find which selected series must be pulled.
    dicom_series_container_t pull_series_vector;
    dicom_series_container_t selected_series_vector;
    for(const auto& object : *selected_series)
    {
        // Check that the series is a DICOM series.
        const auto& series = std::dynamic_pointer_cast<data::dicom_series>(object);

        // Check if the series must be pulled.
        if(series)
        {
            const auto& series_instance_uid = series->get_series_instance_uid();
            if(m_local_series.find(series_instance_uid) == m_local_series.cend())
            {
                // Add series in the pulling series map.
                m_pulling_dicom_series_map[series_instance_uid] = series;

                pull_series_vector.push_back(series);
                m_instance_count += series->num_instances();
            }

            selected_series_vector.push_back(series);
        }
    }

    // Pull series.
    if(!pull_series_vector.empty())
    {
        this->notifier::info("Downloading series...");

        // Notify Progress Dialog.
        m_sig_progress_started->async_emit(m_progressbar_id);

        // Retrieve informations.
        const auto pacs_config = m_config.lock();

        auto series_enquirer = std::make_shared<sight::io::dimse::series_enquirer>();

        // Initialize connection.
        try
        {
            series_enquirer->initialize(
                pacs_config->get_local_application_title(),
                pacs_config->get_pacs_host_name(),
                pacs_config->get_pacs_application_port(),
                pacs_config->get_pacs_application_title(),
                pacs_config->get_move_application_title()
            );
            series_enquirer->connect();
        }
        catch(const sight::io::dimse::exceptions::base& e)
        {
            SIGHT_ERROR("Unable to establish a connection with the PACS: " + std::string(e.what()));
            this->notifier::failure("Unable to connect to the PACS");
            return;
        }

        core::thread::worker::sptr worker = core::thread::worker::make();

        try
        {
            using sight::io::dimse::helper::series;
            if(pacs_config->get_retrieve_method() == sight::io::dimse::data::pacs_configuration::retrieve_method::get)
            {
                series_enquirer->pull_series_using_get_retrieve_method(
                    series::to_series_instance_uid_container(
                        pull_series_vector
                    )
                );
            }
            else if(pacs_config->get_retrieve_method()
                    == sight::io::dimse::data::pacs_configuration::retrieve_method::move)
            {
                auto series_retriever = std::make_shared<sight::io::dimse::series_retriever>();
                series_retriever->initialize(
                    pacs_config->get_move_application_title(),
                    pacs_config->get_move_application_port(),
                    1,
                    m_slot_store_instance
                );

                // Start series retriever in a worker.
                worker->post([series_retriever](auto&& ...){series_retriever->start();});

                // Pull Selected Series.
                series_enquirer->pull_series_using_move_retrieve_method(
                    series::to_series_instance_uid_container(
                        pull_series_vector
                    )
                );
            }
            else
            {
                SIGHT_ERROR("Unknown retrieve method, 'get' will be used");
                series_enquirer->pull_series_using_get_retrieve_method(
                    series::to_series_instance_uid_container(
                        pull_series_vector
                    )
                );
            }
        }
        catch(const sight::io::dimse::exceptions::base& e)
        {
            SIGHT_ERROR("Unable to execute query to the PACS: " + std::string(e.what()));
            this->notifier::failure("Unable to execute query");
            success = false;
        }

        // Stop the worker.
        worker->stop();
        worker.reset();

        // Disconnect the series enquirer.
        if(series_enquirer->is_connected_to_pacs())
        {
            series_enquirer->disconnect();
        }
    }
    else
    {
        this->notifier::info("Series already downloaded");
        return;
    }

    // Read series if there is no error.
    if(success)
    {
        this->notifier::success("Series downloaded");
        this->read_local_series(selected_series_vector);
    }
    else
    {
        this->notifier::failure("Series download failed");
    }

    // Notify Progress Dialog.
    m_sig_progress_stopped->async_emit(m_progressbar_id);
}

//------------------------------------------------------------------------------

void series_puller::read_local_series(dicom_series_container_t _selected_series)
{
    const auto dest_series_set = m_dest_series_set.lock();

    // Read only series that are not in the series set.

    // Create temporary series helper.
    const auto scoped_emitter = m_series_set->scoped_emit();

    for(const auto& series : _selected_series)
    {
        const auto& type = series->get_dicom_type();
        if(type == sight::data::series::dicom_t::image)
        {
            this->notifier::info("Unable to read the modality '" + series->get_modality_string() + "'");
            return;
        }

        const std::string& selected_series_uid = series->get_series_instance_uid();

        // Check if the series is loaded.
        if(std::find_if(
               dest_series_set->cbegin(),
               dest_series_set->cend(),
               [&selected_series_uid](const data::series::sptr& _already_loaded_series)
            {
                return _already_loaded_series->get_series_instance_uid() == selected_series_uid;
            }) == dest_series_set->cend())
        {
            this->notifier::info("Reading series...");

            // Clear temporary series.
            m_series_set->clear();

            const auto& path = core::os::temp_dir::shared_directory() / "dicom/";
            m_dicom_reader->set_folder(path.string() + selected_series_uid + "/");
            m_dicom_reader->update();

            // Merge series.
            if(!m_dicom_reader->has_failed() && !m_series_set->empty())
            {
                this->notifier::success("Series read");

                // Add the series to the local series vector.
                m_local_series.insert(selected_series_uid);

                const auto destination_notifier = dest_series_set->scoped_emit();
                std::copy(m_series_set->cbegin(), m_series_set->cend(), sight::data::inserter(*dest_series_set));
            }
            else
            {
                this->notifier::failure("Failed to read series");
            }
        }
    }
}

//------------------------------------------------------------------------------

void series_puller::remove_series(data::series_set::container_t _removed_series)
{
    // Find which series to delete
    if(!m_local_series.empty())
    {
        for(const auto& series : _removed_series)
        {
            if(m_local_series.erase(series->get_series_instance_uid()) > 0)
            {
                this->notifier::info("Local series deleted");
            }
        }
    }
}

//------------------------------------------------------------------------------

void series_puller::store_instance_callback(
    const std::string& _series_instance_uid,
    unsigned int _instance_number,
    const std::string& _file_path
)
{
    // Add path in the DICOM series.
    if(!m_pulling_dicom_series_map[_series_instance_uid].expired())
    {
        data::dicom_series::sptr series = m_pulling_dicom_series_map[_series_instance_uid].lock();
        series->add_dicom_path(_instance_number, _file_path);
    }
    else
    {
        SIGHT_WARN("The Dicom Series " + _series_instance_uid + " has expired.");
    }

    // Notify progress dialog.
    std::stringstream ss;
    ss << "Downloading file " << _instance_number << "/" << m_instance_count;
    float percentage = static_cast<float>(_instance_number) / static_cast<float>(m_instance_count);
    m_sig_progressed->async_emit(m_progressbar_id, percentage, ss.str());
}

//------------------------------------------------------------------------------

service::connections_t series_puller::auto_connections() const
{
    connections_t connections;
    connections.push(SERIES_SET_INOUT, data::series_set::REMOVED_OBJECTS_SIG, REMOVE_SERIES_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse.
