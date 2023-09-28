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

#define __FWCOM_SLOT_HPP__

#include "core/com/slot_call.hpp"
#include "core/function.hpp"

#include <functional>
#include <utility>

namespace sight::core::com
{

template<typename F>
class slot;

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
class CORE_CLASS_API slot<R(A ...)>: public slot_call<R(A ...)>
{
public:

    typedef R signature_type(A ...);
    typedef slot<signature_type> self_type;
    typedef SPTR(self_type) sptr;
    typedef WPTR(self_type) wptr;

    slot();
};

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
class CORE_CLASS_API slot<std::function<R(A ...)> >: public slot<R(A ...)>
{
public:

    typedef R signature_type(A ...);
    using function_t = std::function<signature_type>;

    template<typename FUNCTOR>
    slot(FUNCTOR f) :
        slot<R(A ...)>(),
        m_func(std::move(f))
    {
    }

    ~slot() override
    = default;

    //------------------------------------------------------------------------------

    void run(A ... a) const override
    {
        m_func(a ...);
    }

    //------------------------------------------------------------------------------

    R call(A ... a) const override
    {
        return m_func(a ...);
    }

protected:

    function_t m_func;
};

//-----------------------------------------------------------------------------

template<typename R, typename ... A>
class CORE_CLASS_API slot<slot<R(A ...)> >: public slot<std::function<R(A ...)> >
{
public:

    typedef R signature_type(A ...);
    using function_t = std::function<signature_type>;

    template<typename F> slot(SPTR(slot_run<F>)slot);

    template<typename F> slot(SPTR(slot<F>)slot);
};

//-----------------------------------------------------------------------------

template<typename F, typename, typename ... bindings>
SPTR(slot<typename core::com::util::convert_function_type<F>::type>) new_slot(F _f, bindings ... _bindings);

//-----------------------------------------------------------------------------

// Prototype used for lambdas functions
template<typename F, typename>
SPTR(slot<core::lambda_to_function_t<F> >) new_slot(F _f);

//-----------------------------------------------------------------------------

} // namespace sight::core::com
