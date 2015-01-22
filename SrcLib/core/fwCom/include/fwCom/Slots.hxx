/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
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

template<typename F, typename ...A>
Slots& Slots::operator()( const SlotKeyType &key, F f, A...a )
{
    SPTR( SlotBase ) slotBase = ::boost::dynamic_pointer_cast< SlotBase >( ::fwCom::newSlot(f, a...) );
    return this->operator()(key, slotBase);
}

} // namespace fwCom

#endif //__FWCOM_SLOTS_HXX__
