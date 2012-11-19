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
inline void SlotConnection< void ( A1, A2, A3 ) >::connect()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect signal '"<< sig->getID() <<"' <=> slot '"<< m_connectedSlot.lock()->getID() <<"'");
}




template < typename A1, typename A2 >
inline void SlotConnection< void ( A1, A2 ) >::connect()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect signal '"<< sig->getID() <<"' <=> slot '"<< m_connectedSlot.lock()->getID() <<"'");
}




template < typename A1 >
inline void SlotConnection< void ( A1 ) >::connect()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect signal '"<< sig->getID() <<"' <=> slot '"<< m_connectedSlot.lock()->getID() <<"'");
}





inline void SlotConnection< void () >::connect()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect signal '"<< sig->getID() <<"' <=> slot '"<< m_connectedSlot.lock()->getID() <<"'");
}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline void SlotConnection< void (A...) >::connect()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
    OSLM_COM("Connect signal '"<< sig->getID() <<"' <=> slot '"<< m_connectedSlot.lock()->getID() <<"'");
}




#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template < typename A1, typename A2, typename A3 >
inline void SlotConnection< void ( A1, A2, A3 ) >::disconnect()
{
    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());


    if(sig)
    {
        sig->m_slots.remove( &m_pair );
        if(slot)
        {
            OSLM_COM("Disconnect signal '"<< sig->getID() <<"' <=> slot '"<< slot->getID() <<"'");
            sig->m_connections.erase(slot.get());
        }
    }

    m_slotWrapper.reset();

    if(slot)
    {
        try
        {
            boost::shared_ptr< const SlotConnection< void ( A1, A2, A3 ) > > thisSptr =
                boost::dynamic_pointer_cast< const SlotConnection< void ( A1, A2, A3 ) > > ( this->shared_from_this() );
            slot->m_connections.erase( thisSptr );
        }
        catch(const boost::bad_weak_ptr &e)
        {
            // SlotConnection destruction is under way, no need to remove
            // shared_ptr from connections
        }
    }

    m_signal.reset();
    m_connectedSlot.reset();
}




template < typename A1, typename A2 >
inline void SlotConnection< void ( A1, A2 ) >::disconnect()
{
    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());


    if(sig)
    {
        sig->m_slots.remove( &m_pair );
        if(slot)
        {
            OSLM_COM("Disconnect signal '"<< sig->getID() <<"' <=> slot '"<< slot->getID() <<"'");
            sig->m_connections.erase(slot.get());
        }
    }

    m_slotWrapper.reset();

    if(slot)
    {
        try
        {
            boost::shared_ptr< const SlotConnection< void ( A1, A2 ) > > thisSptr =
                boost::dynamic_pointer_cast< const SlotConnection< void ( A1, A2 ) > > ( this->shared_from_this() );
            slot->m_connections.erase( thisSptr );
        }
        catch(const boost::bad_weak_ptr &e)
        {
            // SlotConnection destruction is under way, no need to remove
            // shared_ptr from connections
        }
    }

    m_signal.reset();
    m_connectedSlot.reset();
}




template < typename A1 >
inline void SlotConnection< void ( A1 ) >::disconnect()
{
    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());


    if(sig)
    {
        sig->m_slots.remove( &m_pair );
        if(slot)
        {
            OSLM_COM("Disconnect signal '"<< sig->getID() <<"' <=> slot '"<< slot->getID() <<"'");
            sig->m_connections.erase(slot.get());
        }
    }

    m_slotWrapper.reset();

    if(slot)
    {
        try
        {
            boost::shared_ptr< const SlotConnection< void ( A1 ) > > thisSptr =
                boost::dynamic_pointer_cast< const SlotConnection< void ( A1 ) > > ( this->shared_from_this() );
            slot->m_connections.erase( thisSptr );
        }
        catch(const boost::bad_weak_ptr &e)
        {
            // SlotConnection destruction is under way, no need to remove
            // shared_ptr from connections
        }
    }

    m_signal.reset();
    m_connectedSlot.reset();
}





inline void SlotConnection< void () >::disconnect()
{
    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());


    if(sig)
    {
        sig->m_slots.remove( &m_pair );
        if(slot)
        {
            OSLM_COM("Disconnect signal '"<< sig->getID() <<"' <=> slot '"<< slot->getID() <<"'");
            sig->m_connections.erase(slot.get());
        }
    }

    m_slotWrapper.reset();

    if(slot)
    {
        try
        {
            boost::shared_ptr< const SlotConnection< void () > > thisSptr =
                boost::dynamic_pointer_cast< const SlotConnection< void () > > ( this->shared_from_this() );
            slot->m_connections.erase( thisSptr );
        }
        catch(const boost::bad_weak_ptr &e)
        {
            // SlotConnection destruction is under way, no need to remove
            // shared_ptr from connections
        }
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
    SignalSptrType sig(m_signal.lock());
    SlotBase::sptr slot(m_connectedSlot.lock());


    if(sig)
    {
        sig->m_slots.remove( &m_pair );
        if(slot)
        {
            OSLM_COM("Disconnect signal '"<< sig->getID() <<"' <=> slot '"<< slot->getID() <<"'");
            sig->m_connections.erase(slot.get());
        }
    }

    m_slotWrapper.reset();

    if(slot)
    {
        try
        {
            boost::shared_ptr< const SlotConnection< void (A...) > > thisSptr =
                boost::dynamic_pointer_cast< const SlotConnection< void (A...) > > ( this->shared_from_this() );
            slot->m_connections.erase( thisSptr );
        }
        catch(const boost::bad_weak_ptr &e)
        {
            // SlotConnection destruction is under way, no need to remove
            // shared_ptr from connections
        }
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
    // TODO lock connection
    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        blocker = SlotConnectionBase::BlockerSptrType(
                                                  (void*)NULL,
                                                  boost::bind( &SlotConnection< void ( A1, A2, A3 ) >::unblock, this )
                                                  );
        m_weakBlocker = blocker;
        // TODO lock signal
        m_pair.first = false;
    }
    return blocker;
}




template < typename A1, typename A2 >
inline SlotConnectionBase::BlockerSptrType SlotConnection< void ( A1, A2 ) >::getBlocker()
{
    // TODO lock connection
    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        blocker = SlotConnectionBase::BlockerSptrType(
                                                  (void*)NULL,
                                                  boost::bind( &SlotConnection< void ( A1, A2 ) >::unblock, this )
                                                  );
        m_weakBlocker = blocker;
        // TODO lock signal
        m_pair.first = false;
    }
    return blocker;
}




template < typename A1 >
inline SlotConnectionBase::BlockerSptrType SlotConnection< void ( A1 ) >::getBlocker()
{
    // TODO lock connection
    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        blocker = SlotConnectionBase::BlockerSptrType(
                                                  (void*)NULL,
                                                  boost::bind( &SlotConnection< void ( A1 ) >::unblock, this )
                                                  );
        m_weakBlocker = blocker;
        // TODO lock signal
        m_pair.first = false;
    }
    return blocker;
}





inline SlotConnectionBase::BlockerSptrType SlotConnection< void () >::getBlocker()
{
    // TODO lock connection
    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        blocker = SlotConnectionBase::BlockerSptrType(
                                                  (void*)NULL,
                                                  boost::bind( &SlotConnection< void () >::unblock, this )
                                                  );
        m_weakBlocker = blocker;
        // TODO lock signal
        m_pair.first = false;
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
    // TODO lock connection
    SlotConnectionBase::BlockerSptrType blocker(m_weakBlocker.lock());
    if( !blocker )
    {
        blocker = SlotConnectionBase::BlockerSptrType(
                                                  (void*)NULL,
                                                  boost::bind( &SlotConnection< void (A...) >::unblock, this )
                                                  );
        m_weakBlocker = blocker;
        // TODO lock signal
        m_pair.first = false;
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
    // TODO lock signal
    m_pair.first = true;
}






template < typename A1, typename A2 >
inline void SlotConnection< void ( A1, A2 ) >::unblock()
{
    // TODO lock signal
    m_pair.first = true;
}






template < typename A1 >
inline void SlotConnection< void ( A1 ) >::unblock()
{
    // TODO lock signal
    m_pair.first = true;
}







inline void SlotConnection< void () >::unblock()
{
    // TODO lock signal
    m_pair.first = true;
}






//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template < typename ...A >
inline void SlotConnection< void (A...) >::unblock()
{
    // TODO lock signal
    m_pair.first = true;
}






#endif  // BOOST_NO_VARIADIC_TEMPLATES
} // namespace fwCom

#endif /* __FWCOM_SLOTCONNECTION_HXX__ */


