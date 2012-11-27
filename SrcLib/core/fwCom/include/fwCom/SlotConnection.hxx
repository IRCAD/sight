/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOTCONNECTION_HXX__
#define __FWCOM_SLOTCONNECTION_HXX__

#ifndef __FWCOM_SLOTCONNECTION_HPP__
#error fwCom/SlotConnection.hpp not included
#endif

#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include "fwCom/SlotBase.hpp"
#include "fwCom/SlotRun.hpp"

#include "fwCom/util/log.hpp"

namespace fwCom
{
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline ::boost::shared_ptr< SlotConnection< void ( A1, A2, A3 ) > >  SlotConnection< void ( A1, A2, A3 ) >::New(
                                                                const SignalSptrType &signal,
                                                                const SlotRunSptrType &slot
                                                                )
{
    return boost::make_shared< SelfType >(signal, slot);
}




template < typename A1, typename A2 >
inline ::boost::shared_ptr< SlotConnection< void ( A1, A2 ) > >  SlotConnection< void ( A1, A2 ) >::New(
                                                                const SignalSptrType &signal,
                                                                const SlotRunSptrType &slot
                                                                )
{
    return boost::make_shared< SelfType >(signal, slot);
}




template < typename A1 >
inline ::boost::shared_ptr< SlotConnection< void ( A1 ) > >  SlotConnection< void ( A1 ) >::New(
                                                                const SignalSptrType &signal,
                                                                const SlotRunSptrType &slot
                                                                )
{
    return boost::make_shared< SelfType >(signal, slot);
}





inline ::boost::shared_ptr< SlotConnection< void () > >  SlotConnection< void () >::New(
                                                                const SignalSptrType &signal,
                                                                const SlotRunSptrType &slot
                                                                )
{
    return boost::make_shared< SelfType >(signal, slot);
}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline ::boost::shared_ptr< SlotConnection< void (A...) > >  SlotConnection< void (A...) >::New(
                                                                const SignalSptrType &signal,
                                                                const SlotRunSptrType &slot
                                                                )
{
    return boost::make_shared< SelfType >(signal, slot);
}




#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline ::boost::shared_ptr< SlotConnection< void ( A1, A2, A3 ) > > SlotConnection< void ( A1, A2, A3 ) >::New(
                                                          const SignalSptrType &signal,
                                                          const SlotBase::sptr &slot,
                                                          const SlotWrapperSptrType &slotWrapper
                                                        )
{
    return boost::make_shared< SelfType >(signal, slot, slotWrapper);
}



template < typename A1, typename A2 >
inline ::boost::shared_ptr< SlotConnection< void ( A1, A2 ) > > SlotConnection< void ( A1, A2 ) >::New(
                                                          const SignalSptrType &signal,
                                                          const SlotBase::sptr &slot,
                                                          const SlotWrapperSptrType &slotWrapper
                                                        )
{
    return boost::make_shared< SelfType >(signal, slot, slotWrapper);
}



template < typename A1 >
inline ::boost::shared_ptr< SlotConnection< void ( A1 ) > > SlotConnection< void ( A1 ) >::New(
                                                          const SignalSptrType &signal,
                                                          const SlotBase::sptr &slot,
                                                          const SlotWrapperSptrType &slotWrapper
                                                        )
{
    return boost::make_shared< SelfType >(signal, slot, slotWrapper);
}




inline ::boost::shared_ptr< SlotConnection< void () > > SlotConnection< void () >::New(
                                                          const SignalSptrType &signal,
                                                          const SlotBase::sptr &slot,
                                                          const SlotWrapperSptrType &slotWrapper
                                                        )
{
    return boost::make_shared< SelfType >(signal, slot, slotWrapper);
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline ::boost::shared_ptr< SlotConnection< void (A...) > > SlotConnection< void (A...) >::New(
                                                          const SignalSptrType &signal,
                                                          const SlotBase::sptr &slot,
                                                          const SlotWrapperSptrType &slotWrapper
                                                        )
{
    return boost::make_shared< SelfType >(signal, slot, slotWrapper);
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline SlotConnection< void ( A1, A2, A3 ) >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotRunSptrType &slot
                                                     )
    : m_signal(signal), m_connectedSlot(slot), m_pair(true, slot.get())
{ }


template < typename A1, typename A2 >
inline SlotConnection< void ( A1, A2 ) >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotRunSptrType &slot
                                                     )
    : m_signal(signal), m_connectedSlot(slot), m_pair(true, slot.get())
{ }


template < typename A1 >
inline SlotConnection< void ( A1 ) >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotRunSptrType &slot
                                                     )
    : m_signal(signal), m_connectedSlot(slot), m_pair(true, slot.get())
{ }



inline SlotConnection< void () >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotRunSptrType &slot
                                                     )
    : m_signal(signal), m_connectedSlot(slot), m_pair(true, slot.get())
{ }


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline SlotConnection< void (A...) >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotRunSptrType &slot
                                                     )
    : m_signal(signal), m_connectedSlot(slot), m_pair(true, slot.get())
{ }


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline SlotConnection< void ( A1, A2, A3 ) >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotBase::sptr &slot,
                                                     const SlotWrapperSptrType &slotWrapper
                                                    )
    : m_signal(signal), m_connectedSlot(slot), m_slotWrapper(slotWrapper), m_pair(true, slotWrapper.get())
{ }



template < typename A1, typename A2 >
inline SlotConnection< void ( A1, A2 ) >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotBase::sptr &slot,
                                                     const SlotWrapperSptrType &slotWrapper
                                                    )
    : m_signal(signal), m_connectedSlot(slot), m_slotWrapper(slotWrapper), m_pair(true, slotWrapper.get())
{ }



template < typename A1 >
inline SlotConnection< void ( A1 ) >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotBase::sptr &slot,
                                                     const SlotWrapperSptrType &slotWrapper
                                                    )
    : m_signal(signal), m_connectedSlot(slot), m_slotWrapper(slotWrapper), m_pair(true, slotWrapper.get())
{ }




inline SlotConnection< void () >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotBase::sptr &slot,
                                                     const SlotWrapperSptrType &slotWrapper
                                                    )
    : m_signal(signal), m_connectedSlot(slot), m_slotWrapper(slotWrapper), m_pair(true, slotWrapper.get())
{ }



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline SlotConnection< void (A...) >::SlotConnection(
                                                     const SignalSptrType &signal,
                                                     const SlotBase::sptr &slot,
                                                     const SlotWrapperSptrType &slotWrapper
                                                    )
    : m_signal(signal), m_connectedSlot(slot), m_slotWrapper(slotWrapper), m_pair(true, slotWrapper.get())
{ }



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline SlotConnection< void ( A1, A2, A3 ) >::~SlotConnection()
{
    this->disconnect();
}







template < typename A1, typename A2 >
inline SlotConnection< void ( A1, A2 ) >::~SlotConnection()
{
    this->disconnect();
}







template < typename A1 >
inline SlotConnection< void ( A1 ) >::~SlotConnection()
{
    this->disconnect();
}








inline SlotConnection< void () >::~SlotConnection()
{
    this->disconnect();
}







//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline SlotConnection< void (A...) >::~SlotConnection()
{
    this->disconnect();
}







#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline void SlotConnection< void ( A1, A2, A3 ) >::connectNoLock()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect '"<< sig->getID() <<"' sig <=> '"<< m_connectedSlot.lock()->getID() <<"' slot");
}



template < typename A1, typename A2 >
inline void SlotConnection< void ( A1, A2 ) >::connectNoLock()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect '"<< sig->getID() <<"' sig <=> '"<< m_connectedSlot.lock()->getID() <<"' slot");
}



template < typename A1 >
inline void SlotConnection< void ( A1 ) >::connectNoLock()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect '"<< sig->getID() <<"' sig <=> '"<< m_connectedSlot.lock()->getID() <<"' slot");
}




inline void SlotConnection< void () >::connectNoLock()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect '"<< sig->getID() <<"' sig <=> '"<< m_connectedSlot.lock()->getID() <<"' slot");
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline void SlotConnection< void (A...) >::connectNoLock()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect '"<< sig->getID() <<"' sig <=> '"<< m_connectedSlot.lock()->getID() <<"' slot");
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline void SlotConnection< void ( A1, A2, A3 ) >::disconnectSignalNoLock(const SignalSptrType &sig)
{
    sig->m_slots.remove( &m_pair );
    sig->m_connections.erase(m_connectedSlot);
    OSLM_COM("Disconnect '"<< sig->getID() <<"' sig <=> '"  <<
            (m_connectedSlot.expired() ? "<dead slot>" : m_connectedSlot.lock()->getID()) <<"' slot" );
}



template < typename A1, typename A2 >
inline void SlotConnection< void ( A1, A2 ) >::disconnectSignalNoLock(const SignalSptrType &sig)
{
    sig->m_slots.remove( &m_pair );
    sig->m_connections.erase(m_connectedSlot);
    OSLM_COM("Disconnect '"<< sig->getID() <<"' sig <=> '"  <<
            (m_connectedSlot.expired() ? "<dead slot>" : m_connectedSlot.lock()->getID()) <<"' slot" );
}



template < typename A1 >
inline void SlotConnection< void ( A1 ) >::disconnectSignalNoLock(const SignalSptrType &sig)
{
    sig->m_slots.remove( &m_pair );
    sig->m_connections.erase(m_connectedSlot);
    OSLM_COM("Disconnect '"<< sig->getID() <<"' sig <=> '"  <<
            (m_connectedSlot.expired() ? "<dead slot>" : m_connectedSlot.lock()->getID()) <<"' slot" );
}




inline void SlotConnection< void () >::disconnectSignalNoLock(const SignalSptrType &sig)
{
    sig->m_slots.remove( &m_pair );
    sig->m_connections.erase(m_connectedSlot);
    OSLM_COM("Disconnect '"<< sig->getID() <<"' sig <=> '"  <<
            (m_connectedSlot.expired() ? "<dead slot>" : m_connectedSlot.lock()->getID()) <<"' slot" );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline void SlotConnection< void (A...) >::disconnectSignalNoLock(const SignalSptrType &sig)
{
    sig->m_slots.remove( &m_pair );
    sig->m_connections.erase(m_connectedSlot);
    OSLM_COM("Disconnect '"<< sig->getID() <<"' sig <=> '"  <<
            (m_connectedSlot.expired() ? "<dead slot>" : m_connectedSlot.lock()->getID()) <<"' slot" );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline void SlotConnection< void ( A1, A2, A3 ) >::disconnectSlotNoLock(const SlotBase::sptr &slot)
{
    try
    {
        boost::shared_ptr< const SlotConnection< void ( A1, A2, A3 ) > > thisSptr =
            boost::dynamic_pointer_cast< const SlotConnection< void ( A1, A2, A3 ) > > ( this->shared_from_this() );
        slot->m_connections.erase( thisSptr );
        OSLM_COM("Disconnect '"
                << (m_signal.expired() ? "<dead signal>" : m_signal.lock()->getID())
                <<"' sig <=> '"  <<  slot->getID() <<"' slot" );
    }
    catch(const boost::bad_weak_ptr &e)
    {
        // SlotConnection destruction is under way, no need to remove
        // shared_ptr from connections
    }
}



template < typename A1, typename A2 >
inline void SlotConnection< void ( A1, A2 ) >::disconnectSlotNoLock(const SlotBase::sptr &slot)
{
    try
    {
        boost::shared_ptr< const SlotConnection< void ( A1, A2 ) > > thisSptr =
            boost::dynamic_pointer_cast< const SlotConnection< void ( A1, A2 ) > > ( this->shared_from_this() );
        slot->m_connections.erase( thisSptr );
        OSLM_COM("Disconnect '"
                << (m_signal.expired() ? "<dead signal>" : m_signal.lock()->getID())
                <<"' sig <=> '"  <<  slot->getID() <<"' slot" );
    }
    catch(const boost::bad_weak_ptr &e)
    {
        // SlotConnection destruction is under way, no need to remove
        // shared_ptr from connections
    }
}



template < typename A1 >
inline void SlotConnection< void ( A1 ) >::disconnectSlotNoLock(const SlotBase::sptr &slot)
{
    try
    {
        boost::shared_ptr< const SlotConnection< void ( A1 ) > > thisSptr =
            boost::dynamic_pointer_cast< const SlotConnection< void ( A1 ) > > ( this->shared_from_this() );
        slot->m_connections.erase( thisSptr );
        OSLM_COM("Disconnect '"
                << (m_signal.expired() ? "<dead signal>" : m_signal.lock()->getID())
                <<"' sig <=> '"  <<  slot->getID() <<"' slot" );
    }
    catch(const boost::bad_weak_ptr &e)
    {
        // SlotConnection destruction is under way, no need to remove
        // shared_ptr from connections
    }
}




inline void SlotConnection< void () >::disconnectSlotNoLock(const SlotBase::sptr &slot)
{
    try
    {
        boost::shared_ptr< const SlotConnection< void () > > thisSptr =
            boost::dynamic_pointer_cast< const SlotConnection< void () > > ( this->shared_from_this() );
        slot->m_connections.erase( thisSptr );
        OSLM_COM("Disconnect '"
                << (m_signal.expired() ? "<dead signal>" : m_signal.lock()->getID())
                <<"' sig <=> '"  <<  slot->getID() <<"' slot" );
    }
    catch(const boost::bad_weak_ptr &e)
    {
        // SlotConnection destruction is under way, no need to remove
        // shared_ptr from connections
    }
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline void SlotConnection< void (A...) >::disconnectSlotNoLock(const SlotBase::sptr &slot)
{
    try
    {
        boost::shared_ptr< const SlotConnection< void (A...) > > thisSptr =
            boost::dynamic_pointer_cast< const SlotConnection< void (A...) > > ( this->shared_from_this() );
        slot->m_connections.erase( thisSptr );
        OSLM_COM("Disconnect '"
                << (m_signal.expired() ? "<dead signal>" : m_signal.lock()->getID())
                <<"' sig <=> '"  <<  slot->getID() <<"' slot" );
    }
    catch(const boost::bad_weak_ptr &e)
    {
        // SlotConnection destruction is under way, no need to remove
        // shared_ptr from connections
    }
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline void SlotConnection< void ( A1, A2, A3 ) >::disconnect()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        ::fwCore::mt::WriteLock lock(sig->m_connectionsMutex);
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}


template < typename A1, typename A2 >
inline void SlotConnection< void ( A1, A2 ) >::disconnect()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        ::fwCore::mt::WriteLock lock(sig->m_connectionsMutex);
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}


template < typename A1 >
inline void SlotConnection< void ( A1 ) >::disconnect()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        ::fwCore::mt::WriteLock lock(sig->m_connectionsMutex);
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}



inline void SlotConnection< void () >::disconnect()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        ::fwCore::mt::WriteLock lock(sig->m_connectionsMutex);
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline void SlotConnection< void (A...) >::disconnect()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        ::fwCore::mt::WriteLock lock(sig->m_connectionsMutex);
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline void SlotConnection< void ( A1, A2, A3 ) >::disconnectWeakLock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}




template < typename A1, typename A2 >
inline void SlotConnection< void ( A1, A2 ) >::disconnectWeakLock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}




template < typename A1 >
inline void SlotConnection< void ( A1 ) >::disconnectWeakLock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}





inline void SlotConnection< void () >::disconnectWeakLock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline void SlotConnection< void (A...) >::disconnectWeakLock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);

    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());

    if(sig)
    {
        this->disconnectSignalNoLock(sig);
    }

    m_slotWrapper.reset();

    if(slot)
    {
        ::fwCore::mt::WriteLock lock(slot->m_connectionsMutex);
        this->disconnectSlotNoLock(slot);
    }

    m_signal.reset();
    m_connectedSlot.reset();
}




#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline SlotConnectionBase::BlockerSptrType SlotConnection< void ( A1, A2, A3 ) >::getBlocker()
{
    ::fwCore::mt::ReadToWriteLock lock(m_mutex);

    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);

        blocker = m_weakBlocker.lock();

        if(!blocker)
        {
            blocker = SlotConnectionBase::BlockerSptrType(
                              (void*)NULL,
                              boost::bind( &SlotConnection< void ( A1, A2, A3 ) >::unblock, this )
                              );
            m_weakBlocker = blocker;

            // signal has to be locked : signal got a pointer on m_pair
            SignalSptrType sig(m_signal);
            ::fwCore::mt::ReadLock lock(sig->m_connectionsMutex);
            m_pair.first = false;
        }
    }
    return blocker;
}




template < typename A1, typename A2 >
inline SlotConnectionBase::BlockerSptrType SlotConnection< void ( A1, A2 ) >::getBlocker()
{
    ::fwCore::mt::ReadToWriteLock lock(m_mutex);

    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);

        blocker = m_weakBlocker.lock();

        if(!blocker)
        {
            blocker = SlotConnectionBase::BlockerSptrType(
                              (void*)NULL,
                              boost::bind( &SlotConnection< void ( A1, A2 ) >::unblock, this )
                              );
            m_weakBlocker = blocker;

            // signal has to be locked : signal got a pointer on m_pair
            SignalSptrType sig(m_signal);
            ::fwCore::mt::ReadLock lock(sig->m_connectionsMutex);
            m_pair.first = false;
        }
    }
    return blocker;
}




template < typename A1 >
inline SlotConnectionBase::BlockerSptrType SlotConnection< void ( A1 ) >::getBlocker()
{
    ::fwCore::mt::ReadToWriteLock lock(m_mutex);

    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);

        blocker = m_weakBlocker.lock();

        if(!blocker)
        {
            blocker = SlotConnectionBase::BlockerSptrType(
                              (void*)NULL,
                              boost::bind( &SlotConnection< void ( A1 ) >::unblock, this )
                              );
            m_weakBlocker = blocker;

            // signal has to be locked : signal got a pointer on m_pair
            SignalSptrType sig(m_signal);
            ::fwCore::mt::ReadLock lock(sig->m_connectionsMutex);
            m_pair.first = false;
        }
    }
    return blocker;
}





inline SlotConnectionBase::BlockerSptrType SlotConnection< void () >::getBlocker()
{
    ::fwCore::mt::ReadToWriteLock lock(m_mutex);

    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);

        blocker = m_weakBlocker.lock();

        if(!blocker)
        {
            blocker = SlotConnectionBase::BlockerSptrType(
                              (void*)NULL,
                              boost::bind( &SlotConnection< void () >::unblock, this )
                              );
            m_weakBlocker = blocker;

            // signal has to be locked : signal got a pointer on m_pair
            SignalSptrType sig(m_signal);
            ::fwCore::mt::ReadLock lock(sig->m_connectionsMutex);
            m_pair.first = false;
        }
    }
    return blocker;
}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline SlotConnectionBase::BlockerSptrType SlotConnection< void (A...) >::getBlocker()
{
    ::fwCore::mt::ReadToWriteLock lock(m_mutex);

    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);

        blocker = m_weakBlocker.lock();

        if(!blocker)
        {
            blocker = SlotConnectionBase::BlockerSptrType(
                              (void*)NULL,
                              boost::bind( &SlotConnection< void (A...) >::unblock, this )
                              );
            m_weakBlocker = blocker;

            // signal has to be locked : signal got a pointer on m_pair
            SignalSptrType sig(m_signal);
            ::fwCore::mt::ReadLock lock(sig->m_connectionsMutex);
            m_pair.first = false;
        }
    }
    return blocker;
}




#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline void SlotConnection< void ( A1, A2, A3 ) >::unblock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    // signal has to be locked : signal got a pointer on m_pair
    SignalSptrType sig(m_signal);
    ::fwCore::mt::ReadLock connectionLock(sig->m_connectionsMutex);
    m_pair.first = true;
}






template < typename A1, typename A2 >
inline void SlotConnection< void ( A1, A2 ) >::unblock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    // signal has to be locked : signal got a pointer on m_pair
    SignalSptrType sig(m_signal);
    ::fwCore::mt::ReadLock connectionLock(sig->m_connectionsMutex);
    m_pair.first = true;
}






template < typename A1 >
inline void SlotConnection< void ( A1 ) >::unblock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    // signal has to be locked : signal got a pointer on m_pair
    SignalSptrType sig(m_signal);
    ::fwCore::mt::ReadLock connectionLock(sig->m_connectionsMutex);
    m_pair.first = true;
}







inline void SlotConnection< void () >::unblock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    // signal has to be locked : signal got a pointer on m_pair
    SignalSptrType sig(m_signal);
    ::fwCore::mt::ReadLock connectionLock(sig->m_connectionsMutex);
    m_pair.first = true;
}






//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline void SlotConnection< void (A...) >::unblock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    // signal has to be locked : signal got a pointer on m_pair
    SignalSptrType sig(m_signal);
    ::fwCore::mt::ReadLock connectionLock(sig->m_connectionsMutex);
    m_pair.first = true;
}






#endif  // BOOST_NO_VARIADIC_TEMPLATES
} // namespace fwCom

#endif /* __FWCOM_SLOTCONNECTION_HXX__ */


