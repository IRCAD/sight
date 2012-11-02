/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_NUMERICROUNDCAST_HPP__
#define __FWTOOLS_NUMERICROUNDCAST_HPP__

#include <boost/numeric/conversion/converter.hpp>

namespace fwTools
{

/**
 * This method converts an value of type TYPEIN to an value of type TYPEOUT.
 * If TYPEIN is a floating point type and TYPEOUT is a integral type then
 * it rounds float number to nearest integer (returns 3 for 3.1f, 5 for 4.5f).
 *
 * Be careful: there is not range checking, overflow is not detected (silent overflow policy)
 *
 * @param value to cast (and rounds if is a floating point type) in TYPEOUT
 * @return casted and rounded value.
 */
template < typename TYPEOUT, typename TYPEIN>
TYPEOUT numericRoundCast (const TYPEIN &value)
{
    typedef ::boost::numeric::conversion_traits<TYPEOUT, TYPEIN> TraitsType;
    typedef ::boost::numeric::silent_overflow_handler OverflowHandlerType;
    typedef ::boost::numeric::RoundEven<typename TraitsType::source_type> RoundEvenType;
    typedef ::boost::numeric::raw_converter<TraitsType> ConverterType;
    typedef ::boost::numeric::UseInternalRangeChecker RangeCheckerType;

    typedef ::boost::numeric::converter<
            TYPEOUT,
            TYPEIN,
            TraitsType,
            OverflowHandlerType,
            RoundEvenType,
            ConverterType,
            RangeCheckerType
            > RoundCastType;

    return RoundCastType::convert(value);
}

} // namespace fwTools

#endif //__FWTOOLS_NUMERICROUNDCAST_HPP__
