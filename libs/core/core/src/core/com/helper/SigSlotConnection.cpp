/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/com/helper/SigSlotConnection.hpp"

#include <core/com/exception/AlreadyConnected.hpp>
#include <core/com/exception/BadSlot.hpp>
#include <core/com/SignalBase.hpp>
#include <core/spyLog.hpp>
#include <core/tools/Object.hpp>

#include <boost/foreach.hpp>

namespace sight::core::com
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

void SigSlotConnection::connect(const core::com::HasSignals::csptr& hasSignals,
                                core::com::Signals::SignalKeyType signalKey,
                                const core::com::HasSlots::csptr& hasSlots, core::com::Slots::SlotKeyType slotKey )
{
    try
    {
        core::com::Connection connection = hasSignals->signal( signalKey )->connect( hasSlots->slot( slotKey ) );
        m_connections.push_back(connection);
    }
    catch (core::com::exception::BadSlot& e)
    {
        SLM_ERROR("Can't connect signal '" + signalKey + "' with slot '" + slotKey + "' : " << e.what() << ".");
    }
    catch (core::com::exception::AlreadyConnected& e)
    {
        const core::tools::Object::csptr source = core::tools::Object::dynamicCast(hasSignals);
        auto sourceID                           = source ? source->getID() : "";

        const core::tools::Object::csptr target = core::tools::Object::dynamicCast(hasSlots);
        auto targetID                           = target ? target->getID() : "";

        SLM_ERROR("Can't connect signal '" + sourceID + "/" + signalKey + "' with slot '"
                  + targetID + "/" + slotKey + "' : " << e.what() << ".");
    }
}

//-----------------------------------------------------------------------------

void SigSlotConnection::connect(const core::com::HasSignals::csptr& hasSignals,
                                const core::com::HasSlots::csptr& hasSlots,
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
            core::com::Connection connection = signal->connect( slot );
            m_connections.push_back(connection);
        }
        catch (core::com::exception::BadSlot& e)
        {
            SLM_ERROR("Can't connect signal '" + keys.first + "' with slot '" + keys.second + "' : "
                      << e.what() << ".");
        }
        catch (core::com::exception::AlreadyConnected& e)
        {
            const core::tools::Object::csptr source = core::tools::Object::dynamicCast(hasSignals);
            auto sourceID                           = source ? source->getID() : "";

            const core::tools::Object::csptr target = core::tools::Object::dynamicCast(hasSlots);
            auto targetID                           = target ? target->getID() : "";

            SLM_ERROR("Can't connect signal '" + sourceID + "/" + keys.first + "' with slot '"
                      + targetID + "/" + keys.second + "' : " << e.what() << ".");
        }
    }
}

//-----------------------------------------------------------------------------

void SigSlotConnection::addConnection( core::com::Connection connection )
{
    m_connections.push_back(connection);
}

//-----------------------------------------------------------------------------

void SigSlotConnection::disconnect()
{
    BOOST_REVERSE_FOREACH( core::com::Connection& connection, m_connections )
    {
        connection.disconnect();
    }
    m_connections.clear();
}

//-----------------------------------------------------------------------------

} // end namespace helper
} // end namespace sight::core::com
