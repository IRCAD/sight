/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_UTIL_AUTOBIND_HXX__
#define __FWCOM_UTIL_AUTOBIND_HXX__

#ifndef __FWCOM_UTIL_AUTOBIND_HPP__
#error fwCom/util/AutoBind.hpp not included
#endif

#include <boost/bind.hpp>
#include <boost/function_types/function_arity.hpp>


namespace fwCom
{


namespace util
{
#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 0 >::FunctionType > AutoBind< F, 0 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 0 >::FunctionType > AutoBind< F, 0 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 0 >::FunctionType > AutoBind< F, 0 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 0 >::FunctionType > AutoBind< F, 0 >::wrap( W f)
{
    return ::boost::bind( f);
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 0 >::FunctionType > AutoBind< F, 0 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a... );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 1 >::FunctionType > AutoBind< F, 1 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3, _1 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 1 >::FunctionType > AutoBind< F, 1 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2, _1 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 1 >::FunctionType > AutoBind< F, 1 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1, _1 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 1 >::FunctionType > AutoBind< F, 1 >::wrap( W f)
{
    return ::boost::bind( f, _1 );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 1 >::FunctionType > AutoBind< F, 1 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a..., _1 );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 2 >::FunctionType > AutoBind< F, 2 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3, _1, _2 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 2 >::FunctionType > AutoBind< F, 2 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2, _1, _2 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 2 >::FunctionType > AutoBind< F, 2 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1, _1, _2 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 2 >::FunctionType > AutoBind< F, 2 >::wrap( W f)
{
    return ::boost::bind( f, _1, _2 );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 2 >::FunctionType > AutoBind< F, 2 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a..., _1, _2 );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 3 >::FunctionType > AutoBind< F, 3 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3, _1, _2, _3 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 3 >::FunctionType > AutoBind< F, 3 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2, _1, _2, _3 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 3 >::FunctionType > AutoBind< F, 3 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1, _1, _2, _3 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 3 >::FunctionType > AutoBind< F, 3 >::wrap( W f)
{
    return ::boost::bind( f, _1, _2, _3 );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 3 >::FunctionType > AutoBind< F, 3 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a..., _1, _2, _3 );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 4 >::FunctionType > AutoBind< F, 4 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3, _1, _2, _3, _4 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 4 >::FunctionType > AutoBind< F, 4 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2, _1, _2, _3, _4 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 4 >::FunctionType > AutoBind< F, 4 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1, _1, _2, _3, _4 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 4 >::FunctionType > AutoBind< F, 4 >::wrap( W f)
{
    return ::boost::bind( f, _1, _2, _3, _4 );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 4 >::FunctionType > AutoBind< F, 4 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a..., _1, _2, _3, _4 );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 5 >::FunctionType > AutoBind< F, 5 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3, _1, _2, _3, _4, _5 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 5 >::FunctionType > AutoBind< F, 5 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2, _1, _2, _3, _4, _5 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 5 >::FunctionType > AutoBind< F, 5 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1, _1, _2, _3, _4, _5 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 5 >::FunctionType > AutoBind< F, 5 >::wrap( W f)
{
    return ::boost::bind( f, _1, _2, _3, _4, _5 );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 5 >::FunctionType > AutoBind< F, 5 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a..., _1, _2, _3, _4, _5 );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 6 >::FunctionType > AutoBind< F, 6 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3, _1, _2, _3, _4, _5, _6 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 6 >::FunctionType > AutoBind< F, 6 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2, _1, _2, _3, _4, _5, _6 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 6 >::FunctionType > AutoBind< F, 6 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1, _1, _2, _3, _4, _5, _6 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 6 >::FunctionType > AutoBind< F, 6 >::wrap( W f)
{
    return ::boost::bind( f, _1, _2, _3, _4, _5, _6 );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 6 >::FunctionType > AutoBind< F, 6 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a..., _1, _2, _3, _4, _5, _6 );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 7 >::FunctionType > AutoBind< F, 7 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3, _1, _2, _3, _4, _5, _6, _7 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 7 >::FunctionType > AutoBind< F, 7 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2, _1, _2, _3, _4, _5, _6, _7 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 7 >::FunctionType > AutoBind< F, 7 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1, _1, _2, _3, _4, _5, _6, _7 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 7 >::FunctionType > AutoBind< F, 7 >::wrap( W f)
{
    return ::boost::bind( f, _1, _2, _3, _4, _5, _6, _7 );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 7 >::FunctionType > AutoBind< F, 7 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a..., _1, _2, _3, _4, _5, _6, _7 );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 8 >::FunctionType > AutoBind< F, 8 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3, _1, _2, _3, _4, _5, _6, _7, _8 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 8 >::FunctionType > AutoBind< F, 8 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2, _1, _2, _3, _4, _5, _6, _7, _8 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 8 >::FunctionType > AutoBind< F, 8 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1, _1, _2, _3, _4, _5, _6, _7, _8 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 8 >::FunctionType > AutoBind< F, 8 >::wrap( W f)
{
    return ::boost::bind( f, _1, _2, _3, _4, _5, _6, _7, _8 );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 8 >::FunctionType > AutoBind< F, 8 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a..., _1, _2, _3, _4, _5, _6, _7, _8 );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template< typename F >
template< typename W, typename A1, typename A2, typename A3 >
::boost::function< typename AutoBind< F, 9 >::FunctionType > AutoBind< F, 9 >::wrap( W f, A1 a1, A2 a2, A3 a3 )
{
    return ::boost::bind( f, a1, a2, a3, _1, _2, _3, _4, _5, _6, _7, _8, _9 );
}



template< typename F >
template< typename W, typename A1, typename A2 >
::boost::function< typename AutoBind< F, 9 >::FunctionType > AutoBind< F, 9 >::wrap( W f, A1 a1, A2 a2 )
{
    return ::boost::bind( f, a1, a2, _1, _2, _3, _4, _5, _6, _7, _8, _9 );
}



template< typename F >
template< typename W, typename A1 >
::boost::function< typename AutoBind< F, 9 >::FunctionType > AutoBind< F, 9 >::wrap( W f, A1 a1 )
{
    return ::boost::bind( f, a1, _1, _2, _3, _4, _5, _6, _7, _8, _9 );
}



template< typename F >
template< typename W>
::boost::function< typename AutoBind< F, 9 >::FunctionType > AutoBind< F, 9 >::wrap( W f)
{
    return ::boost::bind( f, _1, _2, _3, _4, _5, _6, _7, _8, _9 );
}



//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template< typename F >
template< typename W, typename ...A >
::boost::function< typename AutoBind< F, 9 >::FunctionType > AutoBind< F, 9 >::wrap( W f, A...a )
{
    return ::boost::bind( f, a..., _1, _2, _3, _4, _5, _6, _7, _8, _9 );
}



#endif  // BOOST_NO_VARIADIC_TEMPLATES

#ifdef BOOST_NO_VARIADIC_TEMPLATES
//===============================================================================
//===============================================================================
//==================================== BEGIN ====================================
template <typename F, typename A1, typename A2, typename A3 >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A1 a1, A2 a2, A3 a3 )
{
    typedef typename convert_function_type< F >::type FunctionType;
    const int arity = ::boost::function_types::function_arity< FunctionType >::value;
    return AutoBind< F, arity >::wrap(f, a1, a2, a3 );
}




template <typename F, typename A1, typename A2 >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A1 a1, A2 a2 )
{
    typedef typename convert_function_type< F >::type FunctionType;
    const int arity = ::boost::function_types::function_arity< FunctionType >::value;
    return AutoBind< F, arity >::wrap(f, a1, a2 );
}




template <typename F, typename A1 >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A1 a1 )
{
    typedef typename convert_function_type< F >::type FunctionType;
    const int arity = ::boost::function_types::function_arity< FunctionType >::value;
    return AutoBind< F, arity >::wrap(f, a1 );
}




template <typename F>
::boost::function< typename convert_function_type< F >::type > autobind(F f)
{
    typedef typename convert_function_type< F >::type FunctionType;
    const int arity = ::boost::function_types::function_arity< FunctionType >::value;
    return AutoBind< F, arity >::wrap(f);
}




//===================================== END =====================================
//===============================================================================
//===============================================================================

#else  // BOOST_NO_VARIADIC_TEMPLATES
template <typename F, typename ...A >
::boost::function< typename convert_function_type< F >::type > autobind(F f, A...a)
{
    typedef typename convert_function_type< F >::type FunctionType;
    const int arity = ::boost::function_types::function_arity< FunctionType >::value;
    return AutoBind< F, arity >::wrap(f, a...);
}




#endif  // BOOST_NO_VARIADIC_TEMPLATES
} //namespace util

} //namespace fwCom

#endif /* __FWCOM_UTIL_AUTOBIND_HXX__ */

