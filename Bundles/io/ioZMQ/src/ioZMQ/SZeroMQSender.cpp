/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

fwServicesRegisterMacro(::ioNetwork::INetworkSender, ::ioZMQ::SZeroMQSender);

namespace ioZMQ
{

const ::fwCom::Slots::SlotKeyType SZeroMQSender::s_UPDATE_CONFIGURATION_SLOT = "updateConfiguration";

//-----------------------------------------------------------------------------

SZeroMQSender::SZeroMQSender() :
    m_patternMode(::zmqNetwork::Socket::Publish),
    m_sockMode(::zmqNetwork::Socket::Server)
{
    newSlot(s_UPDATE_CONFIGURATION_SLOT, &SZeroMQSender::updateConfiguration, this);
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
    FW_DEPRECATED_MSG("'ioZMQ' bundle and the associated services are no longer supported.", "19.0");
    try
    {
        m_socket = std::make_shared< ::zmqNetwork::Socket >(m_sockMode, m_patternMode);
        m_socket->start(m_hostStr);
        m_sigConnected->asyncEmit();
    }
    catch (std::exception& err)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Error",
                                                          "Cannot start the sender : " + std::string(err.what()));
    }
}

//-----------------------------------------------------------------------------

void SZeroMQSender::updateConfiguration (const ::zmqNetwork::Socket::PatternMode pattern,
                                         const ::zmqNetwork::Socket::SocketMode sockMode,
                                         const std::string& host)
{
    m_sockMode    = sockMode;
    m_patternMode = pattern;
    m_hostStr     = host;
}

//-----------------------------------------------------------------------------

void SZeroMQSender::stopping()
{
    m_socket->stop();
    m_sigDisconnected->asyncEmit();
}

//-----------------------------------------------------------------------------

void SZeroMQSender::sendObject(const ::fwData::Object::csptr& obj)
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

void SZeroMQSender::sendObject(const ::fwData::Object::csptr& obj,  const size_t)
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
