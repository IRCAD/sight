/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioIGTL/SOpenIGTLinkSender.hpp"

#include "ioIGTL/helper/preferences.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <functional>

fwServicesRegisterMacro(::ioNetwork::INetworkSender, ::ioIGTL::SOpenIGTLinkSender);

namespace ioIGTL
{

//-----------------------------------------------------------------------------

SOpenIGTLinkSender::SOpenIGTLinkSender()
{
    m_server = std::make_shared< ::igtlNetwork::Server>();
}

//-----------------------------------------------------------------------------

SOpenIGTLinkSender::~SOpenIGTLinkSender()
{
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkSender::configuring() throw (::fwTools::Failed)
{
    ::fwServices::IService::ConfigType config = this->getConfigTree().get_child("service");

    m_portConfig = config.get("port", "4242");
    m_deviceName = config.get("deviceName", "");
}

//-----------------------------------------------------------------------------

void SOpenIGTLinkSender::starting() throw (::fwTools::Failed)
{
    ::ioNetwork::INetworkSender::starting();

    try
    {
        std::uint16_t port = ::ioIGTL::helper::getPreferenceKey<std::uint16_t>(m_portConfig);
        m_server->start(port);

        m_serverFuture = std::async(std::launch::async, std::bind(&::igtlNetwork::Server::runServer, m_server) );
        m_sigServerStarted->asyncEmit();
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

void SOpenIGTLinkSender::stopping() throw (::fwTools::Failed)
{
    ::fwGui::dialog::MessageDialog msgDialog;

    try
    {
        if(m_server->isStarted())
        {
            m_server->stop();
        }
        m_serverFuture.wait();
        m_sigServerStopped->asyncEmit();
    }
    catch (::fwCore::Exception& e)
    {
        msgDialog.showMessageDialog("Error", e.what());
    }
    catch (std::future_error& e)
    {
        // This happens when the server failed to start, so we just ignore it silently.
    }

    ::ioNetwork::INetworkSender::stopping();
}

//-----------------------------------------------------------------------------
void SOpenIGTLinkSender::sendObject(const ::fwData::Object::sptr& obj)
{
    if(m_deviceName != "")
    {
        m_server->setMessageDeviceName(m_deviceName);
    }
    m_server->broadcast(obj);
}

//-----------------------------------------------------------------------------

} // namespace ioIGTL

