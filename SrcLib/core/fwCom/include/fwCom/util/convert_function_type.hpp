/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_UTIL_CONVERT_FUNCTION_TYPE_HPP__
#define __FWCOM_UTIL_CONVERT_FUNCTION_TYPE_HPP__

#include <boost/function.hpp>
#include <boost/type_traits/remove_pointer.hpp>


namespace fwCom
{


namespace util
{



/// Convert Class member method type to equivalent function type
template <typename F>
struct convert_function_type;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template <typename R, typename C, typename Args1, typename Args2, typename Args3 >
struct convert_function_type< R (C::*) ( Args1, Args2, Args3 )  >
{
    typedef R type( Args1, Args2, Args3 ) ;
};
template <typename R, typename C, typename Args1, typename Args2 >
struct convert_function_type< R (C::*) ( Args1, Args2 )  >
{
    typedef R type( Args1, Args2 ) ;
};
template <typename R, typename C, typename Args1 >
struct convert_function_type< R (C::*) ( Args1 )  >
{
    typedef R type( Args1 ) ;
};
template <typename R, typename C>
struct convert_function_type< R (C::*) ()  >
{
    typedef R type() ;
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template <typename R, typename C, typename ...Args >
struct convert_function_type< R (C::*) ( Args... )  >
{
    typedef R type( Args... ) ;
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template <typename R, typename C, typename Args1, typename Args2, typename Args3 >
struct convert_function_type< R (C::*) ( Args1, Args2, Args3 ) const  >
{
    typedef R type( Args1, Args2, Args3 ) ;
};
template <typename R, typename C, typename Args1, typename Args2 >
struct convert_function_type< R (C::*) ( Args1, Args2 ) const  >
{
    typedef R type( Args1, Args2 ) ;
};
template <typename R, typename C, typename Args1 >
struct convert_function_type< R (C::*) ( Args1 ) const  >
{
    typedef R type( Args1 ) ;
};
template <typename R, typename C>
struct convert_function_type< R (C::*) () const  >
{
    typedef R type() ;
};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template <typename R, typename C, typename ...Args >
struct convert_function_type< R (C::*) ( Args... ) const  >
{
    typedef R type( Args... ) ;
};
#endif  // BOOST_NO_VARIADIC_TEMPLATES


//extract function type from a boost function
template <typename F>
struct convert_function_type< ::boost::function< F > >
{
    typedef F type;
};

template <typename F>
struct convert_function_type
{
    typedef typename ::boost::remove_pointer<F>::type type;
};




} //namespace util

} //namespace fwCom

#endif /* __FWCOM_UTIL_CONVERT_FUNCTION_TYPE_HPP__ */



