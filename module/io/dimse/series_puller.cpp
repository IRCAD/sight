/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/notification/observer.hpp>

#include <data/series_set.hpp>

#include <io/dicom/helper/series.hpp>
#include <io/dicom/reader/file.hpp>
#include <io/dimse/exceptions/base.hpp>
#include <io/dimse/helper/series.hpp>
#include <io/dimse/series_enquirer.hpp>
#include <io/dimse/series_retriever.hpp>

#include <cstddef>

namespace sight::module::io::dimse
{

series_puller::series_puller() noexcept :
    has_monitors(has_signals::signals())
{
    this->new_signal<signals::progress_started_t>(signals::STARTED_PROGRESS);
    this->new_signal<signals::progress_stopped_t>(signals::STOPPED_PROGRESS);

    new_slot(slots::REMOVE_SERIES, &series_puller::remove_series, this);
}

//------------------------------------------------------------------------------

void series_puller::configuring()
{
}

//------------------------------------------------------------------------------

void series_puller::starting()
{
    // Create the DICOM reader.
    m_series_set = std::make_shared<data::series_set>();
}

//------------------------------------------------------------------------------

void series_puller::updating()
{
    const auto selected_series = m_selected_series.lock();

    if(selected_series->empty())
    {
        this->inform("No series selected");
    }
    else
    {
        this->pull_series();
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
        const auto& series = std::dynamic_pointer_cast<data::series>(object);

        // Check if the series must be pulled.
        if(series)
        {
            const auto& series_instance_uid = series->get_series_instance_uid();
            if(m_local_series.find(series_instance_uid) == m_local_series.cend())
            {
                // Add series in the pulling series map.
                m_pulling_dicom_series_map[series_instance_uid] = series;

                pull_series_vector.push_back(series);
                m_instance_count += static_cast<std::size_t>(series->get_instance_number().value());
            }

            selected_series_vector.push_back(series);
        }
    }

    // Pull series.
    if(!pull_series_vector.empty())
    {
        this->inform("Downloading series...");

        // Notify Progress Dialog.
        this->async_emit(signals::STARTED_PROGRESS);

        // Retrieve informations.
        const auto pacs_config = m_config.lock();

        auto series_enquirer = std::make_shared<sight::io::dimse::series_enquirer>();

        auto progress = this->observe("Pull DICOM Series", false, nullptr, m_instance_count);

        // Initialize connection.
        try
        {
            series_enquirer->initialize(
                pacs_config->get_local_application_title(),
                pacs_config->get_pacs_host_name(),
                pacs_config->get_pacs_application_port(),
                pacs_config->get_pacs_application_title(),
                pacs_config->get_move_application_title(),
                progress
            );
            series_enquirer->connect();
        }
        catch(const sight::io::dimse::exceptions::base& e)
        {
            SIGHT_ERROR("Unable to establish a connection with the PACS: " + std::string(e.what()));
            this->fail("Unable to connect to the PACS");
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
                    progress
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
            this->fail("Unable to execute query");
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
        this->inform("Series already downloaded");
        return;
    }

    // Read series if there is no error.
    if(success)
    {
        this->inform("Series downloaded");
        this->read_local_series(selected_series_vector);
    }
    else
    {
        this->fail("Series download failed");
    }

    // Notify Progress Dialog.
    this->async_emit(signals::STOPPED_PROGRESS);
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
            this->inform("Unable to read the modality '" + series->get_modality_string() + "'");
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
            this->inform("Reading series...");

            // Clear temporary series.
            m_series_set->clear();

            auto path   = sight::io::dicom::helper::series::get_path(*series);
            auto reader = std::make_shared<sight::io::dicom::reader::file>();
            reader->set_object(m_series_set);
            reader->set_folder({path.string()});

            auto observer = this->make_notification<sight::core::notification::observer>("Read image series");
            reader->read(observer);

            // Merge series.
            if(!m_series_set->empty())
            {
                this->inform("Series read");

                // Add the series to the local series vector.
                m_local_series.insert(selected_series_uid);

                const auto destination_notifier = dest_series_set->scoped_emit();
                std::copy(m_series_set->cbegin(), m_series_set->cend(), sight::data::inserter(*dest_series_set));
            }
            else
            {
                this->fail("Failed to read series");
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
                this->inform("Local series deleted");
            }
        }
    }
}

//------------------------------------------------------------------------------

service::connections_t series_puller::auto_connections() const
{
    connections_t connections;
    connections.push(SERIES_SET_INOUT, data::series_set::signals::REMOVED_OBJECTS, slots::REMOVE_SERIES);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse.
