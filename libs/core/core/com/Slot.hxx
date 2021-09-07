/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#if !defined(__FWCOM_SLOT_HPP__)
#error core/com/Slot.hpp not included
#endif

#include "core/com/SlotCall.hxx"
#include "core/com/util/AutoBind.hpp"
#include "core/com/util/AutoBind.hxx"

#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/static_assert.hpp>

#include <type_traits>

namespace sight::core::com
{

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
Slot< R( A ... ) >::Slot() :
    SlotCall< R(A ...) >()
{
    // 'this->' is needed by gcc 4.2
    this->SlotBase::m_signature = SlotBase::getTypeName< R( A ... ) >();
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
template<typename F>
SPTR( Slot< R( A ... ) > ) Slot< R( A ... ) >::New( F f )
{
    return newSlot(f);
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
template<typename F, typename O>
SPTR( Slot< R( A ... ) > ) Slot< R( A ... ) >::New( F f, O o )
{
    return newSlot(f, o);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<typename R, typename ... A >
template< typename F >
Slot< Slot< R( A ... ) > >::Slot( SPTR( SlotRun< F > )slot ) :
    Slot< FunctionType >(
        core::com::util::AutoBind<
            SignatureType,
            ::boost::function_types::function_arity< F >::value
            >::wrap( &SlotRun< F >::run, slot.get() ) )
{
    BOOST_STATIC_ASSERT( (std::is_same<void, R>::value) );
    this->setWorker(slot->getWorker());
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
template< typename F >
Slot< Slot< R( A ... ) > >::Slot( SPTR( Slot< F > )slot ) :
    Slot< FunctionType >(
        core::com::util::AutoBind<
            SignatureType,
            ::boost::function_types::function_arity< F >::value
            >::wrap( &Slot< F >::call, slot.get() ) )
{
    this->setWorker(slot->getWorker());
}

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
template<typename F >
SPTR(Slot< R( A ...  )>) Slot< Slot< R( A ... )> >::New( SPTR( SlotRun< F > ) slot )
{
    assert(::boost::function_types::function_arity< F >::value <=
           ::boost::function_types::function_arity< R( A ... ) >::value);
    return std::make_shared< Slot< Slot< R( A ... ) > > >( slot );
}

//-----------------------------------------------------------------------------

template<typename F, typename ... BINDING>
SPTR(Slot< typename core::com::util::convert_function_type< F >::type >) newSlot(F f, BINDING ... binding)
{
#ifdef _DEBUG
    constexpr bool hasValidNbArgs = (sizeof ... (binding) < 2);
    SIGHT_ASSERT( "Too many arguments", hasValidNbArgs );
#endif
    typedef std::function< typename core::com::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = core::com::util::autobind(f, binding ...);
    return std::make_shared< Slot< FunctionType > > ( func );
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
