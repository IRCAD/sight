/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#if !defined(__FWCOM_SIGNAL_HPP__)
#error core/com/Signal.hpp not included
#endif

#include "core/com/exception/AlreadyConnected.hpp"
#include "core/com/exception/BadSlot.hpp"
#include "core/com/Slot.hpp"
#include "core/com/Slot.hxx"
#include "core/com/SlotConnection.hpp"
#include "core/com/SlotConnection.hxx"
#include "core/com/util/remove_last_arg.hpp"

#include <boost/function_types/function_arity.hpp>

namespace sight::core::com
{

//------------------------------------------------------------------------------

template < typename R, typename ... A >
typename Signal< R(A ...) >::sptr Signal< R(A ...) >::New()
{
    return std::make_shared< Signal< R(A ...) > > ();
}

//-----------------------------------------------------------------------------

template < typename R, typename ... A >
Connection Signal< R(A ...) >::connect( SlotBase::sptr slot )
{
    return this->connect< SignatureType >(slot);
}

//-----------------------------------------------------------------------------

template < typename R, typename ... A >
void Signal< R(A ...) >::disconnect( SlotBase::sptr slot )
{
    core::mt::ReadToWriteLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter != m_connections.end())
    {
        SlotConnectionBase::sptr connection( iter->second.lock() );
        SIGHT_ASSERT( "Connection has been previously destroyed", connection );
        if (connection)
        {
            core::mt::UpgradeToWriteLock writeLock(lock);
            connection->disconnectWeakLock();
            // m_connections.erase(slot.get()); // done in connection->disconnect
        }
    }
    else
    {
        SIGHT_THROW_EXCEPTION( core::com::exception::BadSlot( "No such slot connected" ) );
    }
}

//-----------------------------------------------------------------------------

template < typename R, typename ... A >
void Signal< R(A ...) >::disconnectAll()
{
    core::mt::WriteLock lock(m_connectionsMutex);

    ConnectionMapType connections = m_connections;

    for( const typename ConnectionMapType::value_type& conn : connections )
    {
        SlotConnectionBase::sptr connection( conn.second.lock() );

        if(connection)
        {
            connection->disconnectWeakLock();
        }
    }
}

//-----------------------------------------------------------------------------

template < typename R, typename ... A >
void Signal< R(A ...) >::emit( A ... a ) const
{
    core::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin(); iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            (*iter)->second->run(a ...);
        }
    }
}

//-----------------------------------------------------------------------------

template < typename R, typename ... A >
void Signal< R(A ...) >::asyncEmit( A ... a ) const
{
    core::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin(); iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            (*iter)->second->asyncRun(a ...);
        }
    }
}

//-----------------------------------------------------------------------------

template < typename R, typename ... A >
template< typename FROM_F >
Connection Signal< R( A ... ) >::connect( SlotBase::sptr slot )
{
    {
        core::mt::ReadLock lock(m_connectionsMutex);

        if(m_connections.find( slot ) != m_connections.end())
        {
            SIGHT_THROW_EXCEPTION( core::com::exception::AlreadyConnected("Slot already connected") );
        }
    }

    typedef SlotConnection< void ( A ... ) > ConnectionType;
    Connection connection;

    unsigned int sigArity = ::boost::function_types::function_arity< SignatureType >::value;
    if ( sigArity == slot->arity() )
    {
        SlotSptr slotToConnect = std::dynamic_pointer_cast< SlotRunType >(slot);
        if(slotToConnect)
        {
            core::mt::WriteLock lock(m_connectionsMutex);
            typename Signal< R( A ... ) >::sptr sig =
                std::dynamic_pointer_cast < Signal< R( A ... ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection = ConnectionType::New( sig, slotToConnect);
            slot->m_connections.insert(slotConnection);
            m_connections.insert( typename ConnectionMapType::value_type( slot, slotConnection ) );
            slotConnection->connectNoLock();
            connection = Connection(slotConnection);
        }
        else
        {
            SIGHT_THROW_EXCEPTION( core::com::exception::BadSlot( "Incompatible slot" ) );
        }
    }
    else if ( sigArity > slot->arity() )
    {

        typedef SlotRun< FROM_F > WrappedSlotRunType;
        typename SPTR(WrappedSlotRunType) wrappedSlot = std::dynamic_pointer_cast< WrappedSlotRunType >(slot);

        if(wrappedSlot)
        {
            core::mt::WriteLock lock(m_connectionsMutex);
            SlotSptr slotToConnect                  = Slot < Slot < void (A ...) > >::New(wrappedSlot);
            typename Signal< R( A ... ) >::sptr sig =
                std::dynamic_pointer_cast < Signal< R( A ... ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection = ConnectionType::New( sig, slot, slotToConnect );
            slot->m_connections.insert(slotConnection);
            m_connections.insert( typename ConnectionMapType::value_type( slot, slotConnection ) );
            slotConnection->connectNoLock();
            connection = Connection(slotConnection);
        }
        else
        {
            connection = this->connect< typename core::com::util::remove_last_arg< FROM_F >::type >( slot );
        }

    }
    else
    {
        SIGHT_THROW_EXCEPTION( core::com::exception::BadSlot( "Incompatible slot" ) );
    }

    return connection;
}

//-----------------------------------------------------------------------------

template < typename R, typename ... A >
Connection Signal< R( A ... ) >::getConnection( SlotBase::sptr slot, bool throws )
{
    core::mt::ReadLock lock(m_connectionsMutex);
    Connection connection;

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter == m_connections.end() )
    {
        if(throws)
        {
            SIGHT_THROW_EXCEPTION( core::com::exception::BadSlot( "No such slot connected" ) );
        }
    }
    else
    {
        SlotConnectionBase::sptr slotConnection(iter->second);
        connection = Connection( slotConnection );
    }
    return connection;
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
