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

#include "client_sender.hpp"

#include <core/com/signal.hxx>
#include <core/tools/failed.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>

namespace sight::module::io::igtl
{

//-----------------------------------------------------------------------------

client_sender::client_sender()
= default;

//-----------------------------------------------------------------------------

client_sender::~client_sender()
= default;

//-----------------------------------------------------------------------------

void client_sender::configuring()
{
    service::config_t config = this->get_config();

    const config_t config_in = config.get_child("in");

    SIGHT_ASSERT(
        "configured group must be 'objects'",
        config_in.get<std::string>("<xmlattr>.group", "") == "objects"
    );

    const auto key_cfg = config_in.equal_range("key");
    for(auto it_cfg = key_cfg.first ; it_cfg != key_cfg.second ; ++it_cfg)
    {
        const service::config_t& attr = it_cfg->second.get_child("<xmlattr>");
        const std::string name        = attr.get("deviceName", "Sight");
        m_deviceNames.push_back(name);
    }

    const std::string server_info = config.get("server", "");
    if(!server_info.empty())
    {
        const std::string::size_type split_position = server_info.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", split_position != std::string::npos);

        m_hostnameConfig = server_info.substr(0, split_position);
        m_portConfig     = server_info.substr(split_position + 1, server_info.size());
    }
    else
    {
        throw core::tools::failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void client_sender::starting()
{
    if(!m_client.isConnected())
    {
        try
        {
            ui::preferences preferences;
            const auto port     = preferences.delimited_get<std::uint16_t>(m_portConfig);
            const auto hostname = preferences.delimited_get<std::string>(m_hostnameConfig);

            m_client.connect(hostname, port);
            m_sigConnected->async_emit();
        }
        catch(core::exception& ex)
        {
            sight::ui::dialog::message::show("Connection error", ex.what());
            SIGHT_ERROR(ex.what());
            this->slot(service::slots::STOP)->async_run();
        }
    }
}

//-----------------------------------------------------------------------------

void client_sender::stopping()
{
    try
    {
        if(m_client.isConnected())
        {
            m_client.disconnect();
        }

        m_sigDisconnected->async_emit();
    }
    catch(core::exception& e)
    {
        sight::ui::dialog::message::show("Error", e.what());
        SIGHT_ERROR(e.what());
    }
}

//-----------------------------------------------------------------------------

void client_sender::sendObject(const data::object::csptr& _obj, const std::size_t _index)
{
    SIGHT_ASSERT("No device name associated with object index " << _index, _index < m_deviceNames.size());

    if(m_client.isConnected())
    {
        m_client.setDeviceNameOut(m_deviceNames[_index]);
        m_client.sendObject(_obj);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
