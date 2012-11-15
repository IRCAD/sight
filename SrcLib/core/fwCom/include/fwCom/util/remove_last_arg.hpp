/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_UTIL_REMOVE_LAST_ARG_HPP__
#define __FWCOM_UTIL_REMOVE_LAST_ARG_HPP__

namespace fwCom
{


namespace util
{


template < typename F > struct remove_last_arg;


template < typename R, typename A1, typename A2, typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 , typename A9 > struct remove_last_arg< R(A1, A2, A3, A4, A5, A6, A7, A8, A9) >
{
    typedef R type(A1, A2, A3, A4, A5, A6, A7, A8);
};


template < typename R, typename A1, typename A2, typename A3 , typename A4 , typename A5 , typename A6 , typename A7 , typename A8 > struct remove_last_arg< R(A1, A2, A3, A4, A5, A6, A7, A8) >
{
    typedef R type(A1, A2, A3, A4, A5, A6, A7);
};


template < typename R, typename A1, typename A2, typename A3 , typename A4 , typename A5 , typename A6 , typename A7 > struct remove_last_arg< R(A1, A2, A3, A4, A5, A6, A7) >
{
    typedef R type(A1, A2, A3, A4, A5, A6);
};


template < typename R, typename A1, typename A2, typename A3 , typename A4 , typename A5 , typename A6 > struct remove_last_arg< R(A1, A2, A3, A4, A5, A6) >
{
    typedef R type(A1, A2, A3, A4, A5);
};


template < typename R, typename A1, typename A2, typename A3 , typename A4 , typename A5 > struct remove_last_arg< R(A1, A2, A3, A4, A5) >
{
    typedef R type(A1, A2, A3, A4);
};


template < typename R, typename A1, typename A2, typename A3 , typename A4 > struct remove_last_arg< R(A1, A2, A3, A4) >
{
    typedef R type(A1, A2, A3);
};


template < typename R, typename A1, typename A2, typename A3 > struct remove_last_arg< R(A1, A2, A3) >
{
    typedef R type(A1, A2);
};

template < typename R, typename A1, typename A2 > struct remove_last_arg< R(A1, A2) >
{
    typedef R type(A1);
};

template < typename R, typename A1 > struct remove_last_arg< R(A1) >
{
    typedef R type();
};


template < typename R > struct remove_last_arg< R() >
{
    typedef R type();
};


} //namespace util

} //namespace fwCom

#endif /* __FWCOM_UTIL_REMOVE_LAST_ARG_HPP__ */


