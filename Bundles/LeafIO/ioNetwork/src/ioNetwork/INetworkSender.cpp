/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include "ioNetwork/INetworkSender.hpp"

#include <fwData/Object.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <boost/lexical_cast.hpp>

namespace ioNetwork
{
const ::fwCom::Slots::SlotKeyType INetworkSender::s_SEND_DATA_SLOT            = "sendData";
const ::fwCom::Signals::SignalKeyType INetworkSender::s_SERVER_STARTED_SIGNAL = "serverStarted";
const ::fwCom::Signals::SignalKeyType INetworkSender::s_SERVER_STOPPED_SIGNAL = "serverStopped";

//-----------------------------------------------------------------------------

INetworkSender::INetworkSender()
{
    ::fwServices::registry::ActiveWorkers::sptr activeWorkers;

    m_sendDataSlot = ::fwCom::newSlot (&INetworkSender::sendData, this);
    ::fwCom::HasSlots::m_slots (INetworkSender::s_SEND_DATA_SLOT, m_sendDataSlot);
    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);

    m_sigServerStarted = ServerStartedSignalType::New();
    m_sigServerStopped = ServerStoppedSignalType::New();

    ::fwCom::HasSignals::m_signals (INetworkSender::s_SERVER_STARTED_SIGNAL,  m_sigServerStarted);
    ::fwCom::HasSignals::m_signals (INetworkSender::s_SERVER_STOPPED_SIGNAL, m_sigServerStopped);
}

//-----------------------------------------------------------------------------

INetworkSender::~INetworkSender()
{
}

//-----------------------------------------------------------------------------

void INetworkSender::configuring() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void INetworkSender::starting() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_connections.connect(this->getObject(), this->getSptr(), this->getObjSrvConnections());
}

//-----------------------------------------------------------------------------

void INetworkSender::stopping() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    m_connections.disconnect();
}

//-----------------------------------------------------------------------------

void INetworkSender::updating() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->sendObject(this->getObject());
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

void INetworkSender::swapping() throw (::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Classic default approach to update service when oject change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioNetwork


