/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwCom/config.hpp"
#include "fwCom/SlotConnectionBase.hpp"

namespace fwCom
{

/// Class managing Signal-Slot connections.
struct FWCOM_CLASS_API Connection
{
    /// Class allowing to block a Connection.
    struct Blocker
    {
        Blocker()
        {
        }

        Blocker( Connection connection ) :
            m_blocker(connection.getBlocker())
        {
        }

        /// Unblock the related Connection.
        void reset()
        {
            m_blocker.reset();
        }

        protected:
            SlotConnectionBase::BlockerSptrType m_blocker;
    };

    Connection()
    {
    }

    Connection(const SlotConnectionBase::sptr& connection) :
        m_connectionBase(connection)
    {
    }

    /// Disconnect related Connection.
    void disconnect()
    {
        SlotConnectionBase::sptr connection(m_connectionBase.lock());
        if(connection)
        {
            connection->disconnect();
        }
    }

    /// Returns whether this Connection is expired or not.
    bool expired() const
    {
        return m_connectionBase.expired();
    }

    protected:

        /// Returns a Blocker.
        SlotConnectionBase::BlockerSptrType getBlocker()
        {
            SlotConnectionBase::BlockerSptrType blocker;
            SlotConnectionBase::sptr connection(m_connectionBase.lock());
            if(connection)
            {
                blocker = connection->getBlocker();
            }
            return blocker;
        }

        SlotConnectionBase::wptr m_connectionBase;

};

} // namespace fwCom
