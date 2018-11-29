/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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
#ifndef __FWCOM_SLOTBASE_HXX__
#define __FWCOM_SLOTBASE_HXX__

#ifndef __FWCOM_SLOTBASE_HPP__
#error fwCom/SlotBase.hpp not included
#endif

#include "fwCom/exception/BadCall.hpp"
#include "fwCom/Slot.hpp"
#include "fwCom/Slot.hxx"
#include "fwCom/SlotRun.hpp"
#include "fwCom/SlotRun.hxx"

#include "fwCore/exceptionmacros.hpp"

#include <boost/function_types/function_arity.hpp>

#include <functional>

namespace fwCom
{

//------------------------------------------------------------------------------

template< typename A1, typename A2, typename A3 >
void SlotBase::run(A1 a1, A2 a2, A3 a3) const
{
    typedef SlotRun< void (A1, A2, A3) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        fun->run(a1, a2, a3);
    }
    else
    {
        OSLM_ERROR( "failed to run : " << m_signature << " with " << SlotBase::getTypeName< void(A1, A2, A3) >() );
        this->run(a1, a2);
    }
}

//-----------------------------------------------------------------------------

template< typename A1, typename A2 >
void SlotBase::run(A1 a1, A2 a2) const
{
    typedef SlotRun< void (A1, A2) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        fun->run(a1, a2);
    }
    else
    {
        OSLM_ERROR(  "failed to run : " << m_signature << " with " << SlotBase::getTypeName< void(A1, A2) >() );
        this->run(a1);
    }
}

//-----------------------------------------------------------------------------

template< typename A1 >
void SlotBase::run(A1 a1) const
{
    typedef SlotRun< void (A1) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->run(a1);
    }
    else
    {
        OSLM_ERROR(  "failed to run : " << m_signature << " with " << SlotBase::getTypeName< void(A1) >() );
        return this->run();
    }
}

//------------------------------------------------------------------------------

template< typename R, typename A1, typename A2, typename A3 >
R SlotBase::call(A1 a1, A2 a2, A3 a3) const
{
    typedef Slot< R(A1, A2, A3) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->call(a1, a2, a3);
    }
    else
    {
        OSLM_ERROR(  "failed to call : " << m_signature << " with " << SlotBase::getTypeName< R(A1, A2, A3) >() );
        return this->call<R>(a1, a2);
    }
}

//-----------------------------------------------------------------------------

template< typename R, typename A1, typename A2 >
R SlotBase::call(A1 a1, A2 a2) const
{
    typedef Slot< R(A1, A2) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->call(a1, a2);
    }
    else
    {
        OSLM_ERROR(  "failed to call : " << m_signature << " with " << SlotBase::getTypeName< R(A1, A2) >() );
        return this->call<R>(a1);
    }
}

//-----------------------------------------------------------------------------

template< typename R, typename A1 >
R SlotBase::call(A1 a1) const
{
    typedef Slot< R(A1) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->call(a1);
    }
    else
    {
        OSLM_ERROR(  "failed to call : " << m_signature << " with " << SlotBase::getTypeName< R(A1) >() );
        return this->call<R>();
    }
}

//-----------------------------------------------------------------------------

template< typename R >
R SlotBase::call() const
{
    typedef Slot< R() > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->call();
    }
    OSLM_ERROR( "failed to call : " + m_signature + " with " + SlotBase::getTypeName< R() >() );
    FW_RAISE_EXCEPTION( ::fwCom::exception::BadCall( "Failed to find right signature for call" ) );
}

//------------------------------------------------------------------------------

template< typename A1, typename A2, typename A3 >
SlotBase::VoidSharedFutureType SlotBase::asyncRun(A1 a1, A2 a2, A3 a3) const
{
    typedef SlotRun< void (A1, A2, A3) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->asyncRun(a1, a2, a3);
    }
    else
    {
        OSLM_ERROR(  "failed to asyncRun : " << m_signature << " with " <<
                     SlotBase::getTypeName< void(A1, A2, A3) >() );
        return this->asyncRun(a1, a2);
    }
}

//-----------------------------------------------------------------------------

template< typename A1, typename A2 >
SlotBase::VoidSharedFutureType SlotBase::asyncRun(A1 a1, A2 a2) const
{
    typedef SlotRun< void (A1, A2) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->asyncRun(a1, a2);
    }
    else
    {
        OSLM_ERROR(  "failed to asyncRun : " << m_signature << " with " << SlotBase::getTypeName< void(A1, A2) >() );
        return this->asyncRun(a1);
    }
}

//-----------------------------------------------------------------------------

template< typename A1 >
SlotBase::VoidSharedFutureType SlotBase::asyncRun(A1 a1) const
{
    typedef SlotRun< void (A1) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->asyncRun(a1);
    }
    else
    {
        OSLM_ERROR(  "failed to asyncRun : " << m_signature << " with " << SlotBase::getTypeName< void(A1) >() );
        return this->asyncRun();
    }
}

//------------------------------------------------------------------------------

template< typename R, typename A1, typename A2, typename A3 >
std::shared_future< R > SlotBase::asyncCall(A1 a1, A2 a2, A3 a3) const
{
    typedef Slot< R(A1, A2, A3) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->asyncCall(a1, a2, a3);
    }
    else
    {
        OSLM_ERROR(  "failed to asyncCall : " << m_signature << " with " << SlotBase::getTypeName< R(A1, A2, A3) >() );
        return this->asyncCall<R>(a1, a2);
    }
}

//-----------------------------------------------------------------------------

template< typename R, typename A1, typename A2 >
std::shared_future< R > SlotBase::asyncCall(A1 a1, A2 a2) const
{
    typedef Slot< R(A1, A2) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->asyncCall(a1, a2);
    }
    else
    {
        OSLM_ERROR(  "failed to asyncCall : " << m_signature << " with " << SlotBase::getTypeName< R(A1, A2) >() );
        return this->asyncCall<R>(a1);
    }
}

//-----------------------------------------------------------------------------

template< typename R, typename A1 >
std::shared_future< R > SlotBase::asyncCall(A1 a1) const
{
    typedef Slot< R(A1) > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->asyncCall(a1);
    }
    else
    {
        OSLM_ERROR(  "failed to asyncCall : " << m_signature << " with " << SlotBase::getTypeName< R(A1) >() );
        return this->asyncCall<R>();
    }
}

//-----------------------------------------------------------------------------

template< typename R >
std::shared_future< R > SlotBase::asyncCall() const
{
    typedef Slot< R() > SlotFuncType;
    const SlotFuncType* fun = dynamic_cast< const SlotFuncType* >(this);
    if(fun)
    {
        return fun->asyncCall();
    }
    else
    {
        OSLM_ERROR( "failed to asyncCall : " + m_signature + " with " + SlotBase::getTypeName< R() >() );
        FW_RAISE_EXCEPTION( ::fwCom::exception::BadCall( "Failed to find right signature for asyncCall" ) );
    }
}

//-----------------------------------------------------------------------------

} // namespace fwCom

#endif /* __FWCOM_SLOTBASE_HXX__ */

