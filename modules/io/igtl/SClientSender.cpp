/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SClientSender.hpp"

#include <core/com/Signal.hxx>
#include <core/tools/Failed.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/preferences/helper.hpp>

fwServicesRegisterMacro(::sight::module::io::igtl::INetworkSender, ::sight::module::io::igtl::SClientSender)

namespace sight::module::io::igtl
{

const service::IService::KeyType s_OBJECTS_GROUP = "objects";

//-----------------------------------------------------------------------------

SClientSender::SClientSender()
{
}

//-----------------------------------------------------------------------------

SClientSender::~SClientSender()
{
}

//-----------------------------------------------------------------------------

void SClientSender::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();

    const ConfigType configIn = config.get_child("in");

    SLM_ASSERT("configured group must be '" + s_OBJECTS_GROUP + "'",
               configIn.get<std::string>("<xmlattr>.group", "") == s_OBJECTS_GROUP);

    const auto keyCfg = configIn.equal_range("key");
    for(auto itCfg = keyCfg.first; itCfg != keyCfg.second; ++itCfg)
    {
        const service::IService::ConfigType& attr = itCfg->second.get_child("<xmlattr>");
        const std::string name                    = attr.get("deviceName", "Sight");
        m_deviceNames.push_back(name);
    }

    const std::string serverInfo = config.get("server", "");
    if(!serverInfo.empty())
    {
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_hostnameConfig = serverInfo.substr(0, splitPosition);
        m_portConfig     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw core::tools::Failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void SClientSender::starting()
{
    if(!m_client.isConnected())
    {
        try
        {
            const std::uint16_t port   = ui::base::preferences::getValue<std::uint16_t>(m_portConfig);
            const std::string hostname = ui::base::preferences::getValue(m_hostnameConfig);

            m_client.connect(hostname, port);
            m_sigConnected->asyncEmit();
        }
        catch (core::Exception& ex)
        {
            sight::ui::base::dialog::MessageDialog::show("Connection error", ex.what());
            SLM_ERROR(ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
    }
}

//-----------------------------------------------------------------------------

void SClientSender::stopping()
{
    try
    {
        if(m_client.isConnected())
        {
            m_client.disconnect();
        }
        m_sigDisconnected->asyncEmit();
    }
    catch (core::Exception& e)
    {
        sight::ui::base::dialog::MessageDialog::show("Error", e.what());
        SLM_ERROR(e.what());
    }
}

//-----------------------------------------------------------------------------

void SClientSender::sendObject(const data::Object::csptr& obj, const size_t index)
{
    SLM_ASSERT("No device name associated with object index " << index, index < m_deviceNames.size());

    if (m_client.isConnected())
    {
        m_client.setDeviceNameOut(m_deviceNames[index]);
        m_client.sendObject(obj);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
