/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_SLOTCONNECTION_HXX__
#define __FWCOM_SLOTCONNECTION_HXX__

#ifndef __FWCOM_SLOTCONNECTION_HPP__
#error fwCom/SlotConnection.hpp not included
#endif

#include "fwCom/SlotRun.hpp"

namespace fwCom
{

//------------------------------------------------------------------------------

template < typename ... A >
inline std::shared_ptr< SlotConnection< void (A ...) > >  SlotConnection< void (A ...) >::New(
    const SignalSptrType& signal,
    const SlotRunSptrType& slot
    )
{
    return std::make_shared< SelfType >(signal, slot);
}

//-----------------------------------------------------------------------------

template < typename ... A >
inline std::shared_ptr< SlotConnection< void (A ...) > > SlotConnection< void (A ...) >::New(
    const SignalSptrType& signal,
    const SlotBase::sptr& slot,
    const SlotWrapperSptrType& slotWrapper
    )
{
    return std::make_shared< SelfType >(signal, slot, slotWrapper);
}

//-----------------------------------------------------------------------------

template < typename ... A >
inline SlotConnection< void (A ...) >::SlotConnection(const SignalSptrType& signal,
                                                      const SlotRunSptrType& slot) :
    m_signal(signal),
    m_connectedSlot(slot),
    m_pair(true, slot.get())
{
}

//-----------------------------------------------------------------------------

template < typename ... A >
inline SlotConnection< void (A ...) >::SlotConnection(
    const SignalSptrType& signal,
    const SlotBase::sptr& slot,
    const SlotWrapperSptrType& slotWrapper
    ) :
    m_signal(signal),
    m_connectedSlot(slot),
    m_slotWrapper(slotWrapper),
    m_pair(true, slotWrapper.get())
{
}

//-----------------------------------------------------------------------------

template < typename ... A >
inline SlotConnection< void (A ...) >::~SlotConnection()
{
    this->disconnect();
}

//-----------------------------------------------------------------------------

template < typename ... A >
inline void SlotConnection< void (A ...) >::connectNoLock()
{
    SignalSptrType sig(m_signal);
    sig->m_slots.push_back( &m_pair );
}

//-----------------------------------------------------------------------------

template < typename ... A >
inline void SlotConnection< void (A ...) >::disconnectSignalNoLock(const SignalSptrType& sig)
{
    sig->m_slots.remove( &m_pair );
    sig->m_connections.erase(m_connectedSlot);
}

//-----------------------------------------------------------------------------

template < typename ... A >
inline void SlotConnection< void (A ...) >::disconnectSlotNoLock(const SlotBase::sptr& slot)
{
    try
    {
        std::shared_ptr< const SlotConnection< void (A ...) > > thisSptr =
            std::dynamic_pointer_cast< const SlotConnection< void (A ...) > > ( this->shared_from_this() );
        slot->m_connections.erase( thisSptr );
    }
    catch(const ::boost::bad_weak_ptr&)
    {
        // SlotConnection destruction is under way, no need to remove
        // shared_ptr from connections
    }
}

//-----------------------------------------------------------------------------

template < typename ... A >
inline void SlotConnection< void (A ...) >::disconnect()
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

//-----------------------------------------------------------------------------

template < typename ... A >
inline void SlotConnection< void (A ...) >::disconnectWeakLock()
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

//-----------------------------------------------------------------------------

template < typename ... A >
inline SlotConnectionBase::BlockerSptrType SlotConnection< void (A ...) >::getBlocker()
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
                std::bind( &SlotConnection< void (A ...) >::unblock, this )
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

//-----------------------------------------------------------------------------

template < typename ... A >
inline void SlotConnection< void (A ...) >::unblock()
{
    ::fwCore::mt::WriteLock lock(m_mutex);
    // signal has to be locked : signal got a pointer on m_pair
    SignalSptrType sig(m_signal);
    ::fwCore::mt::ReadLock connectionLock(sig->m_connectionsMutex);
    m_pair.first = true;
}

//-----------------------------------------------------------------------------

} // namespace fwCom

#endif /* __FWCOM_SLOTCONNECTION_HXX__ */

