/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <boost/algorithm/string.hpp>

namespace sight::filter::image
{

// Type for image interpolation
enum class interpolation_t : std::uint8_t
{
    NEAREST,
    LINEAR,
    BSPLINE
};

/// Convert a string to an interpolation type
inline interpolation_t string_to_interpolation(const std::string& _str)
{
    const std::string str = boost::algorithm::to_lower_copy(_str);
    if(str == "nearest")
    {
        return interpolation_t::NEAREST;
    }
    else if(str == "linear")
    {
        return interpolation_t::LINEAR;
    }
    else if(str == "bspline")
    {
        return interpolation_t::BSPLINE;
    }
    else
    {
        throw std::runtime_error("Unknown interpolation type: " + str);
    }
}

} // namespace sight::filter::image
