/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::color);

namespace sight::data
{

//------------------------------------------------------------------------------

color::color()
{
    m_vRGBA.fill(1.0);
}

//------------------------------------------------------------------------------

color::color(
    color::color_t _red,
    color::color_t _green,
    color::color_t _blue,
    color::color_t _alpha
)
{
    m_vRGBA = {_red, _green, _blue, _alpha};
}

//------------------------------------------------------------------------------

void color::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const color>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vRGBA = other->m_vRGBA;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void color::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const color>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vRGBA = other->m_vRGBA;

    base_class::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

void color::setRGBA(const color_t _red, const color_t _green, const color_t _blue, const color_t _alpha)
{
    m_vRGBA = {_red, _green, _blue, _alpha};
}

//------------------------------------------------------------------------------

void color::setRGBA(const std::string& _hexa_color)
{
    SIGHT_ASSERT(
        "color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " << _hexa_color,
        _hexa_color[0] == '#'
        && (_hexa_color.length() == 7 || _hexa_color.length() == 9)
    );

    const std::string red_string   = _hexa_color.substr(1, 2);
    const std::string green_string = _hexa_color.substr(3, 2);
    const std::string blue_string  = _hexa_color.substr(5, 2);
    std::int32_t r                 = 0;
    std::int32_t g                 = 0;
    std::int32_t b                 = 0;
    std::int32_t a                 = 255;

    std::istringstream iss;
    iss.str(red_string);
    iss >> std::hex >> r;
    iss.clear();
    iss.str(green_string);
    iss >> std::hex >> g;
    iss.clear();
    iss.str(blue_string);
    iss >> std::hex >> b;

    if(_hexa_color.length() == 9)
    {
        const std::string alpha_string = _hexa_color.substr(7, 2);
        iss.clear();
        iss.str(alpha_string);
        iss >> std::hex >> a;
    }

    this->setRGBA(
        static_cast<float>(r) / 255.0F,
        static_cast<float>(g) / 255.0F,
        static_cast<float>(b) / 255.0F,
        static_cast<float>(a) / 255.0F
    );
}

//------------------------------------------------------------------------------

color::color_t& color::red()
{
    return m_vRGBA[0];
}

//------------------------------------------------------------------------------

color::color_t& color::green()
{
    return m_vRGBA[1];
}

//------------------------------------------------------------------------------

color::color_t& color::blue()
{
    return m_vRGBA[2];
}

//------------------------------------------------------------------------------

color::color_t& color::alpha()
{
    return m_vRGBA[3];
}

//------------------------------------------------------------------------------

const color::color_t& color::red() const
{
    return m_vRGBA[0];
}

//------------------------------------------------------------------------------

const color::color_t& color::green() const
{
    return m_vRGBA[1];
}

//------------------------------------------------------------------------------

const color::color_t& color::blue() const
{
    return m_vRGBA[2];
}

//------------------------------------------------------------------------------

const color::color_t& color::alpha() const
{
    return m_vRGBA[3];
}

//------------------------------------------------------------------------------

bool color::operator==(const color& _other) const noexcept
{
    // If the attributes are different, then it is not equal
    if(!core::tools::is_equal(m_vRGBA, _other.m_vRGBA))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(_other);
}

//------------------------------------------------------------------------------

bool color::operator!=(const color& _other) const noexcept
{
    return !(*this == _other);
}

} // namespace sight::data
