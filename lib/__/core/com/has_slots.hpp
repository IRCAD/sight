/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <sight/core/config.hpp>

#include "core/com/slot.hpp"
#include "core/com/slots.hpp"
#include "core/com/util/convert_function_type.hpp"
#include "core/function.hpp"

namespace sight::core::com
{

struct slot_base;

/**
 * @brief   This class proposes a mapping between a key_t and a SlotBase.
 */
class SIGHT_CORE_CLASS_API has_slots
{
public:

    using sptr       = std::shared_ptr<has_slots>;
    using csptr      = std::shared_ptr<const has_slots>;
    using slot_key_t = sight::core::com::slots::key_t;

    has_slots() = default;

    virtual ~has_slots() = default;

    //------------------------------------------------------------------------------

    [[nodiscard]] sight::sptr<slot_base> slot(const slots::key_t& _key) const
    {
        return m_slots[_key];
    }

    //------------------------------------------------------------------------------

    template<typename slot_type>
    [[nodiscard]] sight::sptr<slot_type> slot(const slots::key_t& _key) const
    {
        sight::sptr<slot_type> slot = std::dynamic_pointer_cast<slot_type>(this->slot(_key));
        return slot;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] const core::com::slots& slots() const
    {
        return m_slots;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] core::com::slots& slots()
    {
        return m_slots;
    }

    //------------------------------------------------------------------------------

    template<typename F, typename A>
    sight::sptr<core::com::slot<typename core::com::util::convert_function_type<F>::type> > new_slot(
        const slots::key_t& _key,
        F _f,
        A _a
    );

    template<typename F>
    sight::sptr<core::com::slot<core::lambda_to_function_t<F> > > new_slot(const slots::key_t& _key, F _f);

    template<typename F>
    auto new_slot(
        const slots::key_t& _key,
        F _f
    ) -> std::enable_if_t<std::is_function_v<std::remove_pointer_t<F> >,
                          sight::sptr<core::com::slot<typename core::com::util::convert_function_type<F>::type> > >;

protected:

    /// Copy constructor forbidden
    has_slots(const has_slots&);

    /// Copy operator forbidden
    has_slots& operator=(const has_slots&);

private:

    /// Mapping between a key_t and a slot.
    core::com::slots m_slots;
};

//------------------------------------------------------------------------------

template<typename F, typename A>
sight::sptr<slot<typename core::com::util::convert_function_type<F>::type> > has_slots::new_slot(
    const core::com::slots::key_t& _key,
    F _f,
    A _a
)
{
    auto slot = sight::core::com::new_slot(_f, _a);
    this->m_slots(_key, slot);
    return slot;
}

// Prototype used for lambdas functions
template<typename F>
sight::sptr<slot<core::lambda_to_function_t<F> > > has_slots::new_slot(const core::com::slots::key_t& _key, F _f)
{
    auto slot = sight::core::com::new_slot(_f);
    this->m_slots(_key, slot);
    return slot;
}

//------------------------------------------------------------------------------

template<typename F>
auto has_slots::new_slot(
    const core::com::slots::key_t& _key,
    F _f
) -> std::enable_if_t<std::is_function_v<std::remove_pointer_t<F> >,
                      sight::sptr<core::com::slot<typename core::com::util::convert_function_type<F>::type> > >
{
    auto slot = core::com::new_slot(_f);
    this->m_slots(_key, slot);
    return slot;
}

//------------------------------------------------------------------------------

} // namespace sight::core::com
