/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioZMQ/SZeroMQSender.hpp"

#include "ioZMQ/Patterns.hpp"
#include "ioZMQ/ZeroMQConfigurationParser.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/spyLog.hpp>

#include <fwData/Object.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <sstream>

fwServicesRegisterMacro(::ioNetwork::INetworkSender, ::ioZMQ::SZeroMQSender, ::fwData::Object);

namespace ioZMQ
{

const ::fwCom::Slots::SlotKeyType SZeroMQSender::s_UPDATE_CONFIGURATION_SLOT = "updateConfiguration";

//-----------------------------------------------------------------------------

SZeroMQSender::SZeroMQSender() :
    ::ioNetwork::INetworkSender()
{
    m_updateConfigurationSlot = ::fwCom::newSlot(&SZeroMQSender::updateConfiguration, this);
    ::fwCom::HasSlots::m_slots(SZeroMQSender::s_UPDATE_CONFIGURATION_SLOT, m_updateConfigurationSlot);

    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

SZeroMQSender::~SZeroMQSender()
{
}

//-----------------------------------------------------------------------------

void SZeroMQSender::configuring()
{
    try
    {
        ZeroMQConfigurationParser parser(m_configuration);

        parser.parse(Patterns::getSupportedWriterPatterns());
        m_hostStr     = parser.getHostname();
        m_sockMode    = parser.getSocketMode();
        m_patternMode = parser.getPatternMode();
    }
    catch(std::exception& err)
    {
        OSLM_FATAL("Failed to parse configuration "<< err.what());
    }
}

//-----------------------------------------------------------------------------

void SZeroMQSender::setPort (std::uint16_t const port)
{
    std::string newHost;
    std::stringstream stream;

    if (m_hostStr.substr(0, 3) != "tcp")
    {
        throw ::fwTools::Failed("Change port is only supported for tcp protocol");
    }
    newHost = m_hostStr.substr(0, m_hostStr.find(":", 4));
    stream << newHost << ":" << port;
    m_hostStr = stream.str();
}

//-----------------------------------------------------------------------------

void SZeroMQSender::starting()
{
    ::fwGui::dialog::MessageDialog msgDialog;

    try
    {
        ::ioNetwork::INetworkSender::starting();
        m_socket = ::zmqNetwork::Socket::sptr(new ::zmqNetwork::Socket(m_sockMode, m_patternMode));
        m_socket->start(m_hostStr);
        m_sigServerStarted->asyncEmit();
    }
    catch (std::exception& err)
    {
        msgDialog.showMessageDialog("Error", "Cannot start the sender : " + std::string(err.what()));
    }
}

//-----------------------------------------------------------------------------

void SZeroMQSender::updateConfiguration (::zmqNetwork::Socket::PatternMode const pattern,
                                         ::zmqNetwork::Socket::SocketMode const sockMode,
                                         std::string const& host)
{
    m_sockMode    = sockMode;
    m_patternMode = pattern;
    m_hostStr     = host;
}

//-----------------------------------------------------------------------------

void SZeroMQSender::stopping()
{
    m_socket->stop();
    ::ioNetwork::INetworkSender::stopping();
    m_sigServerStopped->asyncEmit();
}

//-----------------------------------------------------------------------------

void SZeroMQSender::sendObject (const ::fwData::Object::sptr& obj)
{
    try
    {
        m_socket->sendObject(obj);
    }
    catch(std::exception& err)
    {
        OSLM_FATAL("Failed to send object: "<< err.what());
    }
}

//-----------------------------------------------------------------------------

void SZeroMQSender::sendObject (const ::fwData::Object::sptr& obj,  const size_t index /* unused */)
{
    try
    {
        m_socket->sendObject(obj);
    }
    catch(std::exception& err)
    {
        OSLM_FATAL("Failed to send object: "<< err.what());
    }
}

//-----------------------------------------------------------------------------

} // namespace ioZMQ

