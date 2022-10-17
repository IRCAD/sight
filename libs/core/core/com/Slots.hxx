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

#if !defined(__FWCOM_SLOTS_HPP__)
#error core/com/Slots.hpp not included
#endif

#include "core/com/Slot.hpp"
#include "core/com/Slot.hxx"

namespace sight::core::com
{

// Only define it when core/com/HasSlots.hpp has been included
// This saves us a file like core/com/HasSlots.hxx
#if defined(__FWCOM_HASSLOTS_HPP__)
template<typename F, typename A>
SPTR(Slot<typename core::com::util::convert_function_type<F>::type>)
HasSlots::newSlot(const core::com::Slots::SlotKeyType& key, F f, A a)
{
    auto slot = core::com::newSlot(f, a);
    this->m_slots(key, slot);
    return slot;
}

// Prototype used for lambdas functions
template<typename F>
SPTR(Slot<core::lambda_to_function_t<F> >)
HasSlots::newSlot(const core::com::Slots::SlotKeyType& key, F f)
{
    auto slot = core::com::newSlot(f);
    this->m_slots(key, slot);
    return slot;
}

//------------------------------------------------------------------------------

template<typename F>
auto HasSlots::newSlot(
    const core::com::Slots::SlotKeyType& key,
    F f
) -> std::enable_if_t<std::is_function_v<std::remove_pointer_t<F> >,
                      SPTR(Slot<typename core::com::util::convert_function_type<F>::type>)>
{
    auto slot = core::com::newSlot(f);
    this->m_slots(key, slot);
    return slot;
}
#endif

} // namespace sight::core::com
