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

#if !defined(FWCOM_SLOT_HPP)
#error core/com/slot.hpp not included
#endif

#include "core/com/slot_call.hxx"
#include "core/com/util/auto_bind.hpp"
#include "core/com/util/auto_bind.hxx"

#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/result_type.hpp>

#include <type_traits>

namespace sight::core::com
{

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
slot<R(A ...)>::slot() :
    slot_call<R(A ...)>()
{
    // 'this->' is needed by gcc 4.2
    this->slot_base::m_signature = slot_base::get_type_name<R(A ...)>();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<typename R, typename ... A>
template<typename F>
slot<slot<R(A ...)> >::slot(SPTR(slot_run<F>)_slot) :
    core::com::slot<function_t>(
        core::com::util::auto_bind<
            signature_type,
            boost::function_types::function_arity<F>::value
        >::wrap(&slot_run<F>::run, _slot.get()))
{
    static_assert(std::is_same<void, R>::value);
    this->set_worker(_slot->get_worker());
    this->m_source_slot = _slot;
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
template<typename F>
slot<slot<R(A ...)> >::slot(SPTR(slot<F>)_slot) :
    core::com::slot<function_t>(
        core::com::util::auto_bind<
            signature_type,
            boost::function_types::function_arity<F>::value
        >::wrap(&core::com::slot<F>::call, _slot.get()))
{
    this->set_worker(_slot->get_worker());
    this->m_source_slot = _slot;
}

//-----------------------------------------------------------------------------

template<typename F, std::enable_if_t<std::is_function_v<typename core::com::util::convert_function_type<F>::type>,
                                      bool> = true,
         typename ... BINDING>
SPTR(slot<typename core::com::util::convert_function_type<F>::type>) new_slot(F _f, BINDING ... _binding)
{
#ifdef _DEBUG
    constexpr bool has_valid_nb_args = (sizeof...(_binding) < 2);
    SIGHT_ASSERT("Too many arguments", has_valid_nb_args);
#endif
    using function_t = std::function<typename core::com::util::convert_function_type<F>::type>;
    function_t func = core::com::util::autobind(_f, _binding ...);
    return std::make_shared<slot<function_t> >(func);
}

//-----------------------------------------------------------------------------

template<typename F, std::enable_if_t<!std::is_function_v<typename core::com::util::convert_function_type<F>::type>,
                                      bool> = true>
SPTR(slot<core::lambda_to_function_t<F> >) new_slot(F _f)
{
    auto fn = lambda_to_function(_f);
    return std::make_shared<sight::core::com::slot<core::lambda_to_function_t<F> > >(fn);
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
