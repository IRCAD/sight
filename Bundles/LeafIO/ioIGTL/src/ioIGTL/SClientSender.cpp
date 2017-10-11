/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/SClientSender.hpp"

#include "ioIGTL/helper/preferences.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Failed.hpp>

#include <functional>

fwServicesRegisterMacro(::ioNetwork::INetworkSender, ::ioIGTL::SClientSender);

namespace ioIGTL
{

const ::fwCom::Slots::SlotKeyType SClientSender::s_START_SENDING_SLOT = "startSending";
const ::fwCom::Slots::SlotKeyType SClientSender::s_STOP_SENDING_SLOT  = "stopSending";

const ::fwServices::IService::KeyType s_OBJECTS_GROUP = "objects";

//-----------------------------------------------------------------------------

SClientSender::SClientSender()
{
    newSlot(s_START_SENDING_SLOT, &SClientSender::startSending, this);
    newSlot(s_STOP_SENDING_SLOT, &SClientSender::stopSending, this);
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
        const std::string name                         = attr.get("deviceName", "F4S");
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

void SClientSender::runClient()
{
    // 1. Connection
    try
    {
        const std::uint16_t port   = ::ioIGTL::helper::getPreferenceKey<std::uint16_t>(m_portConfig);
        const std::string hostname = ::ioIGTL::helper::getPreferenceKey<std::string>(m_hostnameConfig);

        m_client.connect(hostname, port);
        m_sigConnected->asyncEmit();
    }
    catch (::fwCore::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Connection error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            SLM_ERROR(ex.what());
        }
        return;
    }
}

//-----------------------------------------------------------------------------

void SClientSender::starting()
{
}

//-----------------------------------------------------------------------------

void SClientSender::stopping()
{
    this->stopSending();
}

//-----------------------------------------------------------------------------

void SClientSender::startSending()
{
    if(!m_client.isConnected())
    {
        m_clientFuture = std::async(std::launch::async, std::bind(&SClientSender::runClient, this));
    }
}

//-----------------------------------------------------------------------------

void SClientSender::stopSending()
{
    if(m_client.isConnected())
    {
        try
        {
            m_client.disconnect();
            m_clientFuture.wait();
            m_sigDisconnected->asyncEmit();
        }
        catch (::fwCore::Exception& e)
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Error", e.what());
        }
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
