/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "server_sender.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>

#include <functional>

namespace sight::module::io::igtl
{

//-----------------------------------------------------------------------------

server_sender::server_sender() :
    m_server(std::make_shared<sight::io::igtl::server>())
{
}

//-----------------------------------------------------------------------------

server_sender::~server_sender()
= default;

//-----------------------------------------------------------------------------

void server_sender::configuring()
{
    service::config_t config = this->get_config();

    m_port_config = config.get("port", "4242");

    const config_t config_in = config.get_child("in");

    SIGHT_ASSERT(
        "configured group must be 'objects'",
        config_in.get<std::string>("<xmlattr>.group", "") == "objects"
    );

    const auto key_cfg = config_in.equal_range("key");
    for(auto it_cfg = key_cfg.first ; it_cfg != key_cfg.second ; ++it_cfg)
    {
        const service::config_t& attr = it_cfg->second.get_child("<xmlattr>");
        const std::string device_name = attr.get("deviceName", "Sight");
        m_device_names.push_back(device_name);
    }
}

//-----------------------------------------------------------------------------

void server_sender::starting()
{
    try
    {
        ui::preferences preferences;
        const auto port = preferences.delimited_get<std::uint16_t>(m_port_config);

        m_server->start(port);

        m_server_future = std::async(std::launch::async, [this](auto&& ...){m_server->run_server();});
        m_sig_connected->async_emit();
    }
    catch(core::exception& e)
    {
        sight::ui::dialog::message::show(
            "Error",
            "Cannot start the server: "
            + std::string(e.what()),
            sight::ui::dialog::message::critical
        );
        // Only report the error on console (this normally happens only if we have requested the disconnection)
        SIGHT_ERROR(e.what());
        this->slot(service::slots::STOP)->async_run();
    }
}

//-----------------------------------------------------------------------------

void server_sender::stopping()
{
    try
    {
        if(m_server->started())
        {
            m_server->stop();
        }

        m_server_future.wait();
        m_sig_disconnected->async_emit();
    }
    catch(core::exception& e)
    {
        sight::ui::dialog::message::show("Error", e.what());
    }
    catch(std::future_error&)
    {
        // This happens when the server failed to start, so we just ignore it silently.
    }
}

//-----------------------------------------------------------------------------

void server_sender::send_object(const data::object::csptr& _obj, const std::size_t _index)
{
    if(!m_device_names[_index].empty())
    {
        m_server->set_message_device_name(m_device_names[_index]);
    }

    m_server->broadcast(_obj);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
