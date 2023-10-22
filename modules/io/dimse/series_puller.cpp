/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <io/dimse/exceptions/Base.hpp>
#include <io/dimse/helper/series.hpp>
#include <io/dimse/SeriesEnquirer.hpp>

#include <service/extension/config.hpp>

#include <sstream>

namespace sight::module::io::dimse
{

static const core::com::signals::key_t PROGRESSED_SIG       = "progressed";
static const core::com::signals::key_t STARTED_PROGRESS_SIG = "progressStarted";
static const core::com::signals::key_t STOPPED_PROGRESS_SIG = "progressStopped";

static const core::com::slots::key_t REMOVE_SERIES_SLOT = "removeSeries";

static const std::string s_DICOM_READER_CONFIG = "dicomReader";
static const std::string s_READER_CONFIG       = "readerConfig";

series_puller::series_puller() noexcept :
    service::notifier(m_signals)
{
    m_slotStoreInstance = this->new_slot(
        sight::io::dimse::SeriesRetriever::PROGRESS_CALLBACK_SLOT,
        &series_puller::storeInstanceCallback,
        this
    );

    m_sigProgressed      = this->new_signal<progressed_signal_t>(PROGRESSED_SIG);
    m_sigProgressStarted = this->new_signal<progress_started_signal_t>(STARTED_PROGRESS_SIG);
    m_sigProgressStopped = this->new_signal<progress_stopped_signal_t>(STOPPED_PROGRESS_SIG);

    new_slot(REMOVE_SERIES_SLOT, &series_puller::removeSeries, this);
}

//------------------------------------------------------------------------------

void series_puller::configuring()
{
    const config_t config = this->get_config().get_child("config.<xmlattr>");

    m_dicomReaderImplementation = config.get(s_DICOM_READER_CONFIG, m_dicomReaderImplementation);
    SIGHT_ERROR_IF("'" + s_DICOM_READER_CONFIG + "' attribute not set", m_dicomReaderImplementation.empty())

    m_readerConfig = config.get(s_READER_CONFIG, m_readerConfig);
}

//------------------------------------------------------------------------------

void series_puller::starting()
{
    // Create the worker.
    m_requestWorker = core::thread::worker::make();

    // Create the DICOM reader.
    m_series_set = std::make_shared<data::series_set>();

    m_dicomReader = this->registerService<sight::io::service::reader>(m_dicomReaderImplementation);
    SIGHT_ASSERT("Unable to create a reader of type '" + m_dicomReaderImplementation + "'", m_dicomReader);
    m_dicomReader->set_worker(m_requestWorker);
    m_dicomReader->set_inout(m_series_set, sight::io::service::s_DATA_KEY);
    if(!m_readerConfig.empty())
    {
        const auto reader_config =
            service::extension::config::getDefault()->get_service_config(
                m_readerConfig,
                "sight::io::service::reader"
            );

        SIGHT_ASSERT(
            "No service configuration " << m_readerConfig << " for sight::io::service::reader",
            !reader_config.empty()
        );

        m_dicomReader->set_config(reader_config);
    }

    m_dicomReader->configure();
    m_dicomReader->start().wait();
    SIGHT_ASSERT("'" + m_dicomReaderImplementation + "' is not started", m_dicomReader->started());
}

//------------------------------------------------------------------------------

void series_puller::updating()
{
    const auto selected_series = m_selectedSeries.lock();

    if(selected_series->empty())
    {
        this->notifier::info("No series selected");
    }
    else
    {
        m_requestWorker->post([this](auto&& ...){pullSeries();});
    }
}

//------------------------------------------------------------------------------

void series_puller::stopping()
{
    // Unregister the DICOM reader.
    this->unregisterServices();

    // Stop the worker.
    m_requestWorker->stop();
    m_requestWorker.reset();
}

//------------------------------------------------------------------------------

void series_puller::pullSeries()
{
    // Set pulling boolean to true.
    bool success = true;

    // Clear map of Dicom series being pulled.
    m_pullingDicomSeriesMap.clear();

    // Reset Counters
    m_instanceCount = 0;

    // Retrieve data.
    const auto selected_series = m_selectedSeries.lock();

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
            const auto& series_instance_uid = series->getSeriesInstanceUID();
            if(m_localSeries.find(series_instance_uid) == m_localSeries.cend())
            {
                // Add series in the pulling series map.
                m_pullingDicomSeriesMap[series_instance_uid] = series;

                pull_series_vector.push_back(series);
                m_instanceCount += series->numInstances();
            }

            selected_series_vector.push_back(series);
        }
    }

    // Pull series.
    if(!pull_series_vector.empty())
    {
        this->notifier::info("Downloading series...");

        // Notify Progress Dialog.
        m_sigProgressStarted->async_emit(m_progressbarId);

        // Retrieve informations.
        const auto pacs_config = m_config.lock();

        auto series_enquirer = std::make_shared<sight::io::dimse::SeriesEnquirer>();

        // Initialize connection.
        try
        {
            series_enquirer->initialize(
                pacs_config->getLocalApplicationTitle(),
                pacs_config->getPacsHostName(),
                pacs_config->getPacsApplicationPort(),
                pacs_config->getPacsApplicationTitle(),
                pacs_config->getMoveApplicationTitle()
            );
            series_enquirer->connect();
        }
        catch(const sight::io::dimse::exceptions::Base& e)
        {
            SIGHT_ERROR("Unable to establish a connection with the PACS: " + std::string(e.what()));
            this->notifier::failure("Unable to connect to the PACS");
            return;
        }

        core::thread::worker::sptr worker = core::thread::worker::make();

        try
        {
            using sight::io::dimse::helper::series;
            if(pacs_config->getRetrieveMethod() == sight::io::dimse::data::PacsConfiguration::GET_RETRIEVE_METHOD)
            {
                series_enquirer->pullSeriesUsingGetRetrieveMethod(
                    series::toSeriesInstanceUIDContainer(
                        pull_series_vector
                    )
                );
            }
            else if(pacs_config->getRetrieveMethod()
                    == sight::io::dimse::data::PacsConfiguration::MOVE_RETRIEVE_METHOD)
            {
                auto series_retriever = std::make_shared<sight::io::dimse::SeriesRetriever>();
                series_retriever->initialize(
                    pacs_config->getMoveApplicationTitle(),
                    pacs_config->getMoveApplicationPort(),
                    1,
                    m_slotStoreInstance
                );

                // Start series retriever in a worker.
                worker->post([series_retriever](auto&& ...){series_retriever->start();});

                // Pull Selected Series.
                series_enquirer->pullSeriesUsingMoveRetrieveMethod(
                    series::toSeriesInstanceUIDContainer(
                        pull_series_vector
                    )
                );
            }
            else
            {
                SIGHT_ERROR("Unknown retrieve method, 'get' will be used");
                series_enquirer->pullSeriesUsingGetRetrieveMethod(
                    series::toSeriesInstanceUIDContainer(
                        pull_series_vector
                    )
                );
            }
        }
        catch(const sight::io::dimse::exceptions::Base& e)
        {
            SIGHT_ERROR("Unable to execute query to the PACS: " + std::string(e.what()));
            this->notifier::failure("Unable to execute query");
            success = false;
        }

        // Stop the worker.
        worker->stop();
        worker.reset();

        // Disconnect the series enquirer.
        if(series_enquirer->isConnectedToPacs())
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
        this->readLocalSeries(selected_series_vector);
    }
    else
    {
        this->notifier::failure("Series download failed");
    }

    // Notify Progress Dialog.
    m_sigProgressStopped->async_emit(m_progressbarId);
}

//------------------------------------------------------------------------------

void series_puller::readLocalSeries(dicom_series_container_t _selected_series)
{
    const auto dest_series_set = m_destSeriesSet.lock();

    // Read only series that are not in the series set.

    // Create temporary series helper.
    const auto scoped_emitter = m_series_set->scoped_emit();

    for(const auto& series : _selected_series)
    {
        const std::string& modality = series->getModality();
        if(modality != "CT" && modality != "MR" && modality != "XA")
        {
            this->notifier::info("Unable to read the modality '" + modality + "'");
            return;
        }

        const std::string& selected_series_uid = series->getSeriesInstanceUID();

        // Check if the series is loaded.
        if(std::find_if(
               dest_series_set->cbegin(),
               dest_series_set->cend(),
               [&selected_series_uid](const data::series::sptr& _already_loaded_series)
            {
                return _already_loaded_series->getSeriesInstanceUID() == selected_series_uid;
            }) == dest_series_set->cend())
        {
            this->notifier::info("Reading series...");

            // Clear temporary series.
            m_series_set->clear();

            const auto& path = core::os::temp_dir::shared_directory() / "dicom/";
            m_dicomReader->set_folder(path.string() + selected_series_uid + "/");
            m_dicomReader->update();

            // Merge series.
            if(!m_dicomReader->hasFailed() && !m_series_set->empty())
            {
                this->notifier::success("Series read");

                // Add the series to the local series vector.
                m_localSeries.insert(selected_series_uid);

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

void series_puller::removeSeries(data::series_set::container_type _removed_series)
{
    // Find which series to delete
    if(!m_localSeries.empty())
    {
        for(const auto& series : _removed_series)
        {
            if(m_localSeries.erase(series->getSeriesInstanceUID()) > 0)
            {
                this->notifier::info("Local series deleted");
            }
        }
    }
}

//------------------------------------------------------------------------------

void series_puller::storeInstanceCallback(
    const std::string& _series_instance_uid,
    unsigned int _instance_number,
    const std::string& _file_path
)
{
    // Add path in the DICOM series.
    if(!m_pullingDicomSeriesMap[_series_instance_uid].expired())
    {
        data::dicom_series::sptr series = m_pullingDicomSeriesMap[_series_instance_uid].lock();
        series->addDicomPath(_instance_number, _file_path);
    }
    else
    {
        SIGHT_WARN("The Dicom Series " + _series_instance_uid + " has expired.");
    }

    // Notify progress dialog.
    std::stringstream ss;
    ss << "Downloading file " << _instance_number << "/" << m_instanceCount;
    float percentage = static_cast<float>(_instance_number) / static_cast<float>(m_instanceCount);
    m_sigProgressed->async_emit(m_progressbarId, percentage, ss.str());
}

//------------------------------------------------------------------------------

service::connections_t series_puller::auto_connections() const
{
    connections_t connections;
    connections.push(s_SERIES_SET_INOUT, data::series_set::REMOVED_OBJECTS_SIG, REMOVE_SERIES_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse.
