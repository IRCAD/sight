/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __FWCOM_HASSLOTS_HPP__
#define __FWCOM_HASSLOTS_HPP__

#include "fwCom/config.hpp"
#include "fwCom/Slots.hpp"
#include "fwCom/util/convert_function_type.hpp"

namespace fwCom
{

struct SlotBase;

/**
 * @brief   This class proposes a mapping between a SlotKeyType and a SlotBase.
 */
class HasSlots
{

public:

    typedef std::shared_ptr< HasSlots > sptr;
    typedef std::shared_ptr< const HasSlots > csptr;

    HasSlots()
    {
    }
    virtual ~HasSlots()
    {
    }

    SPTR( SlotBase ) slot( const Slots::SlotKeyType & key ) const
    {
        return m_slots[key];
    }

    template< typename SlotType >
    SPTR( SlotType ) slot( const Slots::SlotKeyType & key ) const
    {
        SPTR( SlotType ) slot = std::dynamic_pointer_cast< SlotType >( this->slot(key) );
        return slot;
    }

    template<typename F, typename A>
    SPTR(Slot< typename ::fwCom::util::convert_function_type< F >::type >) newSlot( const Slots::SlotKeyType & key, F f,
                                                                                    A a );

protected:

    /// Copy constructor forbidden
    HasSlots( const HasSlots& );

    /// Copy operator forbidden
    HasSlots& operator=( const HasSlots& );

    Slots m_slots;
};

} // namespace fwCom

#endif // __FWCOM_HASSLOTS_HPP__
