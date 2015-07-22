/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGuiQt/container/QtContainer.hpp>
#include <fwDicomData/DicomSeries.hpp>
#include <fwDicomIOExt/dcmtk/helper/Series.hpp>
#include <fwDicomIOExt/exceptions/Base.hpp>
#include <fwComEd/SeriesDBMsg.hpp>
#include <fwComEd/helper/SeriesDB.hpp>
#include <fwTools/System.hpp>

#include "ioDicomExt/common/data/ProgressMsg.hpp"
#include "ioDicomExt/common/controller/SProgressBarController.hpp"
#include "ioDicomExt/dcmtk/controller/SSeriesPuller.hpp"

namespace ioDicomExt
{

namespace dcmtk
{

namespace controller
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioDicomExt::dcmtk::controller::SSeriesPuller, ::fwData::Vector );

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SSeriesPuller::s_READ_SLOT    = "readDicom";
const ::fwCom::Slots::SlotKeyType SSeriesPuller::s_DISPLAY_SLOT = "displayMessage";

const ::fwCom::Signals::SignalKeyType SSeriesPuller::s_PROGRESSED_SIG = "progressed";

SSeriesPuller::SSeriesPuller() throw() :
    m_isPulling(false),
    m_progressbarId("pullDicomProgressBar"),
    m_seriesCount(0),
    m_seriesIndex(0)
{

    m_slotReadLocalSeries = ::fwCom::newSlot(&SSeriesPuller::readLocalSeries, this);
    ::fwCom::HasSlots::m_slots(s_READ_SLOT, m_slotReadLocalSeries);

    m_slotDisplayMessage = ::fwCom::newSlot(&SSeriesPuller::displayErrorMessage, this);
    ::fwCom::HasSlots::m_slots(s_DISPLAY_SLOT, m_slotDisplayMessage);

    m_slotStoreInstanceCallbackUsingMoveRequests = ::fwCom::newSlot(&SSeriesPuller::storeInstanceCallback, this);
    ::fwCom::HasSlots::m_slots(::fwDicomIOExt::dcmtk::SeriesRetriever::s_STORE_INSTANCE_CALLBACK_SLOT,
                               m_slotStoreInstanceCallbackUsingMoveRequests);

    m_slotStoreInstanceCallbackUsingGetRequests = ::fwCom::newSlot(&SSeriesPuller::storeInstanceCallback, this);
    ::fwCom::HasSlots::m_slots(::fwDicomIOExt::dcmtk::SeriesEnquirer::s_STORE_INSTANCE_CALLBACK_SLOT,
                               m_slotStoreInstanceCallbackUsingGetRequests);

    m_slotProgressCallback = ::fwCom::newSlot(&SSeriesPuller::progressCallback, this);
    ::fwCom::HasSlots::m_slots(::fwDicomIOExt::dcmtk::SeriesEnquirer::s_PROGRESS_CALLBACK_SLOT, m_slotProgressCallback);

    m_sigProgressBar = ProgressBarSignalType::New();
    m_signals( s_PROGRESSED_SIG, m_sigProgressBar);

#ifdef COM_LOG
    m_slotReadLocalSeries->setID( s_READ_SLOT );
    m_slotDisplayMessage->setID( s_DISPLAY_SLOT );
    m_slotStoreInstanceCallbackUsingMoveRequests->setID(
        ::fwDicomIOExt::dcmtk::SeriesRetriever::s_STORE_INSTANCE_CALLBACK_SLOT );
    m_slotStoreInstanceCallbackUsingGetRequests->setID(
        ::fwDicomIOExt::dcmtk::SeriesEnquirer::s_STORE_INSTANCE_CALLBACK_SLOT );
    m_slotProgressCallback->setID( ::fwDicomIOExt::dcmtk::SeriesEnquirer::s_PROGRESS_CALLBACK_SLOT );
    ::fwCom::HasSignals::m_signals.setID();
#endif

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

}
//------------------------------------------------------------------------------

SSeriesPuller::~SSeriesPuller() throw()
{
}

//------------------------------------------------------------------------------

void SSeriesPuller::info(std::ostream &_sstream )
{
    _sstream << "SSeriesPuller::info";
}

//------------------------------------------------------------------------------

void SSeriesPuller::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Create enquirer
    m_seriesEnquirer = ::fwDicomIOExt::dcmtk::SeriesEnquirer::New();

    // Get Destination SeriesDB
    m_destinationSeriesDB = ::fwMedData::SeriesDB::dynamicCast(::fwTools::fwID::getObject(m_destinationSeriesDBUID));
    SLM_ASSERT("The SeriesDB \"" + m_destinationSeriesDBUID + "\" doesn't exist.", m_destinationSeriesDB);

    // Create temporary SeriesDB
    m_tempSeriesDB = ::fwMedData::SeriesDB::New();

    // Create reader
    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();
    m_dicomReader                                           =
        ::io::IReader::dynamicCast(srvFactory->create(m_dicomReaderType));
    SLM_ASSERT(
        "Unable to create a reader of type: \"" + m_dicomReaderType + "\" in ::ioDicomExt::dcmtk::controller::SSeriesPuller.",
        m_dicomReader);
    ::fwServices::OSR::registerService(m_tempSeriesDB, m_dicomReader);
    m_dicomReader->configure();
    m_dicomReader->start();

    // Worker
    m_pullSeriesWorker = ::fwThread::Worker::New();

    // Get pacs configuration
    m_pacsConfiguration =
        ::fwDicomIOExt::data::PacsConfiguration::dynamicCast(::fwTools::fwID::getObject(m_pacsConfigurationUID));
    SLM_ASSERT("The pacs configuration object should not be null.", m_pacsConfiguration);

}

//------------------------------------------------------------------------------

void SSeriesPuller::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Stop reader service
    m_dicomReader->stop();
    ::fwServices::OSR::unregisterService(m_dicomReader);

    // Worker
    m_pullSeriesWorker.reset();

}

//------------------------------------------------------------------------------

void SSeriesPuller::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioDicomExt::dcmtk::controller::SSeriesPuller must have a \"config\" element.",config);

    bool success;

    // Pacs Configuration UID
    ::boost::tie(success, m_pacsConfigurationUID) = config->getSafeAttributeValue("pacsConfigurationUID");
    SLM_ASSERT(
        "It should be a \"pacsConfigurationUID\" tag in the ::ioDicomExt::dcmtk::editor::SQueryEditor config element.",
        success);

    // IODICOMEXT Reader
    ::boost::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" in the ::ioDicomExt::dcmtk::controller::SSeriesPuller config element.",
               success);

    // Destination Series DB ID
    ::boost::tie(success, m_destinationSeriesDBUID) = config->getSafeAttributeValue("destinationSeriesDBUID");
    SLM_ASSERT(
        "It should be a \"destinationSeriesDBUID\" in the ::ioDicomExt::dcmtk::controller::SSeriesPuller config element.",
        success);

}

//------------------------------------------------------------------------------

void SSeriesPuller::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesPuller::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Vector::sptr selectedSeries = this->getObject< ::fwData::Vector >();

    if(m_isPulling)
    {
        // Display a message to inform the user that the service is already pulling data.
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pulling Series");
        messageBox.setMessage( "The service is already pulling data. Please wait until the pulling is done "
                               "before sending a new pull request." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();
    }
    else if(selectedSeries->empty())
    {
        // Display a message to inform the user that there is no series selected.
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pulling Series");
        messageBox.setMessage( "Unable to pull series, there is no series selected." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();
    }
    else
    {
        m_pullSeriesWorker->post(::boost::bind(&::ioDicomExt::dcmtk::controller::SSeriesPuller::pullSeries, this));
    }

}

//------------------------------------------------------------------------------

void SSeriesPuller::pullSeries()
{
    // Catch any errors
    try
    {
        // Clear map of Dicom series being pulled
        m_pullingDicomSeriesMap.clear();

        // Set pulling boolean to true
        m_isPulling = true;

        // Reset Counter
        m_seriesIndex = 0;

        ::fwData::Vector::sptr seriesVector = this->getObject< ::fwData::Vector >();

        // Find which selected series must be pulled
        DicomSeriesContainerType pullSeriesVector;
        DicomSeriesContainerType selectedSeriesVector;

        ::fwData::Vector::IteratorType it = seriesVector->begin();
        for(; it != seriesVector->end(); ++it)
        {
            ::fwDicomData::DicomSeries::sptr series = ::fwDicomData::DicomSeries::dynamicCast(*it);

            // Check if the series must be pulled
            if(series &&
               std::find(m_localSeries.begin(), m_localSeries.end(),
                         series->getInstanceUID()) == m_localSeries.end())
            {
                // Add series in the pulling series map
                m_pullingDicomSeriesMap[series->getInstanceUID()] = series;

                pullSeriesVector.push_back(series);
                m_instanceCountMap[series->getInstanceUID()] = series->getNumberOfInstances();
            }
            selectedSeriesVector.push_back(series);
        }

        m_seriesCount = pullSeriesVector.size();

        if(m_seriesCount > 0)
        {
            //Notify Progress Dialog
            this->notifyProgressBar(::ioDicomExt::common::data::ProgressMsg::s_START_PROGRESS_BAR);
        }

        // Pull series
        if(!pullSeriesVector.empty())
        {
            m_seriesEnquirer->initialize(
                m_pacsConfiguration->getLocalApplicationTitle(),
                m_pacsConfiguration->getPacsHostName(),
                m_pacsConfiguration->getPacsApplicationPort(),
                m_pacsConfiguration->getPacsApplicationTitle(),
                m_pacsConfiguration->getMoveApplicationTitle(),
                m_slotStoreInstanceCallbackUsingGetRequests,
                m_slotProgressCallback);

            // Use C-GET Requests
            if(m_pacsConfiguration->getRetrieveMethod() == ::fwDicomIOExt::data::PacsConfiguration::GET_RETRIEVE_METHOD)
            {
                // Pull Selected Series
                m_seriesEnquirer->connect();
                m_seriesEnquirer->pullSeriesUsingGetRetrieveMethod(::fwDicomIOExt::dcmtk::helper::Series::toSeriesInstanceUIDContainer(
                                                                       pullSeriesVector));
                m_seriesEnquirer->disconnect();
            }
            // Use C-MOVE Requests
            else
            {
                ::fwDicomIOExt::dcmtk::SeriesRetriever::sptr seriesRetriever =
                    ::fwDicomIOExt::dcmtk::SeriesRetriever::New();
                seriesRetriever->initialize(
                    m_pacsConfiguration->getMoveApplicationTitle(),
                    m_pacsConfiguration->getMoveApplicationPort(),
                    1,
                    m_slotStoreInstanceCallbackUsingMoveRequests);

                // Start Series Retriever
                ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
                worker->post(::boost::bind(&::fwDicomIOExt::dcmtk::SeriesRetriever::start, seriesRetriever));

                // Pull Selected Series
                m_seriesEnquirer->connect();
                m_seriesEnquirer->pullSeriesUsingMoveRetrieveMethod(::fwDicomIOExt::dcmtk::helper::Series::toSeriesInstanceUIDContainer(
                                                                        pullSeriesVector));
                m_seriesEnquirer->disconnect();

                worker.reset();
            }

            // Notify Progress Dialog
            this->notifyProgressBar(::ioDicomExt::common::data::ProgressMsg::s_STOP_PROGRESS_BAR);
        }

        // Read series if there is no error
        if(m_isPulling)
        {
            m_slotReadLocalSeries->asyncRun(selectedSeriesVector);
        }

        // Clear instance count map
        m_instanceCountMap.clear();

        // Set pulling boolean to false
        m_isPulling = false;

    }
    catch (::fwDicomIOExt::exceptions::Base& exception)
    {
        ::std::stringstream ss;
        ss << "Unable to connect to the pacs. Please check your configuration: \n"
           << "Pacs host name: " << m_pacsConfiguration->getPacsHostName() << "\n"
           << "Pacs application title: " << m_pacsConfiguration->getPacsApplicationTitle() << "\n"
           << "Pacs port: " << m_pacsConfiguration->getPacsApplicationPort() << "\n";
        m_slotDisplayMessage->asyncRun(ss.str());
        SLM_WARN(exception.what());
        //Notify Progress Dialog
        this->notifyProgressBar(::ioDicomExt::common::data::ProgressMsg::s_STOP_PROGRESS_BAR);
        m_isPulling = false;
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::readLocalSeries(DicomSeriesContainerType selectedSeries)
{
    // Read only series that are not in the SeriesDB
    InstanceUIDContainerType alreadyLoadedSeries =
        ::fwDicomIOExt::dcmtk::helper::Series::toSeriesInstanceUIDContainer(m_destinationSeriesDB->getContainer());

    // Create temporary series helper
    ::fwComEd::helper::SeriesDB tempSDBhelper(m_tempSeriesDB);

    BOOST_FOREACH(const ::fwMedData::Series::sptr& series, selectedSeries)
    {
        ::fwDicomData::DicomSeries::sptr dicomSeries = ::fwDicomData::DicomSeries::dynamicCast(series);
        dicomSeries->setDicomAvailability(::fwDicomData::DicomSeries::PATHS);

        ::std::string selectedSeriesUID = series->getInstanceUID();

        // Add the series to the local series vector
        if(::std::find(m_localSeries.begin(), m_localSeries.end(), selectedSeriesUID) == m_localSeries.end())
        {
            m_localSeries.push_back(selectedSeriesUID);
        }

        // Check if the series is loaded
        if(::std::find(alreadyLoadedSeries.begin(), alreadyLoadedSeries.end(),
                       selectedSeriesUID) == alreadyLoadedSeries.end())
        {
            // Clear temporary series
            tempSDBhelper.clear();

            ::boost::filesystem::path path = ::fwTools::System::getTemporaryFolder() / "dicom/";
            m_dicomReader->setFolder(path.string() + selectedSeriesUID + "/");
            m_dicomReader->update();

            // Merge series
            ::fwComEd::helper::SeriesDB sDBhelper(m_destinationSeriesDB);
            sDBhelper.merge(m_tempSeriesDB);
            sDBhelper.notify(this->getSptr());
        }

    }




}

//------------------------------------------------------------------------------

void SSeriesPuller::storeInstanceCallback(const ::std::string& seriesInstanceUID, unsigned int instanceNumber,
                                          const ::std::string& filePath)
{
    //Add path in the Dicom series
    if(!m_pullingDicomSeriesMap[seriesInstanceUID].expired())
    {
        ::fwDicomData::DicomSeries::sptr series = m_pullingDicomSeriesMap[seriesInstanceUID].lock();
        series->addDicomPath(instanceNumber, filePath);
    }
    else
    {
        SLM_WARN("The Dicom Series " + seriesInstanceUID + " has expired.");
    }

    //Notify Progress Dialog
    ::std::stringstream ss;
    ss << "Series " << m_seriesIndex << "/" << m_seriesCount << " - Downloading file " << instanceNumber;
    float percentage = float(instanceNumber)/m_instanceCountMap[seriesInstanceUID];
    this->notifyProgressBar(::ioDicomExt::common::data::ProgressMsg::s_UPDATE_PROGRESS_BAR, ss.str(), percentage);

}

//------------------------------------------------------------------------------

void SSeriesPuller::progressCallback(float percentage, bool error)
{
    // If a new series is downloaded
    if(percentage == 0)
    {
        ++m_seriesIndex;
    }

    // If an error occur
    if(error && m_isPulling)
    {
        m_isPulling = false;
        ::std::stringstream ss;
        ss << "Unable to store files in the client. Please check your configuration and the pacs configuration:\n"
           << "Destination application title: " << m_pacsConfiguration->getMoveApplicationTitle() << "\n"
           << "Destination port: " << m_pacsConfiguration->getMoveApplicationPort();
        m_slotDisplayMessage->asyncRun(ss.str());

        //Notify Progress Dialog
        this->notifyProgressBar(::ioDicomExt::common::data::ProgressMsg::s_STOP_PROGRESS_BAR);
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::notifyProgressBar(const ::std::string& actionId, const ::std::string& msg, float percentage) const
{
    //Notify Progress Dialog
    ::ioDicomExt::common::data::ProgressMsg::sptr event = ::ioDicomExt::common::data::ProgressMsg::New();
    event->setBarID(m_progressbarId);
    event->addEvent(actionId);
    event->setMessage(msg);
    event->setPercentage(percentage);

    fwServicesNotifyMsgMacro( this->getLightID(), m_sigProgressBar, event );
}

//------------------------------------------------------------------------------

void SSeriesPuller::displayErrorMessage(const ::std::string& message) const
{
    SLM_WARN("Error: " + message);
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( message );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}


} // namespace controller
} // namespace dcmtk
} // namespace ioDicomExt
