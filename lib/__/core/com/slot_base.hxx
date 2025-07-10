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

#if !defined(FWCOM_SLOTBASE_HPP)
#error core/com/slot_base.hpp not included
#endif

#include "core/com/exception/bad_call.hpp"
#include "core/com/slot.hpp"
#include "core/com/slot.hxx"
#include "core/com/slot_run.hpp"
#include "core/com/slot_run.hxx"
#include "core/exceptionmacros.hpp"

#include <boost/function_types/function_arity.hpp>

#include <functional>

namespace sight::core::com
{

//------------------------------------------------------------------------------

template<typename A1, typename A2, typename A3>
void slot_base::run(A1 _a1, A2 _a2, A3 _a3) const
{
    typedef slot_run<void (A1, A2, A3)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        fun->run(_a1, _a2, _a3);
    }
    else
    {
        SIGHT_INFO(
            "Failed to run the slot with three parameters : "
            << m_signature << " != " << slot_base::get_type_name<void(A1, A2, A3)>()
            << ". Trying to run the slot with two parameters."
        );
        this->run(_a1, _a2);
    }
}

//-----------------------------------------------------------------------------

template<typename A1, typename A2>
void slot_base::run(A1 _a1, A2 _a2) const
{
    typedef slot_run<void (A1, A2)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        fun->run(_a1, _a2);
    }
    else
    {
        SIGHT_INFO(
            "Failed to run the slot with two parameters : "
            << m_signature << " != " << slot_base::get_type_name<void(A1, A2)>()
            << ". Trying to run the slot with one parameter."
        );
        this->run(_a1);
    }
}

//-----------------------------------------------------------------------------

template<typename A1>
void slot_base::run(A1 _a1) const
{
    typedef slot_run<void (A1)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->run(_a1);
    }

    SIGHT_INFO(
        "Failed to run the slot with one parameter : "
        << m_signature << " != " << slot_base::get_type_name<void(A1)>()
        << ". Trying to run the slot without parameter."
    );
    return this->run();
}

//------------------------------------------------------------------------------

template<typename R, typename A1, typename A2, typename A3>
R slot_base::call(A1 _a1, A2 _a2, A3 _a3) const
{
    typedef slot<R(A1, A2, A3)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->call(_a1, _a2, _a3);
    }

    SIGHT_INFO(
        "Failed to call the slot with three parameters : "
        << m_signature << " != " << slot_base::get_type_name<R(A1, A2, A3)>()
        << ". Trying to call the slot with two parameters."
    );
    return this->call<R>(_a1, _a2);
}

//-----------------------------------------------------------------------------

template<typename R, typename A1, typename A2>
R slot_base::call(A1 _a1, A2 _a2) const
{
    typedef slot<R(A1, A2)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->call(_a1, _a2);
    }

    SIGHT_INFO(
        "Failed to call the slot with two parameters : "
        << m_signature << " != " << slot_base::get_type_name<R(A1, A2)>()
        << ". Trying to call the slot with one parameter."
    );
    return this->call<R>(_a1);
}

//-----------------------------------------------------------------------------

template<typename R, typename A1>
R slot_base::call(A1 _a1) const
{
    typedef slot<R(A1)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->call(_a1);
    }

    SIGHT_INFO(
        "Failed to call the slot with one parameter : "
        << m_signature << " != " << slot_base::get_type_name<R(A1)>()
        << ". Trying to call the slot without parameter."
    );
    return this->call<R>();
}

//-----------------------------------------------------------------------------

template<typename R>
R slot_base::call() const
{
    typedef slot<R()> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->call();
    }

    SIGHT_ERROR("Failed to call : " + m_signature + " with " + slot_base::get_type_name<R()>());
    SIGHT_THROW_EXCEPTION(core::com::exception::bad_call("Failed to find right signature for call"));
}

//------------------------------------------------------------------------------

template<typename A1, typename A2, typename A3>
slot_base::void_shared_future_type slot_base::async_run(A1 _a1, A2 _a2, A3 _a3) const
{
    typedef slot_run<void (A1, A2, A3)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->async_run(_a1, _a2, _a3);
    }

    SIGHT_INFO(
        "Failed to async_run the slot with three parameters : "
        << m_signature << " != " << slot_base::get_type_name<void(A1, A2, A3)>()
        << ". Trying to async_run the slot with two parameters."
    );
    return this->async_run(_a1, _a2);
}

//-----------------------------------------------------------------------------

template<typename A1, typename A2>
slot_base::void_shared_future_type slot_base::async_run(A1 _a1, A2 _a2) const
{
    typedef slot_run<void (A1, A2)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->async_run(_a1, _a2);
    }

    SIGHT_INFO(
        "Failed to async_run the slot with two parameters : "
        << m_signature << " != " << slot_base::get_type_name<void(A1, A2)>()
        << ". Trying to async_run the slot with one parameter."
    );
    return this->async_run(_a1);
}

//-----------------------------------------------------------------------------

template<typename A1>
slot_base::void_shared_future_type slot_base::async_run(A1 _a1) const
{
    typedef slot_run<void (A1)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->async_run(_a1);
    }

    SIGHT_INFO(
        "Failed to async_run the slot with one parameter : "
        << m_signature << " != " << slot_base::get_type_name<void(A1)>()
        << ". Trying to async_run the slot without parameter."
    );
    return this->async_run();
}

//------------------------------------------------------------------------------

template<typename R, typename A1, typename A2, typename A3>
std::shared_future<R> slot_base::async_call(A1 _a1, A2 _a2, A3 _a3) const
{
    typedef slot<R(A1, A2, A3)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->async_call(_a1, _a2, _a3);
    }

    SIGHT_INFO(
        "Failed to asyncCall the slot with three parameters : "
        << m_signature << " != " << slot_base::get_type_name<R(A1, A2, A3)>()
        << ". Trying to asyncCall the slot with two parameters."
    );
    return this->async_call<R>(_a1, _a2);
}

//-----------------------------------------------------------------------------

template<typename R, typename A1, typename A2>
std::shared_future<R> slot_base::async_call(A1 _a1, A2 _a2) const
{
    typedef slot<R(A1, A2)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->async_call(_a1, _a2);
    }

    SIGHT_INFO(
        "Failed to asyncCall the slot with two parameters : "
        << m_signature << " != " << slot_base::get_type_name<R(A1, A2)>()
        << ". Trying to asyncCall the slot with one parameter."
    );
    return this->async_call<R>(_a1);
}

//-----------------------------------------------------------------------------

template<typename R, typename A1>
std::shared_future<R> slot_base::async_call(A1 _a1) const
{
    typedef slot<R(A1)> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->async_call(_a1);
    }

    SIGHT_INFO(
        "Failed to asyncCall the slot with one parameters : "
        << m_signature << " != " << slot_base::get_type_name<R(A1)>()
        << ". Trying to asyncCall the slot without parameter."
    );
    return this->async_call<R>();
}

//-----------------------------------------------------------------------------

template<typename R>
std::shared_future<R> slot_base::async_call() const
{
    typedef slot<R()> slot_func_type;
    const auto* fun = dynamic_cast<const slot_func_type*>(this);
    if(fun)
    {
        return fun->async_call();
    }

    SIGHT_ERROR("failed to asyncCall : " + m_signature + " with " + slot_base::get_type_name<R()>());
    SIGHT_THROW_EXCEPTION(core::com::exception::bad_call("Failed to find right signature for asyncCall"));
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
