/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ioIGTL/SServerListener.hpp"

#include <fwCom/Signal.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>

#include <functional>

fwServicesRegisterMacro(::ioNetwork::INetworkListener, ::ioIGTL::SServerListener);

namespace ioIGTL
{

const ::fwServices::IService::KeyType s_OBJECTS_GROUP = "objects";

//-----------------------------------------------------------------------------

SServerListener::SServerListener()
{
    m_server = std::make_shared< ::igtlNetwork::Server>();
}

//-----------------------------------------------------------------------------

SServerListener::~SServerListener()
{
}

//-----------------------------------------------------------------------------

void SServerListener::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    m_portConfig = config.get("port", "4242");

    const ConfigType configInOut = config.get_child("inout");

    SLM_ASSERT("configured group must be '" + s_OBJECTS_GROUP + "'",
               configInOut.get<std::string>("<xmlattr>.group", "") == s_OBJECTS_GROUP);

    const auto keyCfg = configInOut.equal_range("key");
    for(auto itCfg = keyCfg.first; itCfg != keyCfg.second; ++itCfg)
    {
        const ::fwServices::IService::ConfigType& attr = itCfg->second.get_child("<xmlattr>");
        const std::string deviceName                   = attr.get("deviceName", "Sight");
        m_deviceNames.push_back(deviceName);
        m_server->addAuthorizedDevice(deviceName);
    }
    m_server->setFilteringByDeviceName(true);
}

//-----------------------------------------------------------------------------

void SServerListener::starting()
{
    try
    {
        const std::uint16_t port = ::fwPreferences::getValue<std::uint16_t>(m_portConfig);
        m_server->start(port);

        m_serverFuture = std::async(std::launch::async, std::bind(&::igtlNetwork::Server::runServer, m_server) );
        m_sigConnected->asyncEmit();
        m_receiveFuture = std::async(std::launch::async, std::bind(&SServerListener::receiveObject, this) );
    }
    catch (::fwCore::Exception& e)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Error", "Cannot start the server: " +
                                                          std::string(e.what()),
                                                          ::fwGui::dialog::IMessageDialog::CRITICAL);
        // Only report the error on console (this normally happens only if we have requested the disconnection)
        SLM_ERROR(e.what());
        this->slot(s_STOP_SLOT)->asyncRun();
    }
}

//-----------------------------------------------------------------------------

void SServerListener::stopping()
{
    try
    {
        if(m_server->isStarted())
        {
            m_server->stop();
        }
        m_serverFuture.wait();
        m_receiveFuture.wait();
        m_sigDisconnected->asyncEmit();
    }
    catch (::fwCore::Exception& e)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Error", e.what());
    }
    catch (std::future_error&)
    {
        // This happens when the server failed to start, so we just ignore it silently.
    }
}

//-----------------------------------------------------------------------------

void SServerListener::receiveObject()
{
    try
    {
        while (m_server->isStarted())
        {
            std::vector<std::string> deviceNamesReceive;
            std::vector< ::fwData::Object::sptr > receiveObjects = m_server->receiveObjects(deviceNamesReceive);

            size_t client = 0;
            for(const auto& receiveObject : receiveObjects)
            {
                if (receiveObject)
                {
                    const std::string deviceName = deviceNamesReceive[client];

                    const auto& iter = std::find(m_deviceNames.begin(), m_deviceNames.end(), deviceName);
                    if(iter != m_deviceNames.end())
                    {
                        const auto indexReceiveObject = std::distance(m_deviceNames.begin(), iter);
                        ::fwData::Object::sptr obj =
                            this->getInOut< ::fwData::Object >(s_OBJECTS_GROUP, indexReceiveObject);

                        obj->shallowCopy(receiveObject);

                        ::fwData::Object::ModifiedSignalType::sptr sig;
                        sig = obj->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
                        sig->asyncEmit();
                    }
                }
                ++client;
            }
        }
    }
    catch (::fwCore::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // ReceiveObject may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            OSLM_ERROR(ex.what());
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL
