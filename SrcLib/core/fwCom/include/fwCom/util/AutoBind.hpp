/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_UTIL_AUTOBIND_HPP__
#define __FWCOM_UTIL_AUTOBIND_HPP__

#include <boost/function.hpp>

#include "fwCom/util/convert_function_type.hpp"


/**
 * @brief fwCom
 */
namespace fwCom
{


    /**
     * @brief util
     */
namespace util
{


/**
 * @class AutoBind
 * @brief Automatic binding class.
 * This class allow to bind automatically a function/method with the right number of arguments.
 *
 * @tparam F Desired binding signature.
 * @tparam PLACEHOLDERS_NB number of placeholders to use.
 */
template< typename F, int PLACEHOLDERS_NB >
struct AutoBind;


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 0 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES


};


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 1 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

};


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 2 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

};


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 3 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

};


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 4 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

};


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 5 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

};


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 6 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

};


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 7 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

};


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 8 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

};


/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 9 >
{
    typedef typename convert_function_type< F >::type FunctionType;
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename W, typename A1, typename A2, typename A3, typename A4, typename A5 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template< typename W, typename A1, typename A2, typename A3, typename A4 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template< typename W, typename A1, typename A2, typename A3 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2, A3 a3 ); //{};
template< typename W, typename A1, typename A2 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1, A2 a2 ); //{};
template< typename W, typename A1 >
    static ::boost::function< FunctionType > wrap( W f, A1 a1 ); //{};
template< typename W>
    static ::boost::function< FunctionType > wrap( W f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename W, typename ...A >
    static ::boost::function< FunctionType > wrap( W f, A...a ); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES

};


/**
 * @brief Automatic bind of given function
 *
 * @return a boost function of type F if F is a function type, of type *F if F is a function pointer,
 * or of the corresponding function type if F is a member method pointer (like R (C::*)(A1, A2, ..))
 * If F is a member method pointer, autobind takes the object on wich we desire to call the method
 * as second argument.
 */
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template <typename F, typename A1, typename A2, typename A3, typename A4, typename A5 >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 ); //{};
template <typename F, typename A1, typename A2, typename A3, typename A4 >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A1 a1, A2 a2, A3 a3, A4 a4 ); //{};
template <typename F, typename A1, typename A2, typename A3 >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A1 a1, A2 a2, A3 a3 ); //{};
template <typename F, typename A1, typename A2 >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A1 a1, A2 a2 ); //{};
template <typename F, typename A1 >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A1 a1 ); //{};
template <typename F>
::boost::function< typename convert_function_type< F >::type > autobind(F f); //{};
//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template <typename F, typename ...A >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A...a); //{};
#endif  // BOOST_NO_VARIADIC_TEMPLATES




} //namespace util

} //namespace fwCom

#endif /* __FWCOM_UTIL_AUTOBIND_HPP__ */
