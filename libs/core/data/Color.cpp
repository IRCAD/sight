/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "data/Color.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>

SIGHT_REGISTER_DATA(sight::data::Color);

namespace sight::data
{

//------------------------------------------------------------------------------

Color::Color(data::Object::Key /*unused*/)
{
    m_vRGBA.fill(1.0);
}

//------------------------------------------------------------------------------

Color::sptr Color::New(
    Color::ColorType red,
    Color::ColorType green,
    Color::ColorType blue,
    Color::ColorType alpha
)
{
    Color::sptr color = data::Color::New();
    color->m_vRGBA = {red, green, blue, alpha};
    return color;
}

//------------------------------------------------------------------------------

Color::~Color()
= default;

//------------------------------------------------------------------------------

void Color::shallowCopy(const Object::csptr& _source)
{
    Color::csptr other = Color::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldShallowCopy(_source);
    m_vRGBA = other->m_vRGBA;
}

//------------------------------------------------------------------------------

void Color::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Color::csptr other = Color::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);
    m_vRGBA = other->m_vRGBA;
}

//------------------------------------------------------------------------------

void Color::setRGBA(const ColorType red, const ColorType green, const ColorType blue, const ColorType alpha)
{
    m_vRGBA = {red, green, blue, alpha};
}

//------------------------------------------------------------------------------

void Color::setRGBA(const std::string& hexaColor)
{
    SIGHT_ASSERT(
        "Color string should start with '#' and followed by 6 or 8 "
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

Color::ColorType& Color::red()
{
    return m_vRGBA[0];
}

//------------------------------------------------------------------------------

Color::ColorType& Color::green()
{
    return m_vRGBA[1];
}

//------------------------------------------------------------------------------

Color::ColorType& Color::blue()
{
    return m_vRGBA[2];
}

//------------------------------------------------------------------------------

Color::ColorType& Color::alpha()
{
    return m_vRGBA[3];
}

//------------------------------------------------------------------------------

const Color::ColorType& Color::red() const
{
    return m_vRGBA[0];
}

//------------------------------------------------------------------------------

const Color::ColorType& Color::green() const
{
    return m_vRGBA[1];
}

//------------------------------------------------------------------------------

const Color::ColorType& Color::blue() const
{
    return m_vRGBA[2];
}

//------------------------------------------------------------------------------

const Color::ColorType& Color::alpha() const
{
    return m_vRGBA[3];
}

//------------------------------------------------------------------------------

bool Color::operator==(const Color& other) const noexcept
{
    // If the attributes are different, then it is not equal
    if(!core::tools::is_equal(m_vRGBA, other.m_vRGBA))
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool Color::operator!=(const Color& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
