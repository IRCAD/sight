/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioNetwork/INetworkSender.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Object.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <boost/lexical_cast.hpp>

namespace ioNetwork
{

const ::fwCom::Signals::SignalKeyType INetworkSender::s_SERVER_STARTED_SIGNAL = "serverStarted";
const ::fwCom::Signals::SignalKeyType INetworkSender::s_SERVER_STOPPED_SIGNAL = "serverStopped";

const ::fwServices::IService::KeyType s_OBJECTS_INOUT = "objects";

//-----------------------------------------------------------------------------

INetworkSender::INetworkSender()
{
    m_sigServerStarted = newSignal<ServerStartedSignalType>(s_SERVER_STARTED_SIGNAL);
    m_sigServerStopped = newSignal<ServerStoppedSignalType>(s_SERVER_STOPPED_SIGNAL);
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
}

//-----------------------------------------------------------------------------

void INetworkSender::stopping()
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

