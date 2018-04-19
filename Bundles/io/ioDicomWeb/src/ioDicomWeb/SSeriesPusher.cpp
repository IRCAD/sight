/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicomWeb/SSeriesPusher.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Vector.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Series.hpp>

#include <fwNetworkIO/exceptions/Base.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

namespace ioDicomWeb
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioDicomWeb::SSeriesPusher, ::fwData::Object );

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SSeriesPusher::s_DISPLAY_SLOT = "displayMessage";

//------------------------------------------------------------------------------

SSeriesPusher::SSeriesPusher() noexcept :
    m_isPushing(false)
{
    // Internal slots
    m_slotDisplayMessage = newSlot(s_DISPLAY_SLOT, &SSeriesPusher::displayMessage, this);
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

    // Worker
    m_pushSeriesWorker = ::fwThread::Worker::New();
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
        // Set pushing boolean to true
        m_isPushing = true;

        // Check whether some selected series are already on the PACS or not
        bool pushOK = this->checkSeriesOnPACS();
        if(pushOK)
        {
            // Push series to the PACS
            m_pushSeriesWorker->post(std::bind(&::ioDicomWeb::SSeriesPusher::pushSeries, this));
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
    // try
    // {
    //     // Find which selected series must be pushed
    //     DicomSeriesContainerType duplicateSeriesVector;
    //
    //     // Connect to PACS
    //     ::fwData::Vector::ConstIteratorType it = seriesVector->begin();
    //     for(; it != seriesVector->end(); ++it)
    //     {
    //         ::fwMedData::DicomSeries::csptr series = ::fwMedData::DicomSeries::dynamicCast(*it);
    //         SLM_ASSERT("The SeriesDB should contain only DicomSeries.", series);
    //
    //         // Try to find series on PACS
    //         OFList< QRResponse* > responses;
    //         responses = m_seriesEnquirer->findSeriesByUID(series->getInstanceUID());
    //
    //         // If the series has been found on the PACS
    //         if(responses.size() > 1)
    //         {
    //             duplicateSeriesVector.push_back(series);
    //         }
    //
    //         ::fwDicomWebIO::helper::Series::releaseResponses(responses);
    //     }
    //
    //     // Inform the user that some series are already on the PACS
    //     if(!duplicateSeriesVector.empty())
    //     {
    //         ::std::stringstream ss;
    //         ss << "Those series are already on the PACS: \n";
    //
    //         // Display duplicated Series
    //         for(const ::fwMedData::Series::csptr& series: duplicateSeriesVector)
    //         {
    //             std::string description = series->getDescription();
    //             description = (description.empty()) ? "[No description]" : description;
    //             ss << "- " << description << std::endl;
    //         }
    //
    //         ss << std::endl << "Would you like to perform the operation anyway ?" << std::endl
    //            << "(This will result in a merge operation)";
    //
    //         ::fwGui::dialog::MessageDialog messageBox;
    //         messageBox.setTitle("Duplicate series");
    //         messageBox.setMessage( ss.str() );
    //         messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
    //         messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    //         messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
    //         ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();
    //
    //         result = (answer == ::fwGui::dialog::IMessageDialog::OK);
    //
    //     }
    // }
    // catch (::fwDicomWebIO::exceptions::Base& exception)
    // {
    //     result = false;
    //
    //     // Set pushing boolean to false
    //     m_isPushing = false;
    //
    // }

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

        // Push series
        // m_seriesRequestor->pushSeries(dicomFilesContainer);

    }
    catch (::fwNetworkIO::exceptions::Base& exception)
    {
        std::stringstream ss;
        m_slotDisplayMessage->asyncRun(ss.str(), true);
        SLM_WARN(exception.what());
    }

    // Set pushing boolean to false
    m_isPushing = false;

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

} // namespace ioDicomWeb
