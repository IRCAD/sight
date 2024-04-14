/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include <sight/core/config.hpp>

#include "core/com/slot_connection_base.hpp"

namespace sight::core::com
{

/// Class managing Signal-Slot connections.
struct SIGHT_CORE_CLASS_API connection
{
    /// Class allowing to block a Connection.
    struct blocker
    {
        blocker()
        = default;

        blocker(connection _connection) :
            m_blocker(_connection.get_blocker())
        {
        }

        /// Unblock the related Connection.
        void reset()
        {
            m_blocker.reset();
        }

        protected:

            slot_connection_base::blocker_sptr_type m_blocker;
    };

    connection()
    = default;

    connection(const slot_connection_base::sptr& _connection) :
        m_connection_base(_connection)
    {
    }

    /// Disconnect related Connection.
    void disconnect()
    {
        slot_connection_base::sptr connection(m_connection_base.lock());
        if(connection)
        {
            connection->disconnect();
        }
    }

    /// Returns whether this Connection is expired or not.
    [[nodiscard]] bool expired() const
    {
        return m_connection_base.expired();
    }

    protected:

        /// Returns a Blocker.
        slot_connection_base::blocker_sptr_type get_blocker()
        {
            slot_connection_base::blocker_sptr_type blocker;
            slot_connection_base::sptr connection(m_connection_base.lock());
            if(connection)
            {
                blocker = connection->get_blocker();
            }

            return blocker;
        }

        slot_connection_base::wptr m_connection_base;
};

} // namespace sight::core::com
