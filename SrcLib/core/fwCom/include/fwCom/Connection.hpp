/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_CONNECTION_HPP__
#define __FWCOM_CONNECTION_HPP__

#include <fwCore/BaseObject.hpp>

#include "fwCom/config.hpp"

#include "fwCom/SlotConnectionBase.hpp"

namespace fwCom
{

struct Connection
{

    struct Blocker
    {
        Blocker( Connection & connection )
            : m_blocker(connection.getBlocker())
        { }

        void reset()
        {
            m_blocker.reset();
        }

    protected:
        SlotConnectionBase::BlockerSptrType m_blocker;
    };

    Connection()
    { }

    Connection(const SlotConnectionBase::sptr &connection)
        :m_connectionBase(connection)
    {
    }

    void disconnect()
    {
        SlotConnectionBase::sptr connection(m_connectionBase.lock());
        if(connection)
        {
            connection->disconnect();
        }
    };

    bool expired() const
    {
        return m_connectionBase.expired();
    }

protected:

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

#endif /* __FWCOM_CONNECTION_HPP__ */

