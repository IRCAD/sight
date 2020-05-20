/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwCom/SlotCall.hpp"

#include <functional>

namespace fwCom
{

template< typename F >
class Slot;

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
class FWCOM_CLASS_API Slot< R( A ... ) > : public SlotCall< R(A ...) >
{
public:
    typedef R SignatureType (A ...);
    typedef Slot< SignatureType > SelfType;
    typedef SPTR ( SelfType ) sptr;
    typedef WPTR ( SelfType ) wptr;

    Slot();

    template< typename F >
    static SPTR( Slot< R(A ...) > ) New( F f );

    template< typename F, typename O >
    static SPTR( Slot< R(A ...) > ) New( F f, O o );
};

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
class FWCOM_CLASS_API Slot< std::function< R( A ... ) > > : public Slot< R( A ... ) >
{
public:
    typedef R SignatureType (A ...);
    typedef std::function< SignatureType > FunctionType;

    template< typename FUNCTOR >
    Slot( FUNCTOR f ) :
        Slot< R( A ... ) >(),
        m_func(f)
    {
    }

    virtual ~Slot()
    {
    }

    //------------------------------------------------------------------------------

    virtual void run(A ... a) const
    {
        m_func(a ...);
    }

    //------------------------------------------------------------------------------

    virtual R   call(A ... a) const
    {
        return m_func(a ...);
    }

protected:
    FunctionType m_func;
};

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
class FWCOM_CLASS_API Slot< Slot< R( A ... ) > > : public Slot< std::function < R( A ... ) > >
{
public:

    typedef R SignatureType ( A ... );
    typedef std::function< SignatureType > FunctionType;

    template< typename F >
    Slot( SPTR( SlotRun< F > )slot );

    template< typename F >
    Slot( SPTR( Slot< F > )slot );

    template< typename F >
    static SPTR( Slot< R(A ...) > ) New( SPTR( SlotRun< F > ) slot );
};

//-----------------------------------------------------------------------------

template<typename F, typename ... Bindings>
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, Bindings ... bindings);

} // namespace fwCom
