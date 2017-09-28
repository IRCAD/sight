/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioPacs/SSeriesPusher.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Vector.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Series.hpp>

#include <fwPacsIO/exceptions/Base.hpp>
#include <fwPacsIO/helper/Series.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

namespace ioPacs
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioPacs::SSeriesPusher, ::fwData::Object );

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SSeriesPusher::s_DISPLAY_SLOT = "displayMessage";

const ::fwCom::Signals::SignalKeyType SSeriesPusher::s_PROGRESSED_SIG       = "progressed";
const ::fwCom::Signals::SignalKeyType SSeriesPusher::s_STARTED_PROGRESS_SIG = "startedProgress";
const ::fwCom::Signals::SignalKeyType SSeriesPusher::s_STOPPED_PROGRESS_SIG = "stoppedProgress";

//------------------------------------------------------------------------------

SSeriesPusher::SSeriesPusher() noexcept :
    m_progressbarId("pushDicomProgressBar"),
    m_isPushing(false)
{
    // Internal slots
    m_slotDisplayMessage   = newSlot(s_DISPLAY_SLOT, &SSeriesPusher::displayMessage, this);
    m_slotProgressCallback = newSlot(::fwPacsIO::SeriesEnquirer::s_PROGRESS_CALLBACK_SLOT,
                                     &SSeriesPusher::progressCallback, this);

    // Public signals
    m_sigProgressed      = newSignal<ProgressedSignalType>(s_PROGRESSED_SIG);
    m_sigStartedProgress = newSignal<StartedProgressSignalType>(s_STARTED_PROGRESS_SIG);
    m_sigStoppedProgress = newSignal<StoppedProgressSignalType>(s_STOPPED_PROGRESS_SIG);
}
//------------------------------------------------------------------------------

SSeriesPusher::~SSeriesPusher() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesPusher::info(std::ostream& _sstream )
{
    _sstream << "SSeriesPusher::info";
}

//------------------------------------------------------------------------------

void SSeriesPusher::configuring()
{
}

//------------------------------------------------------------------------------

void SSeriesPusher::starting()
{
    SLM_TRACE_FUNC();

    // Create enquirer
    m_seriesEnquirer = ::fwPacsIO::SeriesEnquirer::New();

    // Worker
    m_pushSeriesWorker = ::fwThread::Worker::New();

    // Get pacs configuration
    m_pacsConfiguration = this->getInput< ::fwPacsIO::data::PacsConfiguration>("pacsConfig");
    SLM_ASSERT("The pacs configuration object should not be null.", m_pacsConfiguration);
}

//------------------------------------------------------------------------------

void SSeriesPusher::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesPusher::updating()
{
    ::fwData::Vector::csptr selectedSeries = this->getInput< ::fwData::Vector >("selectedSeries");

    if(m_isPushing)
    {
        // Display a message to inform the user that the service is already pushing data.
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pushing Series");
        messageBox.setMessage( "The service is already pushing data. Please wait until the pushing is done "
                               "before sending a new push request." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    else if(selectedSeries->empty())
    {
        // Display a message to inform the user that there is no series selected.
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pushing Series");
        messageBox.setMessage( "Unable to push series, there is no series selected." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    else
    {
        // Initialize enquirer
        m_seriesEnquirer->initialize(
            m_pacsConfiguration->getLocalApplicationTitle(),
            m_pacsConfiguration->getPacsHostName(),
            m_pacsConfiguration->getPacsApplicationPort(),
            m_pacsConfiguration->getPacsApplicationTitle(),
            m_pacsConfiguration->getMoveApplicationTitle(),
            m_slotProgressCallback);

        // Set pushing boolean to true
        m_isPushing = true;

        // Check whether some selected series are already on the PACS or not
        bool pushOK = this->checkSeriesOnPACS();
        if(pushOK)
        {
            // Push series to the PACS
            m_pushSeriesWorker->post(std::bind(&::ioPacs::SSeriesPusher::pushSeries, this));
        }

    }

}

//------------------------------------------------------------------------------
bool SSeriesPusher::checkSeriesOnPACS()
{
    // Return true if the push operation must be performed
    bool result = true;

    ::fwData::Vector::csptr seriesVector = this->getInput< ::fwData::Vector >("selectedSeries");

    // Catch any errors
    try
    {
        // Find which selected series must be pushed
        DicomSeriesContainerType duplicateSeriesVector;

        // Connect to PACS
        m_seriesEnquirer->connect();

        ::fwData::Vector::ConstIteratorType it = seriesVector->begin();
        for(; it != seriesVector->end(); ++it)
        {
            ::fwMedData::DicomSeries::csptr series = ::fwMedData::DicomSeries::dynamicCast(*it);
            SLM_ASSERT("The SeriesDB should contain only DicomSeries.", series);

            // Try to find series on PACS
            OFList< QRResponse* > responses;
            responses = m_seriesEnquirer->findSeriesByUID(series->getInstanceUID());

            // If the series has been found on the PACS
            if(responses.size() > 1)
            {
                duplicateSeriesVector.push_back(series);
            }

            ::fwPacsIO::helper::Series::releaseResponses(responses);
        }

        // Disconnect from PACS
        m_seriesEnquirer->disconnect();

        // Inform the user that some series are already on the PACS
        if(!duplicateSeriesVector.empty())
        {
            ::std::stringstream ss;
            ss << "Those series are already on the PACS: \n";

            // Display duplicated Series
            for(const ::fwMedData::Series::csptr& series: duplicateSeriesVector)
            {
                std::string description = series->getDescription();
                description = (description.empty()) ? "[No description]" : description;
                ss << "- " << description << std::endl;
            }

            ss << std::endl << "Would you like to perform the operation anyway ?" << std::endl
               << "(This will result in a merge operation)";

            ::fwGui::dialog::MessageDialog messageBox;
            messageBox.setTitle("Duplicate series");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
            messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
            ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();

            result = (answer == ::fwGui::dialog::IMessageDialog::OK);

        }
    }
    catch (::fwPacsIO::exceptions::Base& exception)
    {
        ::std::stringstream ss;
        ss << "Unable to connect to the pacs. Please check your configuration: \n"
           << "Pacs host name: " << m_pacsConfiguration->getPacsHostName() << "\n"
           << "Pacs application title: " << m_pacsConfiguration->getPacsApplicationTitle() << "\n"
           << "Pacs port: " << m_pacsConfiguration->getPacsApplicationPort() << "\n";
        m_slotDisplayMessage->asyncRun(ss.str(), true);
        SLM_WARN(exception.what());
        result = false;

        // Set pushing boolean to false
        m_isPushing = false;

    }

    return result;
}

//------------------------------------------------------------------------------

void SSeriesPusher::pushSeries()
{
    ::fwData::Vector::csptr seriesVector = this->getInput< ::fwData::Vector >("selectedSeries");

    // Catch any errors
    try
    {
        // List of files that must be pushed
        DicomFileContainer dicomFilesContainer;

        // Connect to PACS
        ::fwData::Vector::ConstIteratorType it = seriesVector->begin();
        for(; it != seriesVector->end(); ++it)
        {
            ::fwMedData::DicomSeries::csptr series = ::fwMedData::DicomSeries::dynamicCast(*it);
            SLM_ASSERT("The SeriesDB should contain only DicomSeries.", series);

            BOOST_FOREACH(::fwMedData::DicomSeries::DicomPathContainerType::value_type value,
                          series->getLocalDicomPaths())
            {
                dicomFilesContainer.push_back(value.second);
            }
        }

        // Number of instances that must be uploaded
        m_instanceCount = dicomFilesContainer.size();

        // Connect from PACS
        m_seriesEnquirer->connect();
        m_sigStartedProgress->asyncEmit(m_progressbarId);

        // Push series
        m_seriesEnquirer->pushSeries(dicomFilesContainer);

        // Disconnect from PACS
        m_seriesEnquirer->disconnect();

    }
    catch (::fwPacsIO::exceptions::Base& exception)
    {
        ::std::stringstream ss;
        ss << "Unable to connect to the pacs. Please check your configuration: \n"
           << "Pacs host name: " << m_pacsConfiguration->getPacsHostName() << "\n"
           << "Pacs application title: " << m_pacsConfiguration->getPacsApplicationTitle() << "\n"
           << "Pacs port: " << m_pacsConfiguration->getPacsApplicationPort() << "\n";
        m_slotDisplayMessage->asyncRun(ss.str(), true);
        SLM_WARN(exception.what());
    }

    // Set pushing boolean to false
    m_isPushing = false;

}

//------------------------------------------------------------------------------

void SSeriesPusher::progressCallback(const std::string& seriesInstanceUID, unsigned int instanceNumber,
                                     const std::string& filePath)
{
    if(instanceNumber < (m_instanceCount-1))
    {
        float percentage = static_cast<float>(instanceNumber)/static_cast<float>(m_instanceCount);
        m_sigProgressed->asyncEmit(m_progressbarId, percentage, "Pushing series...");
    }
    else
    {
        m_sigStoppedProgress->asyncEmit(m_progressbarId);
    }
}

//------------------------------------------------------------------------------

void SSeriesPusher::displayMessage(const ::std::string& message, bool error) const
{
    SLM_WARN_IF("Error: " + message, error);
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle((error ? "Error" : "Information"));
    messageBox.setMessage( message );
    messageBox.setIcon(error ? (::fwGui::dialog::IMessageDialog::CRITICAL): (::fwGui::dialog::IMessageDialog::INFO));
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace ioPacs
