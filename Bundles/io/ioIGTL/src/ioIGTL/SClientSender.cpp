/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "ioIGTL/SClientSender.hpp"

#include "ioIGTL/helper/preferences.hpp"

#include <fwCom/Signal.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Failed.hpp>

fwServicesRegisterMacro(::ioNetwork::INetworkSender, ::ioIGTL::SClientSender);

namespace ioIGTL
{

const ::fwServices::IService::KeyType s_OBJECTS_GROUP = "objects";

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
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    const ConfigType configIn = config.get_child("in");

    SLM_ASSERT("configured group must be '" + s_OBJECTS_GROUP + "'",
               configIn.get<std::string>("<xmlattr>.group", "") == s_OBJECTS_GROUP);

    const auto keyCfg = configIn.equal_range("key");
    for(auto itCfg = keyCfg.first; itCfg != keyCfg.second; ++itCfg)
    {
        const ::fwServices::IService::ConfigType& attr = itCfg->second.get_child("<xmlattr>");
        const std::string name                         = attr.get("deviceName", "Sight");
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
        throw ::fwTools::Failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void SClientSender::starting()
{
    if(!m_client.isConnected())
    {
        try
        {
            const std::uint16_t port   = ::ioIGTL::helper::getPreferenceKey<std::uint16_t>(m_portConfig);
            const std::string hostname = ::ioIGTL::helper::getPreferenceKey<std::string>(m_hostnameConfig);

            m_client.connect(hostname, port);
            m_sigConnected->asyncEmit();
        }
        catch (::fwCore::Exception& ex)
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Connection error", ex.what());
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
    catch (::fwCore::Exception& e)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Error", e.what());
        SLM_ERROR(e.what());
    }
}

//-----------------------------------------------------------------------------

void SClientSender::sendObject(const ::fwData::Object::csptr& obj, const size_t index)
{
    OSLM_ASSERT("No device name associated with object index " << index, index < m_deviceNames.size());

    if (m_client.isConnected())
    {
        m_client.setDeviceNameOut(m_deviceNames[index]);
        m_client.sendObject(obj);
    }
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL
