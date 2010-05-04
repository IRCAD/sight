/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __NUMERICROUNDCAST_HPP__
#define __NUMERICROUNDCAST_HPP__

#include <cmath>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>


namespace fwTools
{

template < typename TYPEOUT, typename TYPEIN, typename T1, typename T2>
TYPEOUT numericRoundCast (const TYPEIN & value, T1, T2)
{
    return static_cast< TYPEOUT >(value);
}

template < typename TYPEOUT, typename TYPEIN>
TYPEOUT numericRoundCast (const TYPEIN & value, const boost::true_type &in_is_float, const boost::true_type &out_is_int)
{
    return static_cast< TYPEOUT >(std::floor(value + 0.5));
}

template < typename TYPEOUT, typename TYPEIN>
TYPEOUT numericRoundCast (const TYPEIN &value)
{
    return numericRoundCast< TYPEIN, TYPEOUT >(value, ::boost::is_floating_point<TYPEOUT>(), ::boost::is_integral<TYPEIN>());
}

} // namespace fwTools

#endif //__NUMERICROUNDCAST_HPP__
