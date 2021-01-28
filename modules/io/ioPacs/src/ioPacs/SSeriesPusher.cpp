/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ioPacs/SSeriesPusher.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/DicomSeries.hpp>
#include <data/Series.hpp>
#include <data/Vector.hpp>

#include <fwPacsIO/exceptions/Base.hpp>
#include <fwPacsIO/helper/Series.hpp>

#include <services/macros.hpp>

#include <dcmtk/dcmdata/dcistrmb.h>

#include <ui/base/dialog/MessageDialog.hpp>

namespace ioPacs
{

fwServicesRegisterMacro( ::sight::services::IController, ::ioPacs::SSeriesPusher, ::sight::data::Object )

//------------------------------------------------------------------------------

const core::com::Slots::SlotKeyType SSeriesPusher::s_DISPLAY_SLOT = "displayMessage";

const core::com::Signals::SignalKeyType SSeriesPusher::s_PROGRESSED_SIG       = "progressed";
const core::com::Signals::SignalKeyType SSeriesPusher::s_STARTED_PROGRESS_SIG = "startedProgress";
const core::com::Signals::SignalKeyType SSeriesPusher::s_STOPPED_PROGRESS_SIG = "stoppedProgress";

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
    // Create enquirer
    m_seriesEnquirer = ::fwPacsIO::SeriesEnquirer::New();

    // Worker
    m_pushSeriesWorker = core::thread::Worker::New();

    // Get pacs configuration
    m_pacsConfiguration = this->getInput< ::fwPacsIOdata::PacsConfiguration>("pacsConfig");
    SLM_ASSERT("The pacs configuration object should not be null.", m_pacsConfiguration);
}

//------------------------------------------------------------------------------

void SSeriesPusher::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesPusher::updating()
{
    data::Vector::csptr selectedSeries = this->getInput< data::Vector >("selectedSeries");

    if(m_isPushing)
    {
        // Display a message to inform the user that the service is already pushing data.
        ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pushing Series");
        messageBox.setMessage( "The service is already pushing data. Please wait until the pushing is done "
                               "before sending a new push request." );
        messageBox.setIcon(ui::base::dialog::IMessageDialog::INFO);
        messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    else if(selectedSeries->empty())
    {
        // Display a message to inform the user that there is no series selected.
        ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pushing Series");
        messageBox.setMessage( "Unable to push series, there is no series selected." );
        messageBox.setIcon(ui::base::dialog::IMessageDialog::INFO);
        messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
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

    data::Vector::csptr seriesVector = this->getInput< data::Vector >("selectedSeries");

    // Catch any errors
    try
    {
        // Find which selected series must be pushed
        DicomSeriesContainerType duplicateSeriesVector;

        // Connect to PACS
        m_seriesEnquirer->connect();

        data::Vector::ConstIteratorType it = seriesVector->begin();
        for(; it != seriesVector->end(); ++it)
        {
            data::DicomSeries::csptr series = data::DicomSeries::dynamicCast(*it);
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
            for(const data::Series::csptr& series: duplicateSeriesVector)
            {
                std::string description = series->getDescription();
                description = (description.empty()) ? "[No description]" : description;
                ss << "- " << description << std::endl;
            }

            ss << std::endl << "Would you like to perform the operation anyway ?" << std::endl
               << "(This will result in a merge operation)";

            ui::base::dialog::MessageDialog messageBox;
            messageBox.setTitle("Duplicate series");
            messageBox.setMessage( ss.str() );
            messageBox.setIcon(ui::base::dialog::IMessageDialog::INFO);
            messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
            messageBox.addButton(ui::base::dialog::IMessageDialog::CANCEL);
            ui::base::dialog::IMessageDialog::Buttons answer = messageBox.show();

            result = (answer == ui::base::dialog::IMessageDialog::OK);

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
    data::Vector::csptr seriesVector = this->getInput< data::Vector >("selectedSeries");

    // Catch any errors
    try
    {
        // List of dicom slice that must be pushed
        std::vector< CSPTR(DcmDataset) > dicomContainer;

        // Connect to PACS
        for(const auto& series : *seriesVector)
        {
            data::DicomSeries::csptr dicomSeries = data::DicomSeries::dynamicCast(series);
            SLM_ASSERT("The SeriesDB should contain only DicomSeries.", dicomSeries);

            for(const auto& item : dicomSeries->getDicomContainer())
            {
                DcmFileFormat fileFormat;
                core::memory::BufferObject::sptr bufferObj = item.second;
                const size_t buffSize                      = bufferObj->getSize();
                core::memory::BufferObject::Lock lock(bufferObj);
                char* buffer = static_cast< char* >( lock.getBuffer() );

                DcmInputBufferStream is;
                is.setBuffer(buffer, offile_off_t(buffSize));
                is.setEos();

                fileFormat.transferInit();
                if (!fileFormat.read(is).good())
                {
                    FW_RAISE("Unable to read Dicom file '"<< bufferObj->getStreamInfo().fsFile.string() <<"'");
                }

                fileFormat.loadAllDataIntoMemory();
                fileFormat.transferEnd();

                const DcmDataset* dataset = fileFormat.getAndRemoveDataset();
                CSPTR(DcmDataset) datasetPtr(dataset);
                dicomContainer.push_back(datasetPtr);
            }
        }

        // Number of instances that must be uploaded
        m_instanceCount = dicomContainer.size();

        // Connect from PACS
        m_seriesEnquirer->connect();
        m_sigStartedProgress->asyncEmit(m_progressbarId);

        // Push series
        m_seriesEnquirer->pushSeries(dicomContainer);

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
    ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle((error ? "Error" : "Information"));
    messageBox.setMessage( message );
    messageBox.setIcon(error ? (ui::base::dialog::IMessageDialog::CRITICAL): (ui::base::dialog::IMessageDialog::INFO));
    messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace ioPacs
