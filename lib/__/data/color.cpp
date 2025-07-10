/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/color.hpp"

#include "data/registry/macros.hpp"

#include <data/tools/color.hpp>

#include <regex>

SIGHT_REGISTER_DATA(sight::data::color);

namespace sight::data
{

//------------------------------------------------------------------------------

color::color()
{
    this->value().fill(1.0);
}

//------------------------------------------------------------------------------

color::color(
    color::color_t _red,
    color::color_t _green,
    color::color_t _blue,
    color::color_t _alpha
) :
    sight::data::vec<float, 4>({_red, _green, _blue, _alpha})
{
}

//------------------------------------------------------------------------------

color::color(const std::string& _string_color)
{
    from_string(_string_color);
}

//------------------------------------------------------------------------------

void color::set_rgba(const color_t _red, const color_t _green, const color_t _blue, const color_t _alpha)
{
    this->value() = {_red, _green, _blue, _alpha};
}

//------------------------------------------------------------------------------

void color::set_rgba(
    const std::uint8_t _red,
    const std::uint8_t _green,
    const std::uint8_t _blue,
    const std::uint8_t _alpha
)
{
    this->value() = {
        static_cast<float>(_red) / 255.0F, static_cast<float>(_green) / 255.0F,
        static_cast<float>(_blue) / 255.0F, static_cast<float>(_alpha) / 255.0F
    };
}

//------------------------------------------------------------------------------

std::string color::to_string() const
{
    std::ostringstream oss;
    oss << "#" << std::hex << std::setfill('0')
    << std::setw(2) << static_cast<int>(this->red() * 255)
    << std::setw(2) << static_cast<int>(this->green() * 255)
    << std::setw(2) << static_cast<int>(this->blue() * 255)
    << std::setw(2) << static_cast<int>(this->alpha() * 255);
    return boost::to_upper_copy(oss.str());
}

//------------------------------------------------------------------------------

void color::from_string(const std::string& _value)
{
    std::regex re("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$");
    std::smatch match;
    if(std::regex_match(_value, match, re))
    {
        sight::color_t color;
        data::tools::color::hexa_string_to_rgba(_value, color);
        this->set_rgba(color[0], color[1], color[2], color[3]);
        return;
    }

    sight::data::vec<color_t, 4>::from_string(_value);
}

//------------------------------------------------------------------------------

void color::set_rgba(const std::string& _value)
{
    from_string(_value);
}

} // namespace sight::data
