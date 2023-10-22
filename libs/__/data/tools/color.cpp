/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "data/tools/color.hpp"

#include "core/spy_log.hpp"

#include <core/exceptionmacros.hpp>

#include <sstream>

namespace sight::data::tools
{

//------------------------------------------------------------------------------

void color::hexaStringToRGBA(const std::string& _hexa_color, std::array<std::uint8_t, 4>& _rgba)
{
    SIGHT_THROW_IF(
        "Color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " + _hexa_color,
        _hexa_color[0] != '#'
        || (_hexa_color.length() != 7 && _hexa_color.length() != 9)
    );

    std::string red_string   = _hexa_color.substr(1, 2);
    std::string green_string = _hexa_color.substr(3, 2);
    std::string blue_string  = _hexa_color.substr(5, 2);
    int r                    = 0;
    int g                    = 0;
    int b                    = 0;
    int a                    = 255;

    std::istringstream iss;
    iss.str(red_string);
    iss >> std::hex >> r;
    iss.clear();
    iss.str(green_string);
    iss >> std::hex >> g;
    iss.clear();
    iss.str(blue_string);
    iss >> std::hex >> b;

    _rgba[3] = 255;
    if(_hexa_color.length() == 9)
    {
        std::string alpha_string = _hexa_color.substr(7, 2);
        iss.clear();
        iss.str(alpha_string);
        iss >> std::hex >> a;
    }

    _rgba[0] = static_cast<std::uint8_t>(r);
    _rgba[1] = static_cast<std::uint8_t>(g);
    _rgba[2] = static_cast<std::uint8_t>(b);
    _rgba[3] = static_cast<std::uint8_t>(a);
}

//------------------------------------------------------------------------------

} // end namespace sight::data::tools
