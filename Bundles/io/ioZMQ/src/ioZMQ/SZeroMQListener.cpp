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

#include "ioZMQ/SZeroMQListener.hpp"

#include "ioZMQ/Patterns.hpp"
#include "ioZMQ/ZeroMQConfigurationParser.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>

#include <fwData/Object.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <functional>
#include <sstream>

fwServicesRegisterMacro(::ioNetwork::INetworkListener, ::ioZMQ::SZeroMQListener, ::fwData::Object);

namespace ioZMQ
{

const ::fwCom::Slots::SlotKeyType SZeroMQListener::s_UPDATE_CONFIGURATION_SLOT = "updateConfiguration";

//-----------------------------------------------------------------------------

SZeroMQListener::SZeroMQListener() :
    ::ioNetwork::INetworkListener()
{
    m_updateConfigurationSlot = ::fwCom::newSlot(&SZeroMQListener::updateConfiguration, this);
    ::fwCom::HasSlots::m_slots(SZeroMQListener::s_UPDATE_CONFIGURATION_SLOT, m_updateConfigurationSlot );

    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//-----------------------------------------------------------------------------

SZeroMQListener::~SZeroMQListener()
{
}

//-----------------------------------------------------------------------------

void SZeroMQListener::setHost(std::string const& host, std::uint16_t const port)
{
    std::stringstream stream;

    stream << "tcp://" << host << ":" << port;
    m_hostStr = stream.str();
}

//-----------------------------------------------------------------------------

void SZeroMQListener::configuring()
{
    try
    {
        ZeroMQConfigurationParser parser(m_configuration);

        parser.parse(Patterns::getSupportedReaderPatterns());
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

void SZeroMQListener::runReceiver()
{
    ::fwGui::dialog::MessageDialog msgDialog;
    ::fwData::Object::sptr obj;

    try
    {
        m_socket->start(m_hostStr);
    }
    catch (std::exception& err)
    {
        msgDialog.showMessageDialog("Error", "Cannot start the listener : " + std::string(err.what()));
        return;
    }
    m_socket->setReceiveTimeout(1000);
    obj = this->getObject();
    m_sigConnected->asyncEmit();
    while (m_socket->isStarted())
    {
        try
        {
            std::string deviceName;
            ::fwData::Object::sptr receiveObject = m_socket->receiveObject(deviceName);
            if(receiveObject)
            {
                obj->shallowCopy(receiveObject);

                ::fwData::Object::ModifiedSignalType::sptr sig;
                sig = obj->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
                {
                    ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
            }
        }
        catch(std::exception& err)
        {
            OSLM_FATAL("Failed to revieved object: "<< err.what());
        }
    }
    m_socket->stop();
    m_sigDisconnected->asyncEmit();
}

//-----------------------------------------------------------------------------

void SZeroMQListener::updateConfiguration(::zmqNetwork::Socket::PatternMode const pattern,
                                          ::zmqNetwork::Socket::SocketMode const sockMode,
                                          std::string const& host)
{
    m_sockMode    = sockMode;
    m_patternMode = pattern;
    m_hostStr     = host;
}

//-----------------------------------------------------------------------------

void SZeroMQListener::starting()
{
    FW_DEPRECATED_MSG("'ioZMQ' bundle and the associated services are no longer supported.", "19.0");
    std::function<void() > task = std::bind(&SZeroMQListener::runReceiver, this);

    m_socket        = ::zmqNetwork::Socket::sptr(new ::zmqNetwork::Socket(m_sockMode, m_patternMode));
    m_receiveWorker = ::fwThread::Worker::New();
    m_receiveWorker->post(task);
}

//-----------------------------------------------------------------------------

void SZeroMQListener::stopping()
{
    m_socket->terminate();
    m_receiveWorker->stop();
}

//-----------------------------------------------------------------------------

} // namespace ioZMQ
