/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_SLOTS_HXX__
#define __FWCOM_SLOTS_HXX__


#ifndef __FWCOM_SLOTS_HPP__
#error fwCom/Slots.hpp not included
#endif

#include "fwCom/Slot.hpp"
#include "fwCom/Slot.hxx"


namespace fwCom
{

template<typename F, typename ... A>
Slots& Slots::operator()( const SlotKeyType &key, F f, A ... a )
{
    SPTR( SlotBase ) slotBase = std::dynamic_pointer_cast< SlotBase >( ::fwCom::newSlot(f, a ...) );
    return this->operator()(key, slotBase);
}

// Only define it when fwCom/HasSlots.hpp has been included
// This saves us a file like fwCom/HasSlots.hxx
#ifdef __FWCOM_HASSLOTS_HPP__

template<typename F, typename A>
SPTR(Slot< typename ::fwCom::util::convert_function_type< F >::type >)
HasSlots::newSlot( const ::fwCom::Slots::SlotKeyType & key, F f, A a )
{
    auto slot = ::fwCom::newSlot(f, a);
    this->m_slots(key, slot);
    return slot;
}

#endif

} // namespace fwCom

#endif //__FWCOM_SLOTS_HXX__
