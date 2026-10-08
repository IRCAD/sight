/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include <boost/numeric/conversion/converter.hpp>

namespace sight::core::tools
{

/**
 * This method converts an value of type TYPE_IN to an value of type TYPE_OUT.
 * If TYPE_IN is a floating point type and TYPE_OUT is a integral type then
 * it rounds float number to nearest integer (returns 3 for 3.1f, 5 for 4.5f).
 *
 * Be careful: there is not range checking, overflow is not detected (silent overflow policy)
 *
 * @param _value to cast (and rounds if is a floating point type) in TYPE_OUT
 * @return casted and rounded value.
 */
template<typename TYPE_OUT, typename TYPE_IN>
TYPE_OUT numeric_round_cast(const TYPE_IN& _value)
{
    using traits_type           = boost::numeric::conversion_traits<TYPE_OUT, TYPE_IN>;
    using overflow_handler_type = boost::numeric::silent_overflow_handler;
    using round_even_type       = boost::numeric::RoundEven<typename traits_type::source_type>;
    using converter_type        = boost::numeric::raw_converter<traits_type>;
    using range_checker_type    = boost::numeric::UseInternalRangeChecker;

    using round_cast_type = boost::numeric::converter<
        TYPE_OUT,
        TYPE_IN,
        traits_type,
        overflow_handler_type,
        round_even_type,
        converter_type,
        range_checker_type
    >;

    return round_cast_type::convert(_value);
}

} // namespace sight::core::tools
