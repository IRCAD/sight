/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/SClientSender.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwTools/Failed.hpp>

#include <boost/lexical_cast.hpp>

fwServicesRegisterMacro(::ioNetwork::INetworkSender, ::ioIGTL::SClientSender, ::fwData::Object);

namespace ioIGTL
{

const ::fwCom::Slots::SlotKeyType SClientSender::s_START_SENDING_SLOT = "startSending";
const ::fwCom::Slots::SlotKeyType SClientSender::s_STOP_SENDING_SLOT  = "stopSending";

//-----------------------------------------------------------------------------

SClientSender::SClientSender() throw() :
    m_hostname("127.0.0.1"),
    m_deviceName("F4S"),
    m_port(4242)
{
    m_startSendingSlot = ::fwCom::newSlot(&SClientSender::startSending, this);
    m_stopSendingSlot  = ::fwCom::newSlot(&SClientSender::stopSending, this);
    ::fwCom::HasSlots::m_slots
        (SClientSender::s_START_SENDING_SLOT, m_startSendingSlot )
        (SClientSender::s_STOP_SENDING_SLOT, m_stopSendingSlot );

}

//-----------------------------------------------------------------------------

SClientSender::~SClientSender() throw()
{
}

//-----------------------------------------------------------------------------

void SClientSender::configuring() throw (::fwTools::Failed)
{
    SLM_ASSERT("Configuration not found", m_configuration != NULL);

    std::vector < ::fwRuntime::ConfigurationElement::sptr > deviceNames = m_configuration->find("deviceName");
    if(!deviceNames.empty())
    {
        for(auto dn : deviceNames)
        {
            m_client.addAuthorizedDevice(dn->getValue());
        }
        m_client.setFilteringByDeviceName(true);
    }

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    std::vector< ConfigurationType > inoutCfgs = m_configuration->find("inout");

    SLM_ASSERT("Missing 'in group=\"objects\"'", inoutCfgs[0]->getAttributeValue("group") == "objects");

    std::vector< ConfigurationType > objectsCfgs = inoutCfgs[0]->find("key");
    for(ConfigurationType cfg : objectsCfgs)
    {
        if (cfg->hasAttribute("name"))
        {
            std::string deviceName = cfg->getAttributeValue("name");
            m_deviceNames.push_back(deviceName);
            m_client.addAuthorizedDevice(deviceName);
        }
        else
        {
            m_deviceNames.push_back(m_deviceName);
        }
    }
    if (m_configuration->findConfigurationElement("server"))
    {
        std::string serverInfo = m_configuration->findConfigurationElement("server")->getValue();
        SLM_INFO("OpenIGTLinkListener::configure server: " + serverInfo);
        std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        std::string hostnameStr = serverInfo.substr(0, splitPosition);
        std::string portStr     = serverInfo.substr(splitPosition + 1, serverInfo.size());

        m_hostnameKey = this->getPreferenceKey(hostnameStr);
        m_portKey     = this->getPreferenceKey(portStr);

        if(m_hostnameKey.empty())
        {
            m_hostname = hostnameStr;
        }
        if(m_portKey.empty())
        {
            m_port = ::boost::lexical_cast< std::uint16_t > (portStr);
        }
    }
    else
    {
        throw ::fwTools::Failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void SClientSender::runClient() throw (::fwTools::Failed)
{
    ::fwGui::dialog::MessageDialog msgDialog;

    // 1. Connection
    try
    {
        m_client.connect(m_hostname, m_port);
    }
    catch (::fwCore::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            msgDialog.showMessageDialog("Connection error", ex.what());
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

void SClientSender::setHost(std::string const& hostname, std::uint16_t const port) throw (::fwTools::Failed)
{
    m_hostname = hostname;
    this->setPort(port);
}

//-----------------------------------------------------------------------------

void SClientSender::setPort(boost::uint16_t const port) throw (::fwTools::Failed)
{
    m_port = port;
}

//-----------------------------------------------------------------------------

void SClientSender::starting() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SClientSender::stopping() throw (::fwTools::Failed)
{
    this->::ioNetwork::INetworkSender::stopping();
    this->stopSending();
}

//-----------------------------------------------------------------------------

void SClientSender::startSending()
{
    if(!m_client.isConnected())
    {
        if(!m_hostnameKey.empty())
        {
            std::string hostname = ::fwPreferences::getPreference(m_hostnameKey);
            if(!hostname.empty())
            {
                m_hostname = hostname;
            }
        }
        if(!m_portKey.empty())
        {
            std::string port = ::fwPreferences::getPreference(m_portKey);
            if(!port.empty())
            {
                m_port = ::boost::lexical_cast< std::uint16_t > (port);
            }
        }
        std::function<void() > task = std::bind(&SClientSender::runClient, this);
        m_clientWorker = ::fwThread::Worker::New();

        m_clientWorker->post(task);
        m_sigServerStarted->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SClientSender::stopSending()
{
    if(m_client.isConnected())
    {
        ::fwGui::dialog::MessageDialog msgDialog;

        try
        {
            m_client.disconnect();
            m_clientWorker->stop();
            m_sigServerStopped->asyncEmit();
        }
        catch (::fwCore::Exception& e)
        {
            msgDialog.showMessageDialog("Error", e.what());
        }
    }
}

//-----------------------------------------------------------------------------

std::string SClientSender::getPreferenceKey(const std::string& key) const
{
    std::string keyResult;
    size_t first = key.find('%');
    size_t last  = key.rfind('%');
    if (first == 0 && last == key.size() - 1)
    {
        keyResult = key.substr(1, key.size() - 2);
    }
    return keyResult;
}

//-----------------------------------------------------------------------------

void SClientSender::sendObject(const ::fwData::Object::sptr& obj)
{
    if (m_client.isConnected())
    {
        if (!m_deviceName.empty())
        {
            m_client.setDeviceNameOut(m_deviceName);
        }
        m_client.sendObject(obj);
    }
}

//-----------------------------------------------------------------------------

void SClientSender::sendObject(const ::fwData::Object::sptr& obj, const size_t index)
{
    if (m_client.isConnected())
    {
        if (!m_deviceNames[index].empty())
        {
            m_client.setDeviceNameOut(m_deviceNames[index]);
        }
        m_client.sendObject(obj);
    }
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL
