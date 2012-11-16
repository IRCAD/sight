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
                >::wrap( &SlotRun< F >::run, slot )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );
}




template<typename R, typename A1, typename A2 >
template< typename F >
Slot< Slot< R ( A1, A2 ) > >::Slot( SPTR( SlotRun< F > ) slot )
    : Slot< FunctionType >(
            ::fwCom::util::AutoBind<
                    SignatureType,
                    ::boost::function_types::function_arity< F >::value
                >::wrap( &SlotRun< F >::run, slot )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );
}




template<typename R, typename A1 >
template< typename F >
Slot< Slot< R ( A1 ) > >::Slot( SPTR( SlotRun< F > ) slot )
    : Slot< FunctionType >(
            ::fwCom::util::AutoBind<
                    SignatureType,
                    ::boost::function_types::function_arity< F >::value
                >::wrap( &SlotRun< F >::run, slot )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );
}




template<typename R>
template< typename F >
Slot< Slot< R () > >::Slot( SPTR( SlotRun< F > ) slot )
    : Slot< FunctionType >(
            ::fwCom::util::AutoBind<
                    SignatureType,
                    ::boost::function_types::function_arity< F >::value
                >::wrap( &SlotRun< F >::run, slot )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );
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
                >::wrap( &SlotRun< F >::run, slot )
                                                        )
{
    BOOST_STATIC_ASSERT( (boost::is_same<void, R>::type::value) );
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
            >::wrap( &Slot< F >::call, slot )
                                                    )
{ }





template<typename R, typename A1, typename A2 >
template< typename F >
Slot< Slot< R ( A1, A2 ) > >::Slot( SPTR( Slot< F > ) slot )
: Slot< FunctionType >(
        ::fwCom::util::AutoBind<
                SignatureType,
                ::boost::function_types::function_arity< F >::value
            >::wrap( &Slot< F >::call, slot )
                                                    )
{ }





template<typename R, typename A1 >
template< typename F >
Slot< Slot< R ( A1 ) > >::Slot( SPTR( Slot< F > ) slot )
: Slot< FunctionType >(
        ::fwCom::util::AutoBind<
                SignatureType,
                ::boost::function_types::function_arity< F >::value
            >::wrap( &Slot< F >::call, slot )
                                                    )
{ }





template<typename R>
template< typename F >
Slot< Slot< R () > >::Slot( SPTR( Slot< F > ) slot )
: Slot< FunctionType >(
        ::fwCom::util::AutoBind<
                SignatureType,
                ::boost::function_types::function_arity< F >::value
            >::wrap( &Slot< F >::call, slot )
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
            >::wrap( &Slot< F >::call, slot )
                                                    )
{ }





#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template<typename F, typename Bindings1, typename Bindings2, typename Bindings3 >
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, Bindings1  bindings1, Bindings2  bindings2, Bindings3  bindings3 )
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT( 3 < 2 );
#else
    SLM_ASSERT( "Too many arguments", ( 3 < 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f, bindings1, bindings2, bindings3 );
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}



template<typename F, typename Bindings1, typename Bindings2 >
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, Bindings1  bindings1, Bindings2  bindings2 )
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT( 2 < 2 );
#else
    SLM_ASSERT( "Too many arguments", ( 2 < 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f, bindings1, bindings2 );
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}



template<typename F, typename Bindings1 >
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, Bindings1  bindings1 )
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT( 1 < 2 );
#else
    SLM_ASSERT( "Too many arguments", ( 1 < 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f, bindings1 );
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}



template<typename F>
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f)
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT(0< 2 );
#else
    SLM_ASSERT( "Too many arguments", (0< 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f);
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template<typename F, typename ...Bindings>
SPTR( Slot< typename ::fwCom::util::convert_function_type< F >::type > ) newSlot(F f, Bindings ...bindings)
{
#ifndef BOOST_NO_VARIADIC_TEMPLATES
    BOOST_STATIC_ASSERT( sizeof...(bindings) < 2 );
#else
    SLM_ASSERT( "Too many arguments", ( sizeof...(bindings) < 2 ) );
#endif
    typedef ::boost::function< typename ::fwCom::util::convert_function_type< F >::type > FunctionType;
    FunctionType func = ::fwCom::util::autobind(f, bindings...);
    return ::boost::make_shared< Slot< FunctionType > > ( func );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES
template<typename F, typename F_IN >
SPTR( Slot< F > ) newSlot( SPTR( SlotRun< F_IN > ) slot )
{
    assert (
        ::boost::function_types::function_arity< F_IN >::value <= ::boost::function_types::function_arity< F >::value
        );
    return boost::make_shared< Slot< Slot< F > > > ( slot );
}

template<typename F, typename F_IN >
SPTR( Slot< F > ) newSlot( SPTR( Slot< F_IN > ) slot )
{
    assert (
        ::boost::function_types::function_arity< F_IN >::value <= ::boost::function_types::function_arity< F >::value
        );
    return ::boost::make_shared< Slot< Slot< F > > > ( slot );
}



} // namespace fwCom

#endif /* __FWCOM_SLOT_HXX__ */


