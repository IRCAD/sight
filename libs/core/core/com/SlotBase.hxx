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

#if !defined(__FWCOM_SLOTBASE_HPP__)
#error core/com/SlotBase.hpp not included
#endif

#include "core/com/exception/BadCall.hpp"
#include "core/com/Slot.hpp"
#include "core/com/Slot.hxx"
#include "core/com/SlotRun.hpp"
#include "core/com/SlotRun.hxx"
#include "core/exceptionmacros.hpp"

#include <boost/function_types/function_arity.hpp>

#include <functional>

namespace sight::core::com
{

//------------------------------------------------------------------------------

template<typename A1, typename A2, typename A3>
void SlotBase::run(A1 a1, A2 a2, A3 a3) const
{
    typedef SlotRun<void (A1, A2, A3)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        fun->run(a1, a2, a3);
    }
    else
    {
        SIGHT_INFO(
            "Failed to run the slot with three parameters : "
            << m_signature << " != " << SlotBase::getTypeName<void(A1, A2, A3)>()
            << ". Trying to run the slot with two parameters."
        );
        this->run(a1, a2);
    }
}

//-----------------------------------------------------------------------------

template<typename A1, typename A2>
void SlotBase::run(A1 a1, A2 a2) const
{
    typedef SlotRun<void (A1, A2)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        fun->run(a1, a2);
    }
    else
    {
        SIGHT_INFO(
            "Failed to run the slot with two parameters : "
            << m_signature << " != " << SlotBase::getTypeName<void(A1, A2)>()
            << ". Trying to run the slot with one parameter."
        );
        this->run(a1);
    }
}

//-----------------------------------------------------------------------------

template<typename A1>
void SlotBase::run(A1 a1) const
{
    typedef SlotRun<void (A1)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->run(a1);
    }

    SIGHT_INFO(
        "Failed to run the slot with one parameter : "
        << m_signature << " != " << SlotBase::getTypeName<void(A1)>()
        << ". Trying to run the slot without parameter."
    );
    return this->run();
}

//------------------------------------------------------------------------------

template<typename R, typename A1, typename A2, typename A3>
R SlotBase::call(A1 a1, A2 a2, A3 a3) const
{
    typedef Slot<R(A1, A2, A3)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->call(a1, a2, a3);
    }

    SIGHT_INFO(
        "Failed to call the slot with three parameters : "
        << m_signature << " != " << SlotBase::getTypeName<R(A1, A2, A3)>()
        << ". Trying to call the slot with two parameters."
    );
    return this->call<R>(a1, a2);
}

//-----------------------------------------------------------------------------

template<typename R, typename A1, typename A2>
R SlotBase::call(A1 a1, A2 a2) const
{
    typedef Slot<R(A1, A2)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->call(a1, a2);
    }

    SIGHT_INFO(
        "Failed to call the slot with two parameters : "
        << m_signature << " != " << SlotBase::getTypeName<R(A1, A2)>()
        << ". Trying to call the slot with one parameter."
    );
    return this->call<R>(a1);
}

//-----------------------------------------------------------------------------

template<typename R, typename A1>
R SlotBase::call(A1 a1) const
{
    typedef Slot<R(A1)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->call(a1);
    }

    SIGHT_INFO(
        "Failed to call the slot with one parameter : "
        << m_signature << " != " << SlotBase::getTypeName<R(A1)>()
        << ". Trying to call the slot without parameter."
    );
    return this->call<R>();
}

//-----------------------------------------------------------------------------

template<typename R>
R SlotBase::call() const
{
    typedef Slot<R()> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->call();
    }

    SIGHT_ERROR("Failed to call : " + m_signature + " with " + SlotBase::getTypeName<R()>());
    SIGHT_THROW_EXCEPTION(core::com::exception::BadCall("Failed to find right signature for call"));
}

//------------------------------------------------------------------------------

template<typename A1, typename A2, typename A3>
SlotBase::VoidSharedFutureType SlotBase::asyncRun(A1 a1, A2 a2, A3 a3) const
{
    typedef SlotRun<void (A1, A2, A3)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->asyncRun(a1, a2, a3);
    }

    SIGHT_INFO(
        "Failed to asyncRun the slot with three parameters : "
        << m_signature << " != " << SlotBase::getTypeName<void(A1, A2, A3)>()
        << ". Trying to asyncRun the slot with two parameters."
    );
    return this->asyncRun(a1, a2);
}

//-----------------------------------------------------------------------------

template<typename A1, typename A2>
SlotBase::VoidSharedFutureType SlotBase::asyncRun(A1 a1, A2 a2) const
{
    typedef SlotRun<void (A1, A2)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->asyncRun(a1, a2);
    }

    SIGHT_INFO(
        "Failed to asyncRun the slot with two parameters : "
        << m_signature << " != " << SlotBase::getTypeName<void(A1, A2)>()
        << ". Trying to asyncRun the slot with one parameter."
    );
    return this->asyncRun(a1);
}

//-----------------------------------------------------------------------------

template<typename A1>
SlotBase::VoidSharedFutureType SlotBase::asyncRun(A1 a1) const
{
    typedef SlotRun<void (A1)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->asyncRun(a1);
    }

    SIGHT_INFO(
        "Failed to asyncRun the slot with one parameter : "
        << m_signature << " != " << SlotBase::getTypeName<void(A1)>()
        << ". Trying to asyncRun the slot without parameter."
    );
    return this->asyncRun();
}

//------------------------------------------------------------------------------

template<typename R, typename A1, typename A2, typename A3>
std::shared_future<R> SlotBase::asyncCall(A1 a1, A2 a2, A3 a3) const
{
    typedef Slot<R(A1, A2, A3)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->asyncCall(a1, a2, a3);
    }

    SIGHT_INFO(
        "Failed to asyncCall the slot with three parameters : "
        << m_signature << " != " << SlotBase::getTypeName<R(A1, A2, A3)>()
        << ". Trying to asyncCall the slot with two parameters."
    );
    return this->asyncCall<R>(a1, a2);
}

//-----------------------------------------------------------------------------

template<typename R, typename A1, typename A2>
std::shared_future<R> SlotBase::asyncCall(A1 a1, A2 a2) const
{
    typedef Slot<R(A1, A2)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->asyncCall(a1, a2);
    }

    SIGHT_INFO(
        "Failed to asyncCall the slot with two parameters : "
        << m_signature << " != " << SlotBase::getTypeName<R(A1, A2)>()
        << ". Trying to asyncCall the slot with one parameter."
    );
    return this->asyncCall<R>(a1);
}

//-----------------------------------------------------------------------------

template<typename R, typename A1>
std::shared_future<R> SlotBase::asyncCall(A1 a1) const
{
    typedef Slot<R(A1)> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->asyncCall(a1);
    }

    SIGHT_INFO(
        "Failed to asyncCall the slot with one parameters : "
        << m_signature << " != " << SlotBase::getTypeName<R(A1)>()
        << ". Trying to asyncCall the slot without parameter."
    );
    return this->asyncCall<R>();
}

//-----------------------------------------------------------------------------

template<typename R>
std::shared_future<R> SlotBase::asyncCall() const
{
    typedef Slot<R()> SlotFuncType;
    const auto* fun = dynamic_cast<const SlotFuncType*>(this);
    if(fun)
    {
        return fun->asyncCall();
    }

    SIGHT_ERROR("failed to asyncCall : " + m_signature + " with " + SlotBase::getTypeName<R()>());
    SIGHT_THROW_EXCEPTION(core::com::exception::BadCall("Failed to find right signature for asyncCall"));
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
