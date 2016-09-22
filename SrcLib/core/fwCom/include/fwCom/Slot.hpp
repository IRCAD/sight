/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_SLOT_HPP__
#define __FWCOM_SLOT_HPP__

#include "fwCom/SlotCall.hpp"

#include <boost/function.hpp>

namespace fwCom
{


template< typename F >
class Slot;

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
class Slot< R ( A ... ) > : public SlotCall< R (A ...) >
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
class Slot< ::boost::function< R ( A ... ) > > : public Slot< R ( A ... ) >
{
public:
    typedef R SignatureType (A ...);
    typedef ::boost::function< SignatureType > FunctionType;


    template< typename FUNCTOR >
    Slot( FUNCTOR f ) : Slot< R ( A ... ) >(),  m_func(f)
    {
    }

    virtual ~Slot()
    {
    }

    virtual void run(A ... a) const
    {
        m_func(a ...);
    }

    virtual R   call(A ... a) const
    {
        return m_func(a ...);
    }

protected:
    FunctionType m_func;
};

//-----------------------------------------------------------------------------

template<typename R, typename ... A >
class Slot< Slot< R ( A ... ) > > : public Slot< ::boost::function < R ( A ... ) > >
{
public:

    typedef R SignatureType ( A ... );
    typedef ::boost::function< SignatureType > FunctionType;

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

#endif /* __FWCOM_SLOT_HPP__ */

