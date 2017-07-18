/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_UTIL_CONVERT_FUNCTION_TYPE_HPP__
#define __FWCOM_UTIL_CONVERT_FUNCTION_TYPE_HPP__

#include <boost/type_traits/remove_pointer.hpp>

#include <functional>

namespace fwCom
{

namespace util
{

template <typename F>
struct convert_function_type;

/// Convert class member method type to equivalent function type.
template <typename R, typename C, typename ... Args >
struct convert_function_type< R (C::*)( Args ... )  >
{
    typedef R type ( Args ... );
};

/// Convert class const member method type to equivalent function type.
template <typename R, typename C, typename ... Args >
struct convert_function_type< R (C::*)( Args ... ) const  >
{
    typedef R type ( Args ... );
};

/// Extract function type from a boost function.
template <typename F>
struct convert_function_type< std::function< F > >
{
    typedef F type;
};

/// Convert function pointer type to function type.
template <typename F>
struct convert_function_type
{
    typedef typename ::boost::remove_pointer<F>::type type;
};

} //namespace util

} //namespace fwCom

#endif /* __FWCOM_UTIL_CONVERT_FUNCTION_TYPE_HPP__ */

