/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "SSeriesPusher.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/tools/failed.hpp>

#include <data/DicomSeries.hpp>
#include <data/Series.hpp>
#include <data/Vector.hpp>

#include <io/http/exceptions/Base.hpp>
#include <io/http/helper/Series.hpp>
#include <io/http/Request.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/Preferences.hpp>

namespace sight::module::io::dicomweb
{

//------------------------------------------------------------------------------

SSeriesPusher::SSeriesPusher() noexcept =
    default;

//------------------------------------------------------------------------------

SSeriesPusher::~SSeriesPusher() noexcept =
    default;

//------------------------------------------------------------------------------

void SSeriesPusher::configuring()
{
    service::config_t configuration = this->getConfiguration();
    //Parse server port and hostname
    if(configuration.count("server") != 0U)
    {
        const std::string serverInfo               = configuration.get("server", "");
        const std::string::size_type splitPosition = serverInfo.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_serverHostnameKey = serverInfo.substr(0, splitPosition);
        m_serverPortKey     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw core::tools::failed("'server' element not found");
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
    ui::Preferences preferences;

    try
    {
        m_serverPort = preferences.delimited_get(m_serverPortKey, m_serverPort);
    }
    catch(...)
    {
        // Do nothing
    }

    try
    {
        m_serverHostname = preferences.delimited_get(m_serverHostnameKey, m_serverHostname);
    }
    catch(...)
    {
        // Do nothing
    }

    const auto selectedSeries = m_selectedSeries.lock();

    if(m_isPushing)
    {
        // Display a message to inform the user that the service is already pushing data.
        sight::ui::dialog::message messageBox;
        messageBox.setTitle("Pushing Series");
        messageBox.setMessage(
            "The service is already pushing data. Please wait until the pushing is done "
            "before sending a new push request."
        );
        messageBox.setIcon(ui::dialog::message::INFO);
        messageBox.addButton(ui::dialog::message::OK);
        messageBox.show();
    }
    else if(selectedSeries->empty())
    {
        // Display a message to inform the user that there is no series selected.
        sight::ui::dialog::message messageBox;
        messageBox.setTitle("Pushing Series");
        messageBox.setMessage("Unable to push series, there is no series selected.");
        messageBox.setIcon(ui::dialog::message::INFO);
        messageBox.addButton(ui::dialog::message::OK);
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

    const auto seriesVector = m_selectedSeries.lock();

    // Connect to PACS
    std::size_t nbSeriesSuccess = 0;
    for(const auto& series : *seriesVector)
    {
        const auto& dicomSeries = std::dynamic_pointer_cast<data::DicomSeries>(series);

        if(!dicomSeries)
        {
            continue;
        }

        nbSeriesSuccess++;

        data::DicomSeries::DicomContainerType dicomContainer = dicomSeries->getDicomContainer();
        const std::size_t dicomContainerSize                 = dicomContainer.size();

        try
        {
            std::size_t nbInstanceSuccess = 0;
            for(const auto& item : dicomContainer)
            {
                const core::memory::buffer_object::sptr bufferObj = item.second;
                const core::memory::buffer_object::lock_t lockerDest(bufferObj);
                const char* buffer     = static_cast<char*>(lockerDest.buffer());
                const std::size_t size = bufferObj->size();

                const QByteArray fileBuffer = QByteArray::fromRawData(buffer, int(size));

                /// Url PACS
                const std::string pacsServer("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));
                sight::io::http::Request::sptr request =
                    sight::io::http::Request::New(pacsServer + "/instances");
                QByteArray seriesAnswer;
                if(fileBuffer.size() != 0)
                {
                    seriesAnswer = m_clientQt.post(request, fileBuffer);
                    if(!seriesAnswer.isEmpty())
                    {
                        nbInstanceSuccess++;
                    }
                }

                if(dicomContainerSize == nbInstanceSuccess)
                {
                    sight::module::io::dicomweb::SSeriesPusher::displayMessage(
                        "Upload successful: " + std::to_string(nbSeriesSuccess) + "/"
                        + std::to_string(seriesVector->size()),
                        false
                    );
                }
            }
        }
        catch(sight::io::http::exceptions::HostNotFound& exception)
        {
            std::stringstream ss;
            ss << "Host not found.\n"
            << "Please check your configuration: \n"
            << "Pacs host name: " << m_serverHostname << "\n"
            << "Pacs port: " << m_serverPort << "\n";
            sight::module::io::dicomweb::SSeriesPusher::displayMessage(ss.str(), true);
            SIGHT_WARN(exception.what());
        }
    }

    // Set pushing boolean to false
    m_isPushing = false;
}

//------------------------------------------------------------------------------

void SSeriesPusher::displayMessage(const std::string& message, bool error)
{
    SIGHT_WARN_IF("Error: " + message, error);
    sight::ui::dialog::message messageBox;
    messageBox.setTitle((error ? "Error" : "Information"));
    messageBox.setMessage(message);
    messageBox.setIcon(error ? (ui::dialog::message::CRITICAL) : (ui::dialog::message::INFO));
    messageBox.addButton(ui::dialog::message::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicomweb
