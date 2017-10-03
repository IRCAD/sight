/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioNetwork/INetworkSender.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Object.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <boost/lexical_cast.hpp>

namespace ioNetwork
{
const ::fwCom::Slots::SlotKeyType INetworkSender::s_SEND_DATA_SLOT            = "sendData";
const ::fwCom::Signals::SignalKeyType INetworkSender::s_SERVER_STARTED_SIGNAL = "serverStarted";
const ::fwCom::Signals::SignalKeyType INetworkSender::s_SERVER_STOPPED_SIGNAL = "serverStopped";

const ::fwServices::IService::KeyType s_OBJECTS_INOUT = "objects";

//-----------------------------------------------------------------------------

INetworkSender::INetworkSender()
{
    ::fwServices::registry::ActiveWorkers::sptr activeWorkers;

    m_sendDataSlot = ::fwCom::newSlot(&INetworkSender::sendData, this);
    ::fwCom::HasSlots::m_slots(INetworkSender::s_SEND_DATA_SLOT, m_sendDataSlot);
    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);

    m_sigServerStarted = ServerStartedSignalType::New();
    m_sigServerStopped = ServerStoppedSignalType::New();

    ::fwCom::HasSignals::m_signals(INetworkSender::s_SERVER_STARTED_SIGNAL,  m_sigServerStarted);
    ::fwCom::HasSignals::m_signals(INetworkSender::s_SERVER_STOPPED_SIGNAL, m_sigServerStopped);
}

//-----------------------------------------------------------------------------

INetworkSender::~INetworkSender()
{
}

//-----------------------------------------------------------------------------

void INetworkSender::configuring()
{
}

//-----------------------------------------------------------------------------

void INetworkSender::starting()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void INetworkSender::stopping()
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void INetworkSender::updating()
{
    SLM_TRACE_FUNC();
    if (this->isStarted())
    {
        const size_t numObjects = this->getKeyGroupSize(s_OBJECTS_INOUT);
        // Grab the objects to send
        for(size_t i = 0; i < numObjects; ++i)
        {
            ::fwData::Object::sptr object = this->getInOut< ::fwData::Object >(s_OBJECTS_INOUT, i);

            if (object)
            {
                this->sendObject(object, i);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void INetworkSender::sendData (::fwData::Object::sptr obj)
{
    SLM_TRACE_FUNC();

    OSLM_WARN_IF("Service is stopped, object can not be send", !this->isStarted());
    if (this->isStarted())
    {
        this->sendObject(obj);
    }
}

//-----------------------------------------------------------------------------

void INetworkSender::swapping()
{
    SLM_TRACE_FUNC();

    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioNetwork

