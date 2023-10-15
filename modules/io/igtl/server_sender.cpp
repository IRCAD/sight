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
#include <ui/__/Preferences.hpp>

#include <functional>

namespace sight::module::io::igtl
{

//-----------------------------------------------------------------------------

server_sender::server_sender() :
    m_server(std::make_shared<sight::io::igtl::Server>())
{
}

//-----------------------------------------------------------------------------

server_sender::~server_sender()
= default;

//-----------------------------------------------------------------------------

void server_sender::configuring()
{
    service::config_t config = this->get_config();

    m_portConfig = config.get("port", "4242");

    const config_t configIn = config.get_child("in");

    SIGHT_ASSERT(
        "configured group must be 'objects'",
        configIn.get<std::string>("<xmlattr>.group", "") == "objects"
    );

    const auto keyCfg = configIn.equal_range("key");
    for(auto itCfg = keyCfg.first ; itCfg != keyCfg.second ; ++itCfg)
    {
        const service::config_t& attr = itCfg->second.get_child("<xmlattr>");
        const std::string deviceName  = attr.get("deviceName", "Sight");
        m_deviceNames.push_back(deviceName);
    }
}

//-----------------------------------------------------------------------------

void server_sender::starting()
{
    try
    {
        ui::Preferences preferences;
        const auto port = preferences.delimited_get<std::uint16_t>(m_portConfig);

        m_server->start(port);

        m_serverFuture = std::async(std::launch::async, [this](auto&& ...){m_server->runServer();});
        m_sigConnected->async_emit();
    }
    catch(core::exception& e)
    {
        sight::ui::dialog::message::show(
            "Error",
            "Cannot start the server: "
            + std::string(e.what()),
            sight::ui::dialog::message::CRITICAL
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

        m_serverFuture.wait();
        m_sigDisconnected->async_emit();
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

void server_sender::sendObject(const data::object::csptr& obj, const std::size_t index)
{
    if(!m_deviceNames[index].empty())
    {
        m_server->setMessageDeviceName(m_deviceNames[index]);
    }

    m_server->broadcast(obj);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
