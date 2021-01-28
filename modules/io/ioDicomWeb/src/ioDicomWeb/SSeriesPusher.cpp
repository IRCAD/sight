/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "ioDicomWeb/SSeriesPusher.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/DicomSeries.hpp>
#include <data/Series.hpp>
#include <data/Vector.hpp>

#include <fwNetworkIO/exceptions/Base.hpp>
#include <fwNetworkIO/helper/Series.hpp>
#include <fwNetworkIO/http/Request.hpp>

#include <services/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/preferences/helper.hpp>

namespace ioDicomWeb
{

//------------------------------------------------------------------------------

static const services::IService::KeyType s_SERIES_IN = "selectedSeries";

//------------------------------------------------------------------------------

SSeriesPusher::SSeriesPusher() noexcept :
    m_isPushing(false)
{
}
//------------------------------------------------------------------------------

SSeriesPusher::~SSeriesPusher() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesPusher::configuring()
{
    services::IService::ConfigType configuration = this->getConfigTree();
    //Parse server port and hostname
    if(configuration.count("server"))
    {
        const std::string serverInfo               = configuration.get("server", "");
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_serverHostnameKey = serverInfo.substr(0, splitPosition);
        m_serverPortKey     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw core::tools::Failed("'server' element not found");
    }
}

//------------------------------------------------------------------------------

void SSeriesPusher::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesPusher::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesPusher::updating()
{
    const std::string hostname = modules::ui::base::preferences::getValue(m_serverHostnameKey);
    if(!hostname.empty())
    {
        m_serverHostname = hostname;
    }
    const std::string port = modules::ui::base::preferences::getValue(m_serverPortKey);
    if(!port.empty())
    {
        m_serverPort = std::stoi(port);
    }

    data::Vector::csptr selectedSeries = this->getInput< data::Vector >(s_SERIES_IN);

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
        // Push series to the PACS
        this->pushSeries();
    }
}

//------------------------------------------------------------------------------

void SSeriesPusher::pushSeries()
{
    m_isPushing = true;

    data::Vector::csptr seriesVector = this->getInput< data::Vector >(s_SERIES_IN);

    const std::vector< data::DicomSeries::sptr > dataVector =
        seriesVector->getDataContainer< data::DicomSeries >();
    // Connect to PACS
    const size_t seriesVectorSize = seriesVector->size();
    size_t nbSeriesSuccess        = 0;
    for(const auto& dicomSeries : dataVector)
    {
        nbSeriesSuccess++;

        data::DicomSeries::DicomContainerType dicomContainer = dicomSeries->getDicomContainer();
        const size_t dicomContainerSize                      = dicomContainer.size();

        size_t nbInstanceSuccess = 0;
        try
        {
            for(const auto& item : dicomContainer)
            {
                const core::memory::BufferObject::sptr bufferObj = item.second;
                const core::memory::BufferObject::Lock lockerDest(bufferObj);
                const char* buffer = static_cast<char*>(lockerDest.getBuffer());
                const size_t size  = bufferObj->getSize();

                const QByteArray fileBuffer = QByteArray::fromRawData(buffer, size);

                /// Url PACS
                const std::string pacsServer("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));
                ::fwNetworkIO::http::Request::sptr request =
                    ::fwNetworkIO::http::Request::New(pacsServer + "/instances");
                QByteArray seriesAnswer;
                if (fileBuffer.size() != 0)
                {
                    seriesAnswer = m_clientQt.post(request, fileBuffer);
                    if (!seriesAnswer.isEmpty())
                    {
                        nbInstanceSuccess++;
                    }
                }
                if (dicomContainerSize == nbInstanceSuccess)
                {
                    this->displayMessage("Upload successful: " + std::to_string(nbSeriesSuccess) + "/" +
                                         std::to_string(seriesVectorSize), false);
                }
            }
        }
        catch (::fwNetworkIO::exceptions::HostNotFound& exception)
        {
            std::stringstream ss;
            ss << "Host not found.\n"
               << "Please check your configuration: \n"
               << "Pacs host name: " << m_serverHostname << "\n"
               << "Pacs port: " << m_serverPort << "\n";
            this->displayMessage(ss.str(), true);
            SLM_WARN(exception.what());
        }
    }

    // Set pushing boolean to false
    m_isPushing = false;
}

//------------------------------------------------------------------------------

void SSeriesPusher::displayMessage(const std::string& message, bool error) const
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

} // namespace ioDicomWeb
