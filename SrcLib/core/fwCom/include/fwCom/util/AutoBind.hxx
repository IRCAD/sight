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
#ifndef __FWCOM_UTIL_AUTOBIND_HXX__
#define __FWCOM_UTIL_AUTOBIND_HXX__

#ifndef __FWCOM_UTIL_AUTOBIND_HPP__
#error fwCom/util/AutoBind.hpp not included
#endif

#include <boost/function_types/function_arity.hpp>

namespace fwCom
{

namespace stdplh = std::placeholders;

namespace util
{

//------------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 0 >::FunctionType > AutoBind< F, 0 >::wrap( W f, A ... a )
{
    return std::bind( f, a ... );
}

//-----------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 1 >::FunctionType > AutoBind< F, 1 >::wrap( W f, A ... a )
{
    return std::bind( f, a ..., stdplh::_1 );
}

//-----------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 2 >::FunctionType > AutoBind< F, 2 >::wrap( W f, A ... a )
{
    return std::bind( f, a ..., stdplh::_1, stdplh::_2 );
}

//-----------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 3 >::FunctionType > AutoBind< F, 3 >::wrap( W f, A ... a )
{
    return std::bind( f, a ..., stdplh::_1, stdplh::_2, stdplh::_3 );
}

//-----------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 4 >::FunctionType > AutoBind< F, 4 >::wrap( W f, A ... a )
{
    return std::bind( f, a ..., stdplh::_1, stdplh::_2, stdplh::_3, stdplh::_4 );
}

//-----------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 5 >::FunctionType > AutoBind< F, 5 >::wrap( W f, A ... a )
{
    return std::bind( f, a ..., stdplh::_1, stdplh::_2, stdplh::_3, stdplh::_4, stdplh::_5 );
}

//-----------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 6 >::FunctionType > AutoBind< F, 6 >::wrap( W f, A ... a )
{
    return std::bind( f, a ..., stdplh::_1, stdplh::_2, stdplh::_3, stdplh::_4, stdplh::_5, stdplh::_6 );
}

//-----------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 7 >::FunctionType > AutoBind< F, 7 >::wrap( W f, A ... a )
{
    return std::bind( f, a ..., stdplh::_1, stdplh::_2, stdplh::_3, stdplh::_4, stdplh::_5, stdplh::_6,
                      stdplh::_7 );
}

//-----------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 8 >::FunctionType > AutoBind< F, 8 >::wrap( W f, A ... a )
{
    return std::bind( f, a ..., stdplh::_1, stdplh::_2, stdplh::_3, stdplh::_4, stdplh::_5, stdplh::_6, stdplh::_7,
                      stdplh::_8 );
}

//-----------------------------------------------------------------------------

template< typename F >
template< typename W, typename ... A >
std::function< typename AutoBind< F, 9 >::FunctionType > AutoBind< F, 9 >::wrap( W f, A ... a )
{
    return std::bind( f, a ..., stdplh::_1, stdplh::_2, stdplh::_3, stdplh::_4, stdplh::_5, stdplh::_6, stdplh::_7,
                      stdplh::_8, stdplh::_9 );
}

//-----------------------------------------------------------------------------

template <typename F, typename ... A >
std::function< typename convert_function_type< F >::type > autobind(F f, A ... a)
{
    typedef typename convert_function_type< F >::type FunctionType;
    const int arity = ::boost::function_types::function_arity< FunctionType >::value;
    return AutoBind< F, arity >::wrap(f, a ...);
}

} //namespace util

} //namespace fwCom

#endif /* __FWCOM_UTIL_AUTOBIND_HXX__ */

