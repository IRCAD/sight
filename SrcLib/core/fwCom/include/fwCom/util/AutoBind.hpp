/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_UTIL_AUTOBIND_HPP__
#define __FWCOM_UTIL_AUTOBIND_HPP__

#include "fwCom/util/convert_function_type.hpp"

#include <functional>

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

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );

};

/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 1 >
{
    typedef typename convert_function_type< F >::type FunctionType;

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );
};

/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 2 >
{
    typedef typename convert_function_type< F >::type FunctionType;

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );
};

/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 3 >
{
    typedef typename convert_function_type< F >::type FunctionType;

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );
};

/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 4 >
{
    typedef typename convert_function_type< F >::type FunctionType;

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );
};

/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 5 >
{
    typedef typename convert_function_type< F >::type FunctionType;

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );
};

/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 6 >
{
    typedef typename convert_function_type< F >::type FunctionType;

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );
};

/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 7 >
{
    typedef typename convert_function_type< F >::type FunctionType;

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );
};

/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 8 >
{
    typedef typename convert_function_type< F >::type FunctionType;

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );
};

/// AutoBind specialization.
template< typename F >
struct AutoBind< F, 9 >
{
    typedef typename convert_function_type< F >::type FunctionType;

    template< typename W, typename ... A >
    static std::function< FunctionType > wrap( W f, A ... a );
};

/**
 * @brief Automatic bind of given function
 *
 * @return a boost function of type F if F is a function type, of type *F if F is a function pointer,
 * or of the corresponding function type if F is a member method pointer (like R (C::*)(A1, A2, ..))
 * If F is a member method pointer, autobind takes the object on wich we desire to call the method
 * as second argument.
 */
template <typename F, typename ... A >
std::function< typename convert_function_type< F >::type > autobind(F f, A ... a);

} //namespace util

} //namespace fwCom

#endif /* __FWCOM_UTIL_AUTOBIND_HPP__ */
