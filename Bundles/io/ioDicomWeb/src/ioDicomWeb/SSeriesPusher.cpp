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
#include <fwNetworkIO/helper/Series.hpp>
#include <fwNetworkIO/http/Request.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>

namespace ioDicomWeb
{

//------------------------------------------------------------------------------

static const ::fwServices::IService::KeyType s_SERIES_IN = "selectedSeries";

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

void SSeriesPusher::configuring()
{
    ::fwServices::IService::ConfigType configuration = this->getConfigTree();
    //Parse server port and hostname
    if(configuration.count("server"))
    {
        const std::string serverInfo               = configuration.get("server", "");
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        const std::string hostnameStr = serverInfo.substr(0, splitPosition);
        const std::string portStr     = serverInfo.substr(splitPosition + 1, serverInfo.size());

        m_serverHostnameKey = this->getPreferenceKey(hostnameStr);
        m_serverPortKey     = this->getPreferenceKey(portStr);

        if(m_serverHostnameKey.empty())
        {
            m_serverHostname = hostnameStr;
        }
        if(m_serverPortKey.empty())
        {
            m_serverPort = std::stoi(portStr);
        }
    }
    else
    {
        throw ::fwTools::Failed("'server' element not found");
    }
}

// -----------------------------------------------------------------------------

std::string SSeriesPusher::getPreferenceKey(const std::string& key) const
{
    std::string keyResult;
    const size_t first = key.find('%');
    const size_t last  = key.rfind('%');
    if (first == 0 && last == key.size() - 1)
    {
        keyResult = key.substr(1, key.size() - 2);
    }
    return keyResult;
}

//------------------------------------------------------------------------------

void SSeriesPusher::starting()
{
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
    if(!m_serverHostnameKey.empty())
    {
        const std::string hostname = ::fwPreferences::getPreference(m_serverHostnameKey);
        if(!hostname.empty())
        {
            m_serverHostname = hostname;
        }
    }
    if(!m_serverPortKey.empty())
    {
        const std::string port = ::fwPreferences::getPreference(m_serverPortKey);
        if(!port.empty())
        {
            m_serverPort = std::stoi(port);
        }
    }

    ::fwData::Vector::csptr selectedSeries = this->getInput< ::fwData::Vector >(s_SERIES_IN);

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
        // Push series to the PACS
        m_pushSeriesWorker->post(std::bind(&::ioDicomWeb::SSeriesPusher::pushSeries, this));
    }
}

//------------------------------------------------------------------------------

void SSeriesPusher::pushSeries()
{
    ::fwData::Vector::csptr seriesVector = this->getInput< ::fwData::Vector >(s_SERIES_IN);

    // Catch any errors
    try
    {
        // Connect to PACS
        for(const auto& series : *seriesVector)
        {
            ::fwMedData::DicomSeries::csptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(series);
            SLM_ASSERT("The SeriesDB should contain only DicomSeries.", dicomSeries);

            for(const auto& item : dicomSeries->getDicomContainer())
            {

                const ::fwMemory::BufferObject::sptr bufferObj = item.second;
                const ::fwMemory::BufferObject::Lock lockerDest(bufferObj);
                const char* buffer = static_cast<char*>(lockerDest.getBuffer());
                const size_t size  = bufferObj->getSize();

                const QByteArray fileBuffer = QByteArray::fromRawData(buffer, size);

                /// Url PACS
                const std::string pacsServer("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));
                ::fwNetworkIO::http::Request::sptr request =
                    ::fwNetworkIO::http::Request::New(pacsServer + "/instances");
                QByteArray seriesAnswer;
                try
                {
                    if (fileBuffer.size() != 0)
                    {
                        seriesAnswer = m_clientQt.post(request, fileBuffer);
                    }
                }
                catch  (::fwNetworkIO::exceptions::HostNotFound& exception)
                {
                    std::stringstream ss;
                    ss << "Host not found:\n"
                       << " Please check your configuration: \n"
                       << "Pacs host name: " << m_serverHostname << "\n"
                       << "Pacs port: " << m_serverPort << "\n";

                    this->displayMessage(ss.str(), true);
                    SLM_WARN(exception.what());
                }

            }
        }
    }
    catch (::fwNetworkIO::exceptions::Base& exception)
    {
        ::std::stringstream ss;
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
