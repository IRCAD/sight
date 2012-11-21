/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SIGNAL_HXX__
#define __FWCOM_SIGNAL_HXX__

#ifndef __FWCOM_SIGNAL_HPP__
#error fwCom/Signal.hpp not included
#endif

#include <boost/foreach.hpp>
#include <boost/function_types/function_arity.hpp>
#include <boost/make_shared.hpp>

#include "fwCom/exception/BadSlot.hpp"
#include "fwCom/exception/AlreadyConnected.hpp"
#include "fwCom/SlotConnection.hpp"
#include "fwCom/SlotConnection.hxx"
#include "fwCom/Slot.hpp"
#include "fwCom/Slot.hxx"
#include "fwCom/util/remove_last_arg.hpp"
#include "fwCom/util/log.hpp"


namespace fwCom
{
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
typename Signal< R ( A1, A2, A3 ) >::sptr Signal< R ( A1, A2, A3 ) >::New()
{
    return ::boost::make_shared< Signal< R ( A1, A2, A3 ) > > () ;
}




template < typename R, typename A1, typename A2 >
typename Signal< R ( A1, A2 ) >::sptr Signal< R ( A1, A2 ) >::New()
{
    return ::boost::make_shared< Signal< R ( A1, A2 ) > > () ;
}




template < typename R, typename A1 >
typename Signal< R ( A1 ) >::sptr Signal< R ( A1 ) >::New()
{
    return ::boost::make_shared< Signal< R ( A1 ) > > () ;
}




template < typename R>
typename Signal< R () >::sptr Signal< R () >::New()
{
    return ::boost::make_shared< Signal< R () > > () ;
}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
typename Signal< R (A...) >::sptr Signal< R (A...) >::New()
{
    return ::boost::make_shared< Signal< R (A...) > > () ;
}




#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
Connection Signal< R ( A1, A2, A3 ) >::connect( SlotBase::sptr slot )
{
    return this->connect< SignatureType >(slot);
}


template < typename R, typename A1, typename A2 >
Connection Signal< R ( A1, A2 ) >::connect( SlotBase::sptr slot )
{
    return this->connect< SignatureType >(slot);
}


template < typename R, typename A1 >
Connection Signal< R ( A1 ) >::connect( SlotBase::sptr slot )
{
    return this->connect< SignatureType >(slot);
}


template < typename R>
Connection Signal< R () >::connect( SlotBase::sptr slot )
{
    return this->connect< SignatureType >(slot);
}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
Connection Signal< R (A...) >::connect( SlotBase::sptr slot )
{
    return this->connect< SignatureType >(slot);
}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
void Signal< R ( A1, A2, A3 ) >::disconnect( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadToWriteLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter != m_connections.end())
    {
        SlotConnectionBase::sptr connection ( iter->second.lock() );
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



template < typename R, typename A1, typename A2 >
void Signal< R ( A1, A2 ) >::disconnect( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadToWriteLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter != m_connections.end())
    {
        SlotConnectionBase::sptr connection ( iter->second.lock() );
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



template < typename R, typename A1 >
void Signal< R ( A1 ) >::disconnect( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadToWriteLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter != m_connections.end())
    {
        SlotConnectionBase::sptr connection ( iter->second.lock() );
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



template < typename R>
void Signal< R () >::disconnect( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadToWriteLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter != m_connections.end())
    {
        SlotConnectionBase::sptr connection ( iter->second.lock() );
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



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
void Signal< R (A...) >::disconnect( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadToWriteLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter != m_connections.end())
    {
        SlotConnectionBase::sptr connection ( iter->second.lock() );
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



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
void Signal< R ( A1, A2, A3 ) >::disconnectAll()
{
    ::fwCore::mt::WriteLock lock(m_connectionsMutex);

    ConnectionMapType connections = m_connections;

    BOOST_FOREACH( const typename ConnectionMapType::value_type &conn, connections )
    {
        SlotConnectionBase::sptr connection( conn.second.lock() );

        if(connection)
        {
            connection->disconnectWeakLock();
        }
    }

}


template < typename R, typename A1, typename A2 >
void Signal< R ( A1, A2 ) >::disconnectAll()
{
    ::fwCore::mt::WriteLock lock(m_connectionsMutex);

    ConnectionMapType connections = m_connections;

    BOOST_FOREACH( const typename ConnectionMapType::value_type &conn, connections )
    {
        SlotConnectionBase::sptr connection( conn.second.lock() );

        if(connection)
        {
            connection->disconnectWeakLock();
        }
    }

}


template < typename R, typename A1 >
void Signal< R ( A1 ) >::disconnectAll()
{
    ::fwCore::mt::WriteLock lock(m_connectionsMutex);

    ConnectionMapType connections = m_connections;

    BOOST_FOREACH( const typename ConnectionMapType::value_type &conn, connections )
    {
        SlotConnectionBase::sptr connection( conn.second.lock() );

        if(connection)
        {
            connection->disconnectWeakLock();
        }
    }

}


template < typename R>
void Signal< R () >::disconnectAll()
{
    ::fwCore::mt::WriteLock lock(m_connectionsMutex);

    ConnectionMapType connections = m_connections;

    BOOST_FOREACH( const typename ConnectionMapType::value_type &conn, connections )
    {
        SlotConnectionBase::sptr connection( conn.second.lock() );

        if(connection)
        {
            connection->disconnectWeakLock();
        }
    }

}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
void Signal< R (A...) >::disconnectAll()
{
    ::fwCore::mt::WriteLock lock(m_connectionsMutex);

    ConnectionMapType connections = m_connections;

    BOOST_FOREACH( const typename ConnectionMapType::value_type &conn, connections )
    {
        SlotConnectionBase::sptr connection( conn.second.lock() );

        if(connection)
        {
            connection->disconnectWeakLock();
        }
    }

}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
void Signal< R ( A1, A2, A3 ) >::emit( A1 a1, A2 a2, A3 a3 ) const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'run' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->run( a1, a2, a3 );
        }
    }
}


template < typename R, typename A1, typename A2 >
void Signal< R ( A1, A2 ) >::emit( A1 a1, A2 a2 ) const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'run' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->run( a1, a2 );
        }
    }
}


template < typename R, typename A1 >
void Signal< R ( A1 ) >::emit( A1 a1 ) const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'run' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->run( a1 );
        }
    }
}


template < typename R>
void Signal< R () >::emit() const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'run' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->run();
        }
    }
}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
void Signal< R (A...) >::emit( A...a ) const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'run' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->run(a...);
        }
    }
}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
void Signal< R ( A1, A2, A3 ) >::asyncEmit( A1 a1, A2 a2, A3 a3 ) const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'asyncRun' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->asyncRun( a1, a2, a3 );
        }
    }
}



template < typename R, typename A1, typename A2 >
void Signal< R ( A1, A2 ) >::asyncEmit( A1 a1, A2 a2 ) const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'asyncRun' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->asyncRun( a1, a2 );
        }
    }
}



template < typename R, typename A1 >
void Signal< R ( A1 ) >::asyncEmit( A1 a1 ) const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'asyncRun' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->asyncRun( a1 );
        }
    }
}



template < typename R>
void Signal< R () >::asyncEmit() const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'asyncRun' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->asyncRun();
        }
    }
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
void Signal< R (A...) >::asyncEmit( A...a ) const
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);
    typename SlotContainerType::const_iterator iter;
    typename SlotContainerType::const_iterator end = m_slots.end();
    for ( iter = m_slots.begin() ; iter != end; ++iter )
    {
        if ((*iter)->first)
        {
            OSLM_COM("Signal '"<< this->getID() <<"' calls 'asyncRun' method on slot '"<< (*iter)->second->getID() <<"'");
            (*iter)->second->asyncRun(a...);
        }
    }
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
template< typename FROM_F >
Connection Signal< R( A1, A2, A3 ) >::connect( SlotBase::sptr slot )
{
    {
        ::fwCore::mt::ReadLock lock(m_connectionsMutex);

        if(m_connections.find( slot ) != m_connections.end())
        {
            FW_RAISE_EXCEPTION( fwCom::exception::AlreadyConnected("Slot already connected") );
        }
    }

    typedef SlotConnection< void( A1, A2, A3 ) > ConnectionType;
    Connection connection;

    unsigned int sigArity = ::boost::function_types::function_arity< SignatureType >::value;
    if ( sigArity == slot->arity() )
    {
        SlotSptr slotToConnect = boost::dynamic_pointer_cast< SlotRunType >(slot);
        if(slotToConnect)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            typename Signal< R( A1, A2, A3 ) >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R( A1, A2, A3 ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slotToConnect);
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
        typename WrappedSlotRunType::sptr wrappedSlot = boost::dynamic_pointer_cast< WrappedSlotRunType >(slot);

        if(wrappedSlot)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            SlotSptr slotToConnect = Slot < Slot < void ( A1, A2, A3 ) > >::New(wrappedSlot);
            typename Signal< R( A1, A2, A3 ) >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R( A1, A2, A3 ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slot, slotToConnect );
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



template < typename R, typename A1, typename A2 >
template< typename FROM_F >
Connection Signal< R( A1, A2 ) >::connect( SlotBase::sptr slot )
{
    {
        ::fwCore::mt::ReadLock lock(m_connectionsMutex);

        if(m_connections.find( slot ) != m_connections.end())
        {
            FW_RAISE_EXCEPTION( fwCom::exception::AlreadyConnected("Slot already connected") );
        }
    }

    typedef SlotConnection< void( A1, A2 ) > ConnectionType;
    Connection connection;

    unsigned int sigArity = ::boost::function_types::function_arity< SignatureType >::value;
    if ( sigArity == slot->arity() )
    {
        SlotSptr slotToConnect = boost::dynamic_pointer_cast< SlotRunType >(slot);
        if(slotToConnect)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            typename Signal< R( A1, A2 ) >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R( A1, A2 ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slotToConnect);
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
        typename WrappedSlotRunType::sptr wrappedSlot = boost::dynamic_pointer_cast< WrappedSlotRunType >(slot);

        if(wrappedSlot)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            SlotSptr slotToConnect = Slot < Slot < void ( A1, A2 ) > >::New(wrappedSlot);
            typename Signal< R( A1, A2 ) >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R( A1, A2 ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slot, slotToConnect );
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



template < typename R, typename A1 >
template< typename FROM_F >
Connection Signal< R( A1 ) >::connect( SlotBase::sptr slot )
{
    {
        ::fwCore::mt::ReadLock lock(m_connectionsMutex);

        if(m_connections.find( slot ) != m_connections.end())
        {
            FW_RAISE_EXCEPTION( fwCom::exception::AlreadyConnected("Slot already connected") );
        }
    }

    typedef SlotConnection< void( A1 ) > ConnectionType;
    Connection connection;

    unsigned int sigArity = ::boost::function_types::function_arity< SignatureType >::value;
    if ( sigArity == slot->arity() )
    {
        SlotSptr slotToConnect = boost::dynamic_pointer_cast< SlotRunType >(slot);
        if(slotToConnect)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            typename Signal< R( A1 ) >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R( A1 ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slotToConnect);
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
        typename WrappedSlotRunType::sptr wrappedSlot = boost::dynamic_pointer_cast< WrappedSlotRunType >(slot);

        if(wrappedSlot)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            SlotSptr slotToConnect = Slot < Slot < void ( A1 ) > >::New(wrappedSlot);
            typename Signal< R( A1 ) >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R( A1 ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slot, slotToConnect );
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



template < typename R>
template< typename FROM_F >
Connection Signal< R() >::connect( SlotBase::sptr slot )
{
    {
        ::fwCore::mt::ReadLock lock(m_connectionsMutex);

        if(m_connections.find( slot ) != m_connections.end())
        {
            FW_RAISE_EXCEPTION( fwCom::exception::AlreadyConnected("Slot already connected") );
        }
    }

    typedef SlotConnection< void() > ConnectionType;
    Connection connection;

    unsigned int sigArity = ::boost::function_types::function_arity< SignatureType >::value;
    if ( sigArity == slot->arity() )
    {
        SlotSptr slotToConnect = boost::dynamic_pointer_cast< SlotRunType >(slot);
        if(slotToConnect)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            typename Signal< R() >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R() > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slotToConnect);
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
        typename WrappedSlotRunType::sptr wrappedSlot = boost::dynamic_pointer_cast< WrappedSlotRunType >(slot);

        if(wrappedSlot)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            SlotSptr slotToConnect = Slot < Slot < void () > >::New(wrappedSlot);
            typename Signal< R() >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R() > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slot, slotToConnect );
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



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
template< typename FROM_F >
Connection Signal< R( A... ) >::connect( SlotBase::sptr slot )
{
    {
        ::fwCore::mt::ReadLock lock(m_connectionsMutex);

        if(m_connections.find( slot ) != m_connections.end())
        {
            FW_RAISE_EXCEPTION( fwCom::exception::AlreadyConnected("Slot already connected") );
        }
    }

    typedef SlotConnection< void( A... ) > ConnectionType;
    Connection connection;

    unsigned int sigArity = ::boost::function_types::function_arity< SignatureType >::value;
    if ( sigArity == slot->arity() )
    {
        SlotSptr slotToConnect = boost::dynamic_pointer_cast< SlotRunType >(slot);
        if(slotToConnect)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            typename Signal< R( A... ) >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R( A... ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slotToConnect);
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
        typename WrappedSlotRunType::sptr wrappedSlot = boost::dynamic_pointer_cast< WrappedSlotRunType >(slot);

        if(wrappedSlot)
        {
            ::fwCore::mt::WriteLock lock(m_connectionsMutex);
            SlotSptr slotToConnect = Slot < Slot < void (A...) > >::New(wrappedSlot);
            typename Signal< R( A... ) >::sptr sig =
                boost::dynamic_pointer_cast < Signal< R( A... ) > > ( this->shared_from_this() );
            typename ConnectionType::sptr slotConnection =
                ConnectionType::New( sig, slot, slotToConnect );
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



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename R, typename A1, typename A2, typename A3 >
Connection Signal< R( A1, A2, A3 ) >::getConnection( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter == m_connections.end())
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::BadSlot( "No such slot connected" ) );
    }

    SlotConnectionBase::sptr slotConnection (m_connections[slot]);
    return Connection( slotConnection );

}


template < typename R, typename A1, typename A2 >
Connection Signal< R( A1, A2 ) >::getConnection( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter == m_connections.end())
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::BadSlot( "No such slot connected" ) );
    }

    SlotConnectionBase::sptr slotConnection (m_connections[slot]);
    return Connection( slotConnection );

}


template < typename R, typename A1 >
Connection Signal< R( A1 ) >::getConnection( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter == m_connections.end())
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::BadSlot( "No such slot connected" ) );
    }

    SlotConnectionBase::sptr slotConnection (m_connections[slot]);
    return Connection( slotConnection );

}


template < typename R>
Connection Signal< R() >::getConnection( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter == m_connections.end())
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::BadSlot( "No such slot connected" ) );
    }

    SlotConnectionBase::sptr slotConnection (m_connections[slot]);
    return Connection( slotConnection );

}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename R, typename ...A >
Connection Signal< R( A... ) >::getConnection( SlotBase::sptr slot )
{
    ::fwCore::mt::ReadLock lock(m_connectionsMutex);

    ConnectionMapType::const_iterator iter = m_connections.find(slot);

    if (iter == m_connections.end())
    {
        FW_RAISE_EXCEPTION( ::fwCom::exception::BadSlot( "No such slot connected" ) );
    }

    SlotConnectionBase::sptr slotConnection (m_connections[slot]);
    return Connection( slotConnection );

}


#endif  // BOOST_NO_VARIADIC_TEMPLATES
} // namespace fwCom

#endif /* __FWCOM_SIGNAL_HXX__ */


