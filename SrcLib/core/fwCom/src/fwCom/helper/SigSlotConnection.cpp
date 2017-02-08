/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCom/helper/SigSlotConnection.hpp"

#include <fwCom/SignalBase.hpp>
#include <fwCom/exception/AlreadyConnected.hpp>
#include <fwCom/exception/BadSlot.hpp>

#include <fwCore/spyLog.hpp>

#include <fwTools/Object.hpp>

#include <boost/foreach.hpp>

namespace fwCom
{
namespace helper
{

//-----------------------------------------------------------------------------

SigSlotConnection::SigSlotConnection()
{
}

//-----------------------------------------------------------------------------

SigSlotConnection::~SigSlotConnection()
{
    this->disconnect();
}

//-----------------------------------------------------------------------------

void SigSlotConnection::connect(const ::fwCom::HasSignals::csptr& hasSignals, ::fwCom::Signals::SignalKeyType signalKey,
                                const ::fwCom::HasSlots::csptr& hasSlots, ::fwCom::Slots::SlotKeyType slotKey )
{
    try
    {
        ::fwCom::Connection connection = hasSignals->signal( signalKey )->connect( hasSlots->slot( slotKey ) );
        m_connections.push_back(connection);
    }
    catch (::fwCom::exception::BadSlot& e)
    {
        OSLM_ERROR("Can't connect signal '" + signalKey + "' with slot '" + slotKey + "' : " << e.what() << ".");
    }
    catch (::fwCom::exception::AlreadyConnected& e)
    {
        const ::fwTools::Object::csptr source = ::fwTools::Object::dynamicCast(hasSignals);
        auto sourceID                         = source ? source->getID() : "";

        const ::fwTools::Object::csptr target = ::fwTools::Object::dynamicCast(hasSlots);
        auto targetID                         = target ? target->getID() : "";

        OSLM_ERROR("Can't connect signal '" + sourceID + "/" + signalKey + "' with slot '"
                   + targetID + "/" + slotKey + "' : " << e.what() << ".");
    }
}

//-----------------------------------------------------------------------------

void SigSlotConnection::connect(const ::fwCom::HasSignals::csptr& hasSignals,
                                const ::fwCom::HasSlots::csptr& hasSlots,
                                const KeyConnectionsType& keyConnections )
{
    SLM_ASSERT("Signal source is NULL", hasSignals);
    SLM_ASSERT("Slot destination is NULL", hasSlots);

    for( const KeyConnectionType& keys : keyConnections )
    {
        auto signal = hasSignals->signal( keys.first );
        SLM_ASSERT("Signal '" + keys.first + "' not found.", signal);
        auto slot = hasSlots->slot( keys.second );
        SLM_ASSERT("Slot '" + keys.second + "' not found.", slot);

        try
        {
            ::fwCom::Connection connection = signal->connect( slot );
            m_connections.push_back(connection);
        }
        catch (::fwCom::exception::BadSlot& e)
        {
            OSLM_ERROR("Can't connect signal '" + keys.first + "' with slot '" + keys.second + "' : "
                       << e.what() << ".");
        }
        catch (::fwCom::exception::AlreadyConnected& e)
        {
            const ::fwTools::Object::csptr source = ::fwTools::Object::dynamicCast(hasSignals);
            auto sourceID                         = source ? source->getID() : "";

            const ::fwTools::Object::csptr target = ::fwTools::Object::dynamicCast(hasSlots);
            auto targetID                         = target ? target->getID() : "";

            OSLM_ERROR("Can't connect signal '" + sourceID + "/" + keys.first + "' with slot '"
                       + targetID + "/" + keys.second + "' : " << e.what() << ".");
        }
    }
}

//-----------------------------------------------------------------------------

void SigSlotConnection::addConnection( ::fwCom::Connection connection )
{
    m_connections.push_back(connection);
}

//-----------------------------------------------------------------------------

void SigSlotConnection::disconnect()
{
    BOOST_REVERSE_FOREACH( ::fwCom::Connection& connection, m_connections )
    {
        connection.disconnect();
    }
    m_connections.clear();
}

//-----------------------------------------------------------------------------

} // end namespace helper
} // end namespace fwCom
