/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

// cspell:ignore NOLINTNEXTLINE

#include "core/com/helper/SigSlotConnection.hpp"

#include <core/com/exception/AlreadyConnected.hpp>
#include <core/com/exception/BadSlot.hpp>
#include <core/com/SignalBase.hpp>
#include <core/spyLog.hpp>
#include <core/tools/Object.hpp>

#include <boost/foreach.hpp>

namespace sight::core::com::helper
{

//-----------------------------------------------------------------------------

SigSlotConnection::SigSlotConnection()
= default;

//-----------------------------------------------------------------------------

SigSlotConnection::~SigSlotConnection()
{
    this->disconnect();
}

//-----------------------------------------------------------------------------

void SigSlotConnection::connect(
    const core::com::HasSignals::csptr& hasSignals,
    core::com::Signals::SignalKeyType signalKey,
    const core::com::HasSlots::csptr& hasSlots,
    core::com::Slots::SlotKeyType slotKey
)
{
    try
    {
        core::com::Connection connection = hasSignals->signal(signalKey)->connect(hasSlots->slot(slotKey));
        m_connections.push_back(connection);
    }
    catch(core::com::exception::BadSlot& e)
    {
        SIGHT_ERROR(
            std::string("Can't connect signal '").append(signalKey).append("' with slot '").append(slotKey)
            .append("' : ") << e.what() << "."
        );
    }
    catch(core::com::exception::AlreadyConnected& e)
    {
        const core::tools::Object::csptr source = core::tools::Object::dynamicCast(hasSignals);
        auto sourceID                           = source ? source->getID() : "";

        const core::tools::Object::csptr target = core::tools::Object::dynamicCast(hasSlots);
        auto targetID                           = target ? target->getID() : "";

        SIGHT_ERROR(
            std::string("Can't connect signal '").append(sourceID).append("/").append(signalKey).append("' with slot '")
            .append(targetID).append("/").append(slotKey).append("' : ") << e.what() << "."
        );
    }
}

//-----------------------------------------------------------------------------

void SigSlotConnection::connect(
    const core::com::HasSignals::csptr& hasSignals,
    const core::com::HasSlots::csptr& hasSlots,
    const KeyConnectionsType& keyConnections
)
{
    SIGHT_ASSERT("Signal source is NULL", hasSignals);
    SIGHT_ASSERT("Slot destination is NULL", hasSlots);

    for(const KeyConnectionType& keys : keyConnections)
    {
        auto signal = hasSignals->signal(keys.first);
        SIGHT_ASSERT("Signal '" + keys.first + "' not found.", signal);
        auto slot = hasSlots->slot(keys.second);
        SIGHT_ASSERT("Slot '" + keys.second + "' not found.", slot);

        try
        {
            core::com::Connection connection = signal->connect(slot);
            m_connections.push_back(connection);
        }
        catch(core::com::exception::BadSlot& e)
        {
            SIGHT_ERROR(
                "Can't connect signal '" + keys.first + "' with slot '" + keys.second + "' : "
                << e.what() << "."
            );
        }
        catch(core::com::exception::AlreadyConnected& e)
        {
            const core::tools::Object::csptr source = core::tools::Object::dynamicCast(hasSignals);
            auto sourceID                           = source ? source->getID() : "";

            const core::tools::Object::csptr target = core::tools::Object::dynamicCast(hasSlots);
            auto targetID                           = target ? target->getID() : "";

            SIGHT_ERROR(
                std::string("Can't connect signal '").append(sourceID).append("/").append(keys.first)
                .append("' with slot '").append(targetID).append("/").append(keys.second).append(
                    "' : "
                ) << e.what() << "."
            );
        }
    }
}

//-----------------------------------------------------------------------------

void SigSlotConnection::addConnection(core::com::Connection connection)
{
    m_connections.push_back(connection);
}

//-----------------------------------------------------------------------------

void SigSlotConnection::disconnect()
{
    // NOLINTNEXTLINE(bugprone-branch-clone)
    BOOST_REVERSE_FOREACH(core::com::Connection& connection, m_connections)
    {
        connection.disconnect();
    }
    m_connections.clear();
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com::helper
