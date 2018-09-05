/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioNetwork/INetworkSender.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Object.hpp>

namespace ioNetwork
{

const ::fwCom::Signals::SignalKeyType INetworkSender::s_CONNECTED_SIGNAL    = "connected";
const ::fwCom::Signals::SignalKeyType INetworkSender::s_DISCONNECTED_SIGNAL = "disconnected";

const ::fwServices::IService::KeyType s_OBJECTS_INOUT = "objects";

//-----------------------------------------------------------------------------

INetworkSender::INetworkSender()
{
    m_sigConnected    = newSignal<ConnectedSignalType>(s_CONNECTED_SIGNAL);
    m_sigDisconnected = newSignal<DisconnectSignalType>(s_DISCONNECTED_SIGNAL);
}

//-----------------------------------------------------------------------------

INetworkSender::~INetworkSender()
{
}

//-----------------------------------------------------------------------------

void INetworkSender::updating()
{
    if (this->isStarted())
    {
        const size_t numObjects = this->getKeyGroupSize(s_OBJECTS_INOUT);
        // Grab the objects to send
        for(size_t i = 0; i < numObjects; ++i)
        {
            ::fwData::Object::csptr object = this->getInput< ::fwData::Object >(s_OBJECTS_INOUT, i);

            if (object)
            {
                this->sendObject(object, i);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void INetworkSender::swapping()
{
    // Classic default approach to update service when object change
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

} // namespace ioNetwork

