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

#if !defined(__FWCOM_SIGNAL_HPP__)
#error fwCom/Signal.hpp not included
#endif

#include "fwCom/exception/AlreadyConnected.hpp"
#include "fwCom/exception/BadSlot.hpp"
#include "fwCom/Slot.hpp"
#include "fwCom/Slot.hxx"
#include "fwCom/SlotConnection.hpp"
#include "fwCom/SlotConnection.hxx"
#include "fwCom/util/remove_last_arg.hpp"

#include <boost/function_types/function_arity.hpp>

namespace fwCom
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
    ::fwCore::mt::ReadToWriteLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter != m_connections.end())
    {
        SlotConnectionBase::sptr connection( iter->second.lock() );
        SLM_ASSERT( "Connection has been previously destroyed", connection );
        if (connection)
        {
            ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
            connection->disconnectWeakLock();
            // m_connections.erase(slot.get()); // done in connection->disconnect
        }
    }
    else
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::BadSlot( "No such slot connected" ) );
    }
}

//-----------------------------------------------------------------------------

template < typename R, typename ... A >
void Signal< R(A ...) >::disconnectAll()
{
    ::fwCore::mt::WriteLock lock(m_connectionsMutex);

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
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
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
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
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
        ::fwCore::mt::ReadLock lock(m_connectionsMutex);

        if(m_connections.find( slot ) != m_connections.end())
        {
            FW_RAISE_EXCEPTION( fwCom::exception::AlreadyConnected("Slot already connected") );
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
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
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
            FW_RAISE_EXCEPTION( ::fwCom::exception::BadSlot( "Incompatible slot" ) );
        }
    }
    else if ( sigArity > slot->arity() )
    {

        typedef SlotRun< FROM_F > WrappedSlotRunType;
        typename SPTR(WrappedSlotRunType) wrappedSlot = std::dynamic_pointer_cast< WrappedSlotRunType >(slot);

        if(wrappedSlot)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
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
            connection = this->connect< typename ::fwCom::util::remove_last_arg< FROM_F >::type >( slot );
        }

    }
    else
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::BadSlot( "Incompatible slot" ) );
    }

    return connection;
}

//-----------------------------------------------------------------------------

template < typename R, typename ... A >
Connection Signal< R( A ... ) >::getConnection( SlotBase::sptr slot, bool throws )
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    Connection connection;

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter == m_connections.end() )
    {
        if(throws)
        {
            FW_RAISE_EXCEPTION( ::fwCom::exception::BadSlot( "No such slot connected" ) );
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

} // namespace fwCom
