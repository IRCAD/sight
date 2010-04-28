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

template < typename IN, typename OUT, typename T1, typename T2>
OUT numericRoundCast (IN value, T1, T2)
{
    return static_cast< OUT >(value);
}

template < typename IN, typename OUT>
OUT numericRoundCast (IN value, const boost::true_type &in_is_float, const boost::true_type &out_is_int)
{
    return static_cast< OUT >(std::floor(value + 0.5));
}

template < typename IN, typename OUT>
OUT numericRoundCast (IN &value)
{
    return numericRoundCast< IN, OUT >(value, ::boost::is_floating_point<OUT>(), ::boost::is_integral<IN>());
}

} // namespace fwTools

#endif //__NUMERICROUNDCAST_HPP__
