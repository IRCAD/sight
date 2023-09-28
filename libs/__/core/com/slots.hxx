/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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
#error core/com/slots.hpp not included
#endif

#include "core/com/slot.hpp"
#include "core/com/slot.hxx"

namespace sight::core::com
{

// Only define it when core/com/has_slots.hpp has been included
// This saves us a file like core/com/has_slots.hxx
#if defined(__FWCOM_HASSLOTS_HPP__)
template<typename F, typename A>
SPTR(slot<typename core::com::util::convert_function_type<F>::type>)
has_slots::new_slot(const core::com::slots::key_t& key, F f, A a)
{
    auto slot = core::com::new_slot(f, a);
    this->m_slots(key, slot);
    return slot;
}

// Prototype used for lambdas functions
template<typename F>
SPTR(slot<core::lambda_to_function_t<F> >)
has_slots::new_slot(const core::com::slots::key_t& key, F f)
{
    auto slot = core::com::new_slot(f);
    this->m_slots(key, slot);
    return slot;
}

//------------------------------------------------------------------------------

template<typename F>
auto has_slots::new_slot(
    const core::com::slots::key_t& key,
    F f
) -> std::enable_if_t<std::is_function_v<std::remove_pointer_t<F> >,
                      SPTR(core::com::slot<typename core::com::util::convert_function_type<F>::type>)>
{
    auto slot = core::com::new_slot(f);
    this->m_slots(key, slot);
    return slot;
}
#endif

} // namespace sight::core::com
