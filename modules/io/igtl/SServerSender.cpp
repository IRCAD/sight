/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SServerSender.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/Preferences.hpp>

#include <functional>

namespace sight::module::io::igtl
{

const service::IService::KeyType s_OBJECTS_GROUP = "objects";

//-----------------------------------------------------------------------------

SServerSender::SServerSender() :
    m_server(std::make_shared<sight::io::igtl::Server>())
{
}

//-----------------------------------------------------------------------------

SServerSender::~SServerSender()
= default;

//-----------------------------------------------------------------------------

void SServerSender::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();

    m_portConfig = config.get("port", "4242");

    const ConfigType configIn = config.get_child("in");

    SIGHT_ASSERT(
        "configured group must be '" + s_OBJECTS_GROUP + "'",
        configIn.get<std::string>("<xmlattr>.group", "") == s_OBJECTS_GROUP
    );

    const auto keyCfg = configIn.equal_range("key");
    for(auto itCfg = keyCfg.first ; itCfg != keyCfg.second ; ++itCfg)
    {
        const service::IService::ConfigType& attr = itCfg->second.get_child("<xmlattr>");
        const std::string deviceName              = attr.get("deviceName", "Sight");
        m_deviceNames.push_back(deviceName);
    }
}

//-----------------------------------------------------------------------------

void SServerSender::starting()
{
    try
    {
        ui::base::Preferences preferences;
        const auto port = preferences.delimited_get<std::uint16_t>(m_portConfig);

        m_server->start(port);

        m_serverFuture = std::async(std::launch::async, [this](auto&& ...){m_server->runServer();});
        m_sigConnected->asyncEmit();
    }
    catch(core::Exception& e)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Error",
            "Cannot start the server: "
            + std::string(e.what()),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
        // Only report the error on console (this normally happens only if we have requested the disconnection)
        SIGHT_ERROR(e.what());
        this->slot(s_STOP_SLOT)->asyncRun();
    }
}

//-----------------------------------------------------------------------------

void SServerSender::stopping()
{
    try
    {
        if(m_server->isStarted())
        {
            m_server->stop();
        }

        m_serverFuture.wait();
        m_sigDisconnected->asyncEmit();
    }
    catch(core::Exception& e)
    {
        sight::ui::base::dialog::MessageDialog::show("Error", e.what());
    }
    catch(std::future_error&)
    {
        // This happens when the server failed to start, so we just ignore it silently.
    }
}

//-----------------------------------------------------------------------------

void SServerSender::sendObject(const data::Object::csptr& obj, const std::size_t index)
{
    if(!m_deviceNames[index].empty())
    {
        m_server->setMessageDeviceName(m_deviceNames[index]);
    }

    m_server->broadcast(obj);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
