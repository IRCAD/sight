/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
#ifndef __FWCOM_UTIL_REMOVE_LAST_ARG_HPP__
#define __FWCOM_UTIL_REMOVE_LAST_ARG_HPP__

namespace fwCom
{


namespace util
{

/**
 * @class remove_last_arg
 * @brief Last argument removal class.
 * Removes the last argument type from a function type. If the given function type has no argument,
 * remove_last_arg is the identity.
 *
 * @tparam F Funtion type whereof to remove last argument
 */
template < typename F >
struct remove_last_arg;


/// remove_last_arg specialization.
template < typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
           typename A8, typename A9 >
struct remove_last_arg< R(A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{
    typedef R type (A1, A2, A3, A4, A5, A6, A7, A8);
};


/// remove_last_arg specialization.
template < typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7,
           typename A8 >
struct remove_last_arg< R(A1, A2, A3, A4, A5, A6, A7, A8) >
{
    typedef R type (A1, A2, A3, A4, A5, A6, A7);
};


/// remove_last_arg specialization.
template < typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct remove_last_arg< R(A1, A2, A3, A4, A5, A6, A7) >
{
    typedef R type (A1, A2, A3, A4, A5, A6);
};


/// remove_last_arg specialization.
template < typename R, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct remove_last_arg< R(A1, A2, A3, A4, A5, A6) >
{
    typedef R type (A1, A2, A3, A4, A5);
};


/// remove_last_arg specialization.
template < typename R, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct remove_last_arg< R(A1, A2, A3, A4, A5) >
{
    typedef R type (A1, A2, A3, A4);
};


/// remove_last_arg specialization.
template < typename R, typename A1, typename A2, typename A3, typename A4 >
struct remove_last_arg< R(A1, A2, A3, A4) >
{
    typedef R type (A1, A2, A3);
};


/// remove_last_arg specialization.
template < typename R, typename A1, typename A2, typename A3 >
struct remove_last_arg< R(A1, A2, A3) >
{
    typedef R type (A1, A2);
};

/// remove_last_arg specialization.
template < typename R, typename A1, typename A2 >
struct remove_last_arg< R(A1, A2) >
{
    typedef R type (A1);
};

/// remove_last_arg specialization.
template < typename R, typename A1 >
struct remove_last_arg< R(A1) >
{
    typedef R type ();
};


/// remove_last_arg specialization.
template < typename R >
struct remove_last_arg< R() >
{
    typedef R type ();
};


} //namespace util

} //namespace fwCom

#endif /* __FWCOM_UTIL_REMOVE_LAST_ARG_HPP__ */


