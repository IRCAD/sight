/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

// define required by Slots.hxx
#define __FWCOM_HASSLOTS_HPP__

#include "core/com/Slots.hpp"
#include "core/com/util/convert_function_type.hpp"
#include "core/config.hpp"
#include "core/function.hpp"

namespace sight::core::com
{

struct SlotBase;

/**
 * @brief   This class proposes a mapping between a SlotKeyType and a SlotBase.
 */
class CORE_CLASS_API HasSlots
{
public:

    typedef std::shared_ptr<HasSlots> sptr;
    typedef std::shared_ptr<const HasSlots> csptr;

    HasSlots()
    {
    }

    virtual ~HasSlots()
    {
    }

    SPTR(SlotBase) slot(const Slots::SlotKeyType& key) const
    {
        return m_slots[key];
    }

    template<typename SlotType>
    SPTR(SlotType) slot(const Slots::SlotKeyType& key) const
    {
        SPTR(SlotType) slot = std::dynamic_pointer_cast<SlotType>(this->slot(key));
        return slot;
    }

    template<typename F, typename A>
    SPTR(Slot<typename core::com::util::convert_function_type<F>::type>) newSlot(
        const Slots::SlotKeyType& key,
        F f,
        A a
    );

    template<typename F>
    SPTR(Slot<core::lambda_to_function_t<F> >) newSlot(const Slots::SlotKeyType& key, F f);

protected:

    /// Copy constructor forbidden
    HasSlots(const HasSlots&);

    /// Copy operator forbidden
    HasSlots& operator=(const HasSlots&);

    Slots m_slots;
};

} // namespace sight::core::com
