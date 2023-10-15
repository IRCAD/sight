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
    color::ColorType red,
    color::ColorType green,
    color::ColorType blue,
    color::ColorType alpha
)
{
    m_vRGBA = {red, green, blue, alpha};
}

//------------------------------------------------------------------------------

void color::shallow_copy(const object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const color>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vRGBA = other->m_vRGBA;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void color::deep_copy(const object::csptr& source, const std::unique_ptr<deep_copy_cache_t>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const color>(source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_vRGBA = other->m_vRGBA;

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

void color::setRGBA(const ColorType red, const ColorType green, const ColorType blue, const ColorType alpha)
{
    m_vRGBA = {red, green, blue, alpha};
}

//------------------------------------------------------------------------------

void color::setRGBA(const std::string& hexaColor)
{
    SIGHT_ASSERT(
        "color string should start with '#' and followed by 6 or 8 "
        "hexadecimal digits. Given color: " << hexaColor,
        hexaColor[0] == '#'
        && (hexaColor.length() == 7 || hexaColor.length() == 9)
    );

    const std::string redString   = hexaColor.substr(1, 2);
    const std::string greenString = hexaColor.substr(3, 2);
    const std::string blueString  = hexaColor.substr(5, 2);
    std::int32_t r                = 0;
    std::int32_t g                = 0;
    std::int32_t b                = 0;
    std::int32_t a                = 255;

    std::istringstream iss;
    iss.str(redString);
    iss >> std::hex >> r;
    iss.clear();
    iss.str(greenString);
    iss >> std::hex >> g;
    iss.clear();
    iss.str(blueString);
    iss >> std::hex >> b;

    if(hexaColor.length() == 9)
    {
        const std::string alphaString = hexaColor.substr(7, 2);
        iss.clear();
        iss.str(alphaString);
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

color::ColorType& color::red()
{
    return m_vRGBA[0];
}

//------------------------------------------------------------------------------

color::ColorType& color::green()
{
    return m_vRGBA[1];
}

//------------------------------------------------------------------------------

color::ColorType& color::blue()
{
    return m_vRGBA[2];
}

//------------------------------------------------------------------------------

color::ColorType& color::alpha()
{
    return m_vRGBA[3];
}

//------------------------------------------------------------------------------

const color::ColorType& color::red() const
{
    return m_vRGBA[0];
}

//------------------------------------------------------------------------------

const color::ColorType& color::green() const
{
    return m_vRGBA[1];
}

//------------------------------------------------------------------------------

const color::ColorType& color::blue() const
{
    return m_vRGBA[2];
}

//------------------------------------------------------------------------------

const color::ColorType& color::alpha() const
{
    return m_vRGBA[3];
}

//------------------------------------------------------------------------------

bool color::operator==(const color& other) const noexcept
{
    // If the attributes are different, then it is not equal
    if(!core::tools::is_equal(m_vRGBA, other.m_vRGBA))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool color::operator!=(const color& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
