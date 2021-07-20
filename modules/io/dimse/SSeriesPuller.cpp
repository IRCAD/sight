/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SSeriesPuller.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/tools/System.hpp>

#include <data/helper/SeriesDB.hpp>

#include <io/dimse/data/PacsConfiguration.hpp>
#include <io/dimse/exceptions/Base.hpp>
#include <io/dimse/helper/Series.hpp>
#include <io/dimse/SeriesEnquirer.hpp>

#include <service/extension/Config.hpp>
#include <service/macros.hpp>

#include <filesystem>
#include <sstream>

namespace sight::module::io::dimse
{

static const core::com::Signals::SignalKeyType s_PROGRESSED_SIG       = "progressed";
static const core::com::Signals::SignalKeyType s_STARTED_PROGRESS_SIG = "progressStarted";
static const core::com::Signals::SignalKeyType s_STOPPED_PROGRESS_SIG = "progressStopped";

static const core::com::Slots::SlotKeyType s_REMOVE_SERIES_SLOT = "removeSeries";

static const std::string s_DICOM_READER_CONFIG = "dicomReader";
static const std::string s_READER_CONFIG       = "readerConfig";

static const service::IService::KeyType s_PACS_INPUT     = "pacsConfig";
static const service::IService::KeyType s_SELECTED_INPUT = "selectedSeries";

static const service::IService::KeyType s_SERIES_DB_INOUT = "seriesDB";

SSeriesPuller::SSeriesPuller() noexcept
{
    m_slotStoreInstance = this->newSlot(
        sight::io::dimse::SeriesRetriever::s_PROGRESS_CALLBACK_SLOT,
        &SSeriesPuller::storeInstanceCallback,
        this
    );

    m_sigProgressed      = this->newSignal<ProgressedSignalType>(s_PROGRESSED_SIG);
    m_sigProgressStarted = this->newSignal<ProgressStartedSignalType>(s_STARTED_PROGRESS_SIG);
    m_sigProgressStopped = this->newSignal<ProgressStoppedSignalType>(s_STOPPED_PROGRESS_SIG);

    newSlot(s_REMOVE_SERIES_SLOT, &SSeriesPuller::removeSeries, this);
}

//------------------------------------------------------------------------------

SSeriesPuller::~SSeriesPuller() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesPuller::configuring()
{
    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_dicomReaderImplementation = config.get(s_DICOM_READER_CONFIG, m_dicomReaderImplementation);
    SIGHT_ERROR_IF("'" + s_DICOM_READER_CONFIG + "' attribute not set", m_dicomReaderImplementation.empty())

    m_readerConfig = config.get(s_READER_CONFIG, m_readerConfig);
}

//------------------------------------------------------------------------------

void SSeriesPuller::starting()
{
    // Create the worker.
    m_requestWorker = core::thread::Worker::New();

    // Create the DICOM reader.
    m_seriesDB = data::SeriesDB::New();

    m_dicomReader = this->registerService<sight::io::base::service::IReader>(m_dicomReaderImplementation);
    SIGHT_ASSERT("Unable to create a reader of type '" + m_dicomReaderImplementation + "'", m_dicomReader);
    m_dicomReader->setWorker(m_requestWorker);
    m_dicomReader->setInOut(m_seriesDB, sight::io::base::service::s_DATA_KEY);
    if(!m_readerConfig.empty())
    {
        core::runtime::ConfigurationElement::csptr readerConfig =
            service::extension::Config::getDefault()->getServiceConfig(
                m_readerConfig,
                "sight::io::base::service::IReader"
            );

        SIGHT_ASSERT(
            "No service configuration " << m_readerConfig << " for sight::io::base::service::IReader",
            readerConfig
        );

        m_dicomReader->setConfiguration(core::runtime::ConfigurationElement::constCast(readerConfig));
    }

    m_dicomReader->configure();
    m_dicomReader->start().wait();
    SIGHT_ASSERT("'" + m_dicomReaderImplementation + "' is not started", m_dicomReader->isStarted());
}

//------------------------------------------------------------------------------

void SSeriesPuller::updating()
{
    const auto selectedSeries = this->getLockedInput<const data::Vector>(s_SELECTED_INPUT);

    if(selectedSeries->empty())
    {
        this->notify(NotificationType::INFO, "No series selected");
    }
    else
    {
        m_requestWorker->post(std::bind(&SSeriesPuller::pullSeries, this));
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::stopping()
{
    // Unregister the DICOM reader.
    this->unregisterServices();

    // Stop the worker.
    m_requestWorker->stop();
    m_requestWorker.reset();
}

//------------------------------------------------------------------------------

void SSeriesPuller::pullSeries()
{
    // Set pulling boolean to true.
    bool success = true;

    // Clear map of Dicom series being pulled.
    m_pullingDicomSeriesMap.clear();

    // Reset Counters
    m_instanceCount = 0;

    // Retrieve data.
    const auto selectedSeries = this->getLockedInput<const data::Vector>(s_SELECTED_INPUT);
    const auto localEnd       = m_localSeries.end();

    // Find which selected series must be pulled.
    DicomSeriesContainerType pullSeriesVector;
    DicomSeriesContainerType selectedSeriesVector;
    data::Vector::ConstIteratorType it          = selectedSeries->begin();
    const data::Vector::ConstIteratorType itEnd = selectedSeries->end();
    for( ; it != itEnd ; ++it)
    {
        // Check that the series is a DICOM series.
        data::DicomSeries::sptr series = data::DicomSeries::dynamicCast(*it);

        // Check if the series must be pulled.
        if(series && std::find(m_localSeries.begin(), m_localSeries.end(), series->getInstanceUID()) == localEnd)
        {
            // Add series in the pulling series map.
            m_pullingDicomSeriesMap[series->getInstanceUID()] = series;

            pullSeriesVector.push_back(series);
            m_instanceCount += series->getNumberOfInstances();
        }

        selectedSeriesVector.push_back(series);
    }

    // Pull series.
    if(!pullSeriesVector.empty())
    {
        this->notify(NotificationType::INFO, "Downloading series...");

        // Notify Progress Dialog.
        m_sigProgressStarted->asyncEmit(m_progressbarId);

        // Retrieve informations.
        const auto pacsConfig = this->getLockedInput<const sight::io::dimse::data::PacsConfiguration>(s_PACS_INPUT);

        auto seriesEnquirer = sight::io::dimse::SeriesEnquirer::New();

        // Initialize connection.
        try
        {
            seriesEnquirer->initialize(
                pacsConfig->getLocalApplicationTitle(),
                pacsConfig->getPacsHostName(),
                pacsConfig->getPacsApplicationPort(),
                pacsConfig->getPacsApplicationTitle(),
                pacsConfig->getMoveApplicationTitle()
            );
            seriesEnquirer->connect();
        }
        catch(const sight::io::dimse::exceptions::Base& _e)
        {
            SIGHT_ERROR("Unable to establish a connection with the PACS: " + std::string(_e.what()));
            this->notify(NotificationType::FAILURE, "Unable to connect to the PACS");
            return;
        }

        core::thread::Worker::sptr worker = core::thread::Worker::New();

        try
        {
            using sight::io::dimse::helper::Series;
            if(pacsConfig->getRetrieveMethod() == sight::io::dimse::data::PacsConfiguration::GET_RETRIEVE_METHOD)
            {
                seriesEnquirer->pullSeriesUsingGetRetrieveMethod(
                    Series::toSeriesInstanceUIDContainer(
                        pullSeriesVector
                    )
                );
            }
            else if(pacsConfig->getRetrieveMethod()
                    == sight::io::dimse::data::PacsConfiguration::MOVE_RETRIEVE_METHOD)
            {
                auto seriesRetriever = sight::io::dimse::SeriesRetriever::New();
                seriesRetriever->initialize(
                    pacsConfig->getMoveApplicationTitle(),
                    pacsConfig->getMoveApplicationPort(),
                    1,
                    m_slotStoreInstance
                );

                // Start series retriever in a worker.
                worker->post(std::bind(&sight::io::dimse::SeriesRetriever::start, seriesRetriever));

                // Pull Selected Series.
                seriesEnquirer->pullSeriesUsingMoveRetrieveMethod(
                    Series::toSeriesInstanceUIDContainer(
                        pullSeriesVector
                    )
                );
            }
            else
            {
                SIGHT_ERROR("Unknown retrieve method, 'get' will be used");
                seriesEnquirer->pullSeriesUsingGetRetrieveMethod(
                    Series::toSeriesInstanceUIDContainer(
                        pullSeriesVector
                    )
                );
            }
        }
        catch(const sight::io::dimse::exceptions::Base& _e)
        {
            SIGHT_ERROR("Unable to execute query to the PACS: " + std::string(_e.what()));
            this->notify(NotificationType::FAILURE, "Unable to execute query");
            success = false;
        }

        // Stop the worker.
        worker->stop();
        worker.reset();

        // Disconnect the series enquirer.
        if(seriesEnquirer->isConnectedToPacs())
        {
            seriesEnquirer->disconnect();
        }
    }
    else
    {
        this->notify(NotificationType::INFO, "Series already downloaded");
        return;
    }

    // Read series if there is no error.
    if(success)
    {
        this->notify(NotificationType::SUCCESS, "Series downloaded");
        this->readLocalSeries(selectedSeriesVector);
    }
    else
    {
        this->notify(NotificationType::FAILURE, "Series download failed");
    }

    // Notify Progress Dialog.
    m_sigProgressStopped->asyncEmit(m_progressbarId);
}

//------------------------------------------------------------------------------

void SSeriesPuller::readLocalSeries(DicomSeriesContainerType _selectedSeries)
{
    const auto destinationSeriesDB = this->getLockedInOut<data::SeriesDB>(s_SERIES_DB_INOUT);

    // Read only series that are not in the series DB.
    std::vector<std::string> alreadyLoadedSeries =
        sight::io::dimse::helper::Series::toSeriesInstanceUIDContainer(destinationSeriesDB->getContainer());

    // Create temporary series helper.
    data::helper::SeriesDB readerSeriesHelper(*m_seriesDB);

    for(const data::Series::sptr& series : _selectedSeries)
    {
        const std::string modality = series->getModality();
        if(modality != "CT" && modality != "MR" && modality != "XA")
        {
            this->notify(NotificationType::INFO, "Unable to read the modality '" + modality + "'");
            return;
        }

        const std::string selectedSeriesUID = series->getInstanceUID();

        // Check if the series is loaded.
        if(std::find(
               alreadyLoadedSeries.begin(),
               alreadyLoadedSeries.end(),
               selectedSeriesUID
           ) == alreadyLoadedSeries.end())
        {
            this->notify(NotificationType::INFO, "Reading series...");

            // Clear temporary series.
            readerSeriesHelper.clear();

            std::filesystem::path path = core::tools::System::getTemporaryFolder() / "dicom/";
            m_dicomReader->setFolder(path.string() + selectedSeriesUID + "/");
            m_dicomReader->update();

            // Merge series.
            if(!m_dicomReader->hasFailed() && m_seriesDB->getContainer().size() > 0)
            {
                this->notify(NotificationType::SUCCESS, "Series read");

                // Add the series to the local series vector.
                if(std::find(m_localSeries.begin(), m_localSeries.end(), selectedSeriesUID) == m_localSeries.end())
                {
                    m_localSeries.push_back(selectedSeriesUID);
                }

                data::helper::SeriesDB seriesHelper(*destinationSeriesDB);
                seriesHelper.merge(m_seriesDB);
                seriesHelper.notify();
            }
            else
            {
                this->notify(NotificationType::FAILURE, "Failed to read series");
            }
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::removeSeries(data::SeriesDB::ContainerType _removedSeries)
{
    // Find which series to delete
    if(!m_localSeries.empty())
    {
        for(const auto series : _removedSeries)
        {
            const auto it = std::find(m_localSeries.begin(), m_localSeries.end(), series->getInstanceUID());
            if(it != m_localSeries.end())
            {
                m_localSeries.erase(it);

                this->notify(NotificationType::INFO, "Local series deleted");
            }
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::storeInstanceCallback(
    const ::std::string& _seriesInstanceUID,
    unsigned int _instanceNumber,
    const ::std::string& _filePath
)
{
    // Add path in the DICOM series.
    if(!m_pullingDicomSeriesMap[_seriesInstanceUID].expired())
    {
        data::DicomSeries::sptr series = m_pullingDicomSeriesMap[_seriesInstanceUID].lock();
        series->addDicomPath(_instanceNumber, _filePath);
    }
    else
    {
        SIGHT_WARN("The Dicom Series " + _seriesInstanceUID + " has expired.");
    }

    // Notify progress dialog.
    ::std::stringstream ss;
    ss << "Downloading file " << _instanceNumber << "/" << m_instanceCount;
    float percentage = static_cast<float>(_instanceNumber) / static_cast<float>(m_instanceCount);
    m_sigProgressed->asyncEmit(m_progressbarId, percentage, ss.str());
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SSeriesPuller::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_SERIES_DB_INOUT, data::SeriesDB::s_REMOVED_SERIES_SIG, s_REMOVE_SERIES_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse.
