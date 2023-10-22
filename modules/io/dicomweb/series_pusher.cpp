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

#include "series_pusher.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/tools/failed.hpp>

#include <data/dicom_series.hpp>
#include <data/series.hpp>
#include <data/vector.hpp>

#include <io/http/exceptions/Base.hpp>
#include <io/http/helper/Series.hpp>
#include <io/http/Request.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>

namespace sight::module::io::dicomweb
{

//------------------------------------------------------------------------------

series_pusher::series_pusher() noexcept =
    default;

//------------------------------------------------------------------------------

series_pusher::~series_pusher() noexcept =
    default;

//------------------------------------------------------------------------------

void series_pusher::configuring()
{
    service::config_t configuration = this->get_config();
    //Parse server port and hostname
    if(configuration.count("server") != 0U)
    {
        const std::string server_info               = configuration.get("server", "");
        const std::string::size_type split_position = server_info.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", split_position != std::string::npos);

        m_serverHostnameKey = server_info.substr(0, split_position);
        m_serverPortKey     = server_info.substr(split_position + 1, server_info.size());
    }
    else
    {
        throw core::tools::failed("'server' element not found");
    }
}

//------------------------------------------------------------------------------

void series_pusher::starting()
{
}

//------------------------------------------------------------------------------

void series_pusher::stopping()
{
}

//------------------------------------------------------------------------------

void series_pusher::updating()
{
    ui::preferences preferences;

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

    const auto selected_series = m_selectedSeries.lock();

    if(m_isPushing)
    {
        // Display a message to inform the user that the service is already pushing data.
        sight::ui::dialog::message message_box;
        message_box.setTitle("Pushing Series");
        message_box.setMessage(
            "The service is already pushing data. Please wait until the pushing is done "
            "before sending a new push request."
        );
        message_box.setIcon(ui::dialog::message::INFO);
        message_box.addButton(ui::dialog::message::OK);
        message_box.show();
    }
    else if(selected_series->empty())
    {
        // Display a message to inform the user that there is no series selected.
        sight::ui::dialog::message message_box;
        message_box.setTitle("Pushing Series");
        message_box.setMessage("Unable to push series, there is no series selected.");
        message_box.setIcon(ui::dialog::message::INFO);
        message_box.addButton(ui::dialog::message::OK);
        message_box.show();
    }
    else
    {
        // Push series to the PACS
        this->pushSeries();
    }
}

//------------------------------------------------------------------------------

void series_pusher::pushSeries()
{
    m_isPushing = true;

    const auto series_vector = m_selectedSeries.lock();

    // Connect to PACS
    std::size_t nb_series_success = 0;
    for(const auto& series : *series_vector)
    {
        const auto& dicom_series = std::dynamic_pointer_cast<data::dicom_series>(series);

        if(!dicom_series)
        {
            continue;
        }

        nb_series_success++;

        data::dicom_series::dicom_container_t dicom_container = dicom_series->getDicomContainer();
        const std::size_t dicom_container_size                = dicom_container.size();

        try
        {
            std::size_t nb_instance_success = 0;
            for(const auto& item : dicom_container)
            {
                const core::memory::buffer_object::sptr buffer_obj = item.second;
                const core::memory::buffer_object::lock_t locker_dest(buffer_obj);
                const char* buffer     = static_cast<char*>(locker_dest.buffer());
                const std::size_t size = buffer_obj->size();

                const QByteArray file_buffer = QByteArray::fromRawData(buffer, int(size));

                /// Url PACS
                const std::string pacs_server("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));
                sight::io::http::Request::sptr request =
                    sight::io::http::Request::New(pacs_server + "/instances");
                QByteArray series_answer;
                if(file_buffer.size() != 0)
                {
                    series_answer = m_clientQt.post(request, file_buffer);
                    if(!series_answer.isEmpty())
                    {
                        nb_instance_success++;
                    }
                }

                if(dicom_container_size == nb_instance_success)
                {
                    sight::module::io::dicomweb::series_pusher::displayMessage(
                        "Upload successful: " + std::to_string(nb_series_success) + "/"
                        + std::to_string(series_vector->size()),
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
            sight::module::io::dicomweb::series_pusher::displayMessage(ss.str(), true);
            SIGHT_WARN(exception.what());
        }
    }

    // Set pushing boolean to false
    m_isPushing = false;
}

//------------------------------------------------------------------------------

void series_pusher::displayMessage(const std::string& _message, bool _error)
{
    SIGHT_WARN_IF("Error: " + _message, _error);
    sight::ui::dialog::message message_box;
    message_box.setTitle((_error ? "Error" : "Information"));
    message_box.setMessage(_message);
    message_box.setIcon(_error ? (ui::dialog::message::CRITICAL) : (ui::dialog::message::INFO));
    message_box.addButton(ui::dialog::message::OK);
    message_box.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicomweb
