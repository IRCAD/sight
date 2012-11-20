/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SLOT_HXX__
#define __FWCOM_SLOT_HXX__

#ifndef __FWCOM_SLOT_HPP__
#error fwCom/Slot.hpp not included
#endif

#include <boost/function_types/function_arity.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/make_shared.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#include "fwCom/util/AutoBind.hpp"
#include "fwCom/util/AutoBind.hxx"

#include "fwCom/SlotCall.hxx"

namespace fwCom
{
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3 >
template< typename F >
Slot< Slot< R ( A1, A2, A3 ) > >::Slot( SPTR( SlotRun< F > ) slot )
    : Slot< FunctionType >(
            ::fwCom::util::AutoBind<
                    SignatureType,
                    ::boost::function_types::function_arity< F >::value
                >::wrap( &SlotRun< F >::run, slot.get() )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );
}




template<typename R, typename A1, typename A2 >
template< typename F >
Slot< Slot< R ( A1, A2 ) > >::Slot( SPTR( SlotRun< F > ) slot )
    : Slot< FunctionType >(
            ::fwCom::util::AutoBind<
                    SignatureType,
                    ::boost::function_types::function_arity< F >::value
                >::wrap( &SlotRun< F >::run, slot.get() )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );
}




template<typename R, typename A1 >
template< typename F >
Slot< Slot< R ( A1 ) > >::Slot( SPTR( SlotRun< F > ) slot )
    : Slot< FunctionType >(
            ::fwCom::util::AutoBind<
                    SignatureType,
                    ::boost::function_types::function_arity< F >::value
                >::wrap( &SlotRun< F >::run, slot.get() )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );
}




template<typename R>
template< typename F >
Slot< Slot< R () > >::Slot( SPTR( SlotRun< F > ) slot )
    : Slot< FunctionType >(
            ::fwCom::util::AutoBind<
                    SignatureType,
                    ::boost::function_types::function_arity< F >::value
                >::wrap( &SlotRun< F >::run, slot.get() )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );
}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A >
template< typename F >
Slot< Slot< R ( A... ) > >::Slot( SPTR( SlotRun< F > ) slot )
    : Slot< FunctionType >(
            ::fwCom::util::AutoBind<
                    SignatureType,
                    ::boost::function_types::function_arity< F >::value
                >::wrap( &SlotRun< F >::run, slot.get() )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::value) );
}




#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3 >
template< typename F >
Slot< Slot< R ( A1, A2, A3 ) > >::Slot( SPTR( Slot< F > ) slot )
: Slot< FunctionType >(
        ::fwCom::util::AutoBind<
                SignatureType,
                ::boost::function_types::function_arity< F >::value
            >::wrap( &Slot< F >::call, slot.get() )
                                                    )
{ }





template<typename R, typename A1, typename A2 >
template< typename F >
Slot< Slot< R ( A1, A2 ) > >::Slot( SPTR( Slot< F > ) slot )
: Slot< FunctionType >(
        ::fwCom::util::AutoBind<
                SignatureType,
                ::boost::function_types::function_arity< F >::value
            >::wrap( &Slot< F >::call, slot.get() )
                                                    )
{ }





template<typename R, typename A1 >
template< typename F >
Slot< Slot< R ( A1 ) > >::Slot( SPTR( Slot< F > ) slot )
: Slot< FunctionType >(
        ::fwCom::util::AutoBind<
                SignatureType,
                ::boost::function_types::function_arity< F >::value
            >::wrap( &Slot< F >::call, slot.get() )
                                                    )
{ }





template<typename R>
template< typename F >
Slot< Slot< R () > >::Slot( SPTR( Slot< F > ) slot )
: Slot< FunctionType >(
        ::fwCom::util::AutoBind<
                SignatureType,
                ::boost::function_types::function_arity< F >::value
            >::wrap( &Slot< F >::call, slot.get() )
                                                    )
{ }





//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A >
template< typename F >
Slot< Slot< R ( A... ) > >::Slot( SPTR( Slot< F > ) slot )
: Slot< FunctionType >(
        ::fwCom::util::AutoBind<
                SignatureType,
                ::boost::function_types::function_arity< F >::value
            >::wrap( &Slot< F >::call, slot.get() )
                                                    )
{ }





#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename F, typename BINDING1, typename BINDING2, typename BINDING3 >
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, BINDING1  binding1, BINDING2  binding2, BINDING3  binding3 )
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT( 3 < 2 );
#else
    SLM_ASSERT( "Too many arguments", ( 3 < 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f, binding1, binding2, binding3 );
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}


template<typename F, typename BINDING1, typename BINDING2 >
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, BINDING1  binding1, BINDING2  binding2 )
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT( 2 < 2 );
#else
    SLM_ASSERT( "Too many arguments", ( 2 < 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f, binding1, binding2 );
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}


template<typename F, typename BINDING1 >
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, BINDING1  binding1 )
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT( 1 < 2 );
#else
    SLM_ASSERT( "Too many arguments", ( 1 < 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f, binding1 );
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}


template<typename F>
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f)
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT( 0 < 2 );
#else
    SLM_ASSERT( "Too many arguments", ( 0 < 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f);
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}


//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename F, typename ...BINDING>
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, BINDING ...binding)
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT( sizeof...(binding) < 2 );
#else
    SLM_ASSERT( "Too many arguments", ( sizeof...(binding) < 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f, binding...);
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}


#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3 >
template<typename F>
SPTR( Slot< R ( A1, A2, A3 ) > ) Slot< R ( A1, A2, A3 ) >::New( F f )
{
    return newSlot(f);
}



template<typename R, typename A1, typename A2 >
template<typename F>
SPTR( Slot< R ( A1, A2 ) > ) Slot< R ( A1, A2 ) >::New( F f )
{
    return newSlot(f);
}



template<typename R, typename A1 >
template<typename F>
SPTR( Slot< R ( A1 ) > ) Slot< R ( A1 ) >::New( F f )
{
    return newSlot(f);
}



template<typename R>
template<typename F>
SPTR( Slot< R () > ) Slot< R () >::New( F f )
{
    return newSlot(f);
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A >
template<typename F>
SPTR( Slot< R ( A... ) > ) Slot< R ( A... ) >::New( F f )
{
    return newSlot(f);
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3 >
template<typename F, typename O>
SPTR( Slot< R ( A1, A2, A3 ) > ) Slot< R ( A1, A2, A3 ) >::New( F f, O o )
{
    return newSlot(f, o);
}



template<typename R, typename A1, typename A2 >
template<typename F, typename O>
SPTR( Slot< R ( A1, A2 ) > ) Slot< R ( A1, A2 ) >::New( F f, O o )
{
    return newSlot(f, o);
}



template<typename R, typename A1 >
template<typename F, typename O>
SPTR( Slot< R ( A1 ) > ) Slot< R ( A1 ) >::New( F f, O o )
{
    return newSlot(f, o);
}



template<typename R>
template<typename F, typename O>
SPTR( Slot< R () > ) Slot< R () >::New( F f, O o )
{
    return newSlot(f, o);
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A >
template<typename F, typename O>
SPTR( Slot< R ( A... ) > ) Slot< R ( A... ) >::New( F f, O o )
{
    return newSlot(f, o);
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename R, typename A1, typename A2, typename A3 >
template<typename F_IN >
SPTR( Slot< R ( A1, A2, A3 ) > ) Slot< Slot< R ( A1, A2, A3 ) > >::New( SPTR( SlotRun< F_IN > ) slot )
{
    assert (
        ::boost::function_types::function_arity< F_IN >::value <= ::boost::function_types::function_arity< R ( A1, A2, A3 ) >::value
        );
    return boost::make_shared< Slot< Slot< R ( A1, A2, A3 ) > > > ( slot );
}




template<typename R, typename A1, typename A2 >
template<typename F_IN >
SPTR( Slot< R ( A1, A2 ) > ) Slot< Slot< R ( A1, A2 ) > >::New( SPTR( SlotRun< F_IN > ) slot )
{
    assert (
        ::boost::function_types::function_arity< F_IN >::value <= ::boost::function_types::function_arity< R ( A1, A2 ) >::value
        );
    return boost::make_shared< Slot< Slot< R ( A1, A2 ) > > > ( slot );
}




template<typename R, typename A1 >
template<typename F_IN >
SPTR( Slot< R ( A1 ) > ) Slot< Slot< R ( A1 ) > >::New( SPTR( SlotRun< F_IN > ) slot )
{
    assert (
        ::boost::function_types::function_arity< F_IN >::value <= ::boost::function_types::function_arity< R ( A1 ) >::value
        );
    return boost::make_shared< Slot< Slot< R ( A1 ) > > > ( slot );
}




template<typename R>
template<typename F_IN >
SPTR( Slot< R () > ) Slot< Slot< R () > >::New( SPTR( SlotRun< F_IN > ) slot )
{
    assert (
        ::boost::function_types::function_arity< F_IN >::value <= ::boost::function_types::function_arity< R () >::value
        );
    return boost::make_shared< Slot< Slot< R () > > > ( slot );
}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename R, typename ...A >
template<typename F_IN >
SPTR( Slot< R ( A... ) > ) Slot< Slot< R ( A... ) > >::New( SPTR( SlotRun< F_IN > ) slot )
{
    assert (
        ::boost::function_types::function_arity< F_IN >::value <= ::boost::function_types::function_arity< R ( A... ) >::value
        );
    return boost::make_shared< Slot< Slot< R ( A... ) > > > ( slot );
}




#endif  // BOOST_NO_VARIADIC_TEMPLATES
} // namespace fwCom

#endif /* __FWCOM_SLOT_HXX__ */


