/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "ioPacs/SSeriesPuller.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/exceptions/Base.hpp>
#include <fwPacsIO/helper/Series.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwTools/System.hpp>

#include <filesystem>
#include <sstream>

namespace ioPacs
{

static const ::fwCom::Signals::SignalKeyType s_PROGRESSED_SIG       = "progressed";
static const ::fwCom::Signals::SignalKeyType s_STARTED_PROGRESS_SIG = "progressStarted";
static const ::fwCom::Signals::SignalKeyType s_STOPPED_PROGRESS_SIG = "progressStopped";

static const ::fwCom::Slots::SlotKeyType s_REMOVE_SERIES_SLOT = "removeSeries";

static const std::string s_DICOM_READER_CONFIG = "dicomReader";
static const std::string s_READER_CONFIG       = "readerConfig";

static const ::fwServices::IService::KeyType s_PACS_INPUT     = "pacsConfig";
static const ::fwServices::IService::KeyType s_SELECTED_INPUT = "selectedSeries";

static const ::fwServices::IService::KeyType s_SERIES_DB_INOUT = "seriesDB";

SSeriesPuller::SSeriesPuller() noexcept
{
    m_slotStoreInstanceCallbackUsingMoveRequests = this->newSlot(::fwPacsIO::SeriesRetriever::s_PROGRESS_CALLBACK_SLOT,
                                                                 &SSeriesPuller::storeInstanceCallback, this);

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
    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_dicomReaderImplementation = config.get(s_DICOM_READER_CONFIG, m_dicomReaderImplementation);
    OSLM_ERROR_IF("'" + s_DICOM_READER_CONFIG + "' attribute not set", m_dicomReaderImplementation.empty())

    m_readerConfig = configType.get(s_READER_CONFIG, m_readerConfig);
}

//------------------------------------------------------------------------------

void SSeriesPuller::starting()
{
    // Create the DICOM reader.
    m_seriesDB = ::fwMedData::SeriesDB::New();

    m_dicomReader = this->registerService< ::fwIO::IReader >(m_dicomReaderImplementation);
    SLM_ASSERT("Unable to create a reader of type '" + m_dicomReaderImplementation + "'", m_dicomReader);
    m_dicomReader->setWorker(this->getWorker());
    m_dicomReader->registerInOut(m_seriesDB, "data");
    if(!m_readerConfig.empty())
    {
        ::fwRuntime::ConfigurationElement::csptr readerConfig =
            ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
                m_readerConfig, "::fwIO::IReader");

        SLM_ASSERT("No service configuration " << m_readerConfig << " for ::fwIO::IReader", readerConfig);

        m_dicomReader->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(readerConfig) );
    }

    m_dicomReader->configure();
    m_dicomReader->start();
    SLM_ASSERT("'" + m_dicomReaderImplementation + "' is not started", m_dicomReader->isStarted());
}

//------------------------------------------------------------------------------

void SSeriesPuller::updating()
{
    const auto selectedSeries = this->getLockedInput< const ::fwData::Vector >(s_SELECTED_INPUT);

    if(selectedSeries->empty())
    {
        const auto notif = this->signal< ::fwServices::IService::InfoNotifiedSignalType >(
            ::fwServices::IService::s_INFO_NOTIFIED_SIG);
        notif->asyncEmit("No series selected");
    }
    else
    {
        this->pullSeries();
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::stopping()
{
    // Unregister the DICOM reader.
    this->unregisterServices();
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
    const auto selectedSeries = this->getLockedInput< const ::fwData::Vector >(s_SELECTED_INPUT);
    const auto localEnd       = m_localSeries.end();

    // Find which selected series must be pulled.
    DicomSeriesContainerType pullSeriesVector;
    DicomSeriesContainerType selectedSeriesVector;
    ::fwData::Vector::ConstIteratorType it = selectedSeries->begin();
    const ::fwData::Vector::ConstIteratorType itEnd = selectedSeries->end();
    for(; it != itEnd; ++it)
    {
        // Check that the series is a DICOM series.
        ::fwMedData::DicomSeries::sptr series = ::fwMedData::DicomSeries::dynamicCast(*it);

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
        const auto infoNotif = this->signal< ::fwServices::IService::InfoNotifiedSignalType >(
            ::fwServices::IService::s_INFO_NOTIFIED_SIG);
        infoNotif->asyncEmit("Downloading series...");

        // Notify Progress Dialog.
        m_sigProgressStarted->asyncEmit(m_progressbarId);

        // Retrieve informations.
        const auto pacsConfig = this->getLockedInput< const ::fwPacsIO::data::PacsConfiguration >(s_PACS_INPUT);

        ::fwPacsIO::SeriesEnquirer::sptr seriesEnquirer = ::fwPacsIO::SeriesEnquirer::New();

        // Initialize connection.
        try
        {
            seriesEnquirer->initialize(
                pacsConfig->getLocalApplicationTitle(),
                pacsConfig->getPacsHostName(),
                pacsConfig->getPacsApplicationPort(),
                pacsConfig->getPacsApplicationTitle(),
                pacsConfig->getMoveApplicationTitle());
            seriesEnquirer->connect();
        }
        catch(const ::fwPacsIO::exceptions::Base& _e)
        {
            SLM_ERROR("Unable to establish a connection with the PACS: " + std::string(_e.what()));
            const auto failureNotif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
                ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
            failureNotif->asyncEmit("Unable to connect to the PACS");
            return;
        }

        ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

        try
        {
            if(pacsConfig->getRetrieveMethod() == ::fwPacsIO::data::PacsConfiguration::GET_RETRIEVE_METHOD)
            {
                seriesEnquirer->pullSeriesUsingGetRetrieveMethod(::fwPacsIO::helper::Series::toSeriesInstanceUIDContainer(
                                                                     pullSeriesVector));
            }
            else if(pacsConfig->getRetrieveMethod() ==
                    ::fwPacsIO::data::PacsConfiguration::MOVE_RETRIEVE_METHOD)
            {
                ::fwPacsIO::SeriesRetriever::sptr seriesRetriever = ::fwPacsIO::SeriesRetriever::New();
                seriesRetriever->initialize(
                    pacsConfig->getMoveApplicationTitle(),
                    pacsConfig->getMoveApplicationPort(),
                    1,
                    m_slotStoreInstanceCallbackUsingMoveRequests);

                // Start series retriever in a worker.
                worker->post(std::bind(&::fwPacsIO::SeriesRetriever::start, seriesRetriever));

                // Pull Selected Series.
                seriesEnquirer->pullSeriesUsingMoveRetrieveMethod(::fwPacsIO::helper::Series::toSeriesInstanceUIDContainer(
                                                                      pullSeriesVector));
            }
            else
            {
                SLM_ERROR("Unknown retrieve method, 'get' will be used");
                seriesEnquirer->pullSeriesUsingGetRetrieveMethod(::fwPacsIO::helper::Series::toSeriesInstanceUIDContainer(
                                                                     pullSeriesVector));
            }
        }
        catch(const ::fwPacsIO::exceptions::Base& _e)
        {
            SLM_ERROR("Unable to execute query to the PACS: " + std::string(_e.what()));
            const auto failureNotif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
                ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
            failureNotif->asyncEmit("Unable to execute query");

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

        // Notify Progress Dialog.
        m_sigProgressStopped->asyncEmit(m_progressbarId);
    }
    else
    {
        const auto infoNotif = this->signal< ::fwServices::IService::InfoNotifiedSignalType >(
            ::fwServices::IService::s_INFO_NOTIFIED_SIG);
        infoNotif->asyncEmit("Series already downloaded");

        return;
    }

    // Read series if there is no error.
    if(success)
    {
        const auto sucessNotif = this->signal< ::fwServices::IService::SuccessNotifiedSignalType >(
            ::fwServices::IService::s_SUCCESS_NOTIFIED_SIG);
        sucessNotif->asyncEmit("Series downloaded");

        this->readLocalSeries(selectedSeriesVector);
    }
    else
    {
        const auto failNotif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
            ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
        failNotif->asyncEmit("Series download failed");
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::readLocalSeries(DicomSeriesContainerType _selectedSeries)
{
    const auto destinationSeriesDB = this->getLockedInOut< ::fwMedData::SeriesDB >(s_SERIES_DB_INOUT);

    // Read only series that are not in the series DB.
    std::vector< std::string > alreadyLoadedSeries =
        ::fwPacsIO::helper::Series::toSeriesInstanceUIDContainer(destinationSeriesDB->getContainer());

    // Create temporary series helper.
    ::fwMedDataTools::helper::SeriesDB readerSeriesHelper(m_seriesDB);

    const auto infoNotif = this->signal< ::fwServices::IService::InfoNotifiedSignalType >(
        ::fwServices::IService::s_INFO_NOTIFIED_SIG);
    const auto failNotif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
        ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
    const auto sucessNotif = this->signal< ::fwServices::IService::SuccessNotifiedSignalType >(
        ::fwServices::IService::s_SUCCESS_NOTIFIED_SIG);

    for(const ::fwMedData::Series::sptr& series: _selectedSeries)
    {
        const std::string selectedSeriesUID = series->getInstanceUID();

        // Check if the series is loaded.
        if(std::find(alreadyLoadedSeries.begin(), alreadyLoadedSeries.end(),
                     selectedSeriesUID) == alreadyLoadedSeries.end())
        {
            infoNotif->asyncEmit("Reading serie...");

            // Clear temporary series.
            readerSeriesHelper.clear();

            std::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "dicom/";
            m_dicomReader->setFolder(path.string() + selectedSeriesUID + "/");
            m_dicomReader->update();

            // Merge series.
            if(!m_dicomReader->hasFailed() && m_seriesDB->getContainer().size() > 0)
            {
                sucessNotif->asyncEmit("Serie read");

                // Add the series to the local series vector.
                if(std::find(m_localSeries.begin(), m_localSeries.end(), selectedSeriesUID) == m_localSeries.end())
                {
                    m_localSeries.push_back(selectedSeriesUID);
                }

                ::fwMedDataTools::helper::SeriesDB seriesHelper(destinationSeriesDB.get_shared());
                seriesHelper.merge(m_seriesDB);
                seriesHelper.notify();
            }
            else
            {
                failNotif->asyncEmit("Serie read failed");
            }
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::removeSeries( ::fwMedData::SeriesDB::ContainerType _removedSeries)
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

                const auto infoNotif = this->signal< ::fwServices::IService::InfoNotifiedSignalType >(
                    ::fwServices::IService::s_INFO_NOTIFIED_SIG);
                infoNotif->asyncEmit("Local series deleted");
            }
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::storeInstanceCallback(const ::std::string& _seriesInstanceUID, unsigned int _instanceNumber,
                                          const ::std::string& _filePath)
{
    // Add path in the DICOM series.
    if(!m_pullingDicomSeriesMap[_seriesInstanceUID].expired())
    {
        ::fwMedData::DicomSeries::sptr series = m_pullingDicomSeriesMap[_seriesInstanceUID].lock();
        series->addDicomPath(_instanceNumber, _filePath);
    }
    else
    {
        SLM_WARN("The Dicom Series " + _seriesInstanceUID + " has expired.");
    }

    // Notify progress dialog.
    ::std::stringstream ss;
    ss << "Downloading file " << _instanceNumber << "/" << m_instanceCount;
    float percentage = static_cast<float>(_instanceNumber)/static_cast<float>(m_instanceCount);
    m_sigProgressed->asyncEmit(m_progressbarId, percentage, ss.str());
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SSeriesPuller::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_SERIES_DB_INOUT, ::fwMedData::SeriesDB::s_REMOVED_SERIES_SIG, s_REMOVE_SERIES_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace ioPacs.
