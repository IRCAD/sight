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

#ifndef __FWTOOLS_NUMERICROUNDCAST_HXX__
#define __FWTOOLS_NUMERICROUNDCAST_HXX__

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

#endif //__FWTOOLS_NUMERICROUNDCAST_HXX__
