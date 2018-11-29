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

#include "fwData/registry/macros.hpp"
#include "fwData/Exception.hpp"
#include "fwData/Color.hpp"

#include <fwCore/base.hpp>

fwDataRegisterMacro( ::fwData::Color );

namespace fwData
{

//------------------------------------------------------------------------------

Color::Color ( ::fwData::Object::Key key )
{
    m_vRGBA.fill(1.0);
}

//------------------------------------------------------------------------------

Color::sptr Color::ColorFactory(Color::ColorType red, Color::ColorType green, Color::ColorType blue,
                                Color::ColorType alpha)
{
    Color::sptr color = ::fwData::Color::New();
    color->m_vRGBA = {red, green, blue, alpha};
    return color;
}

//------------------------------------------------------------------------------

Color::~Color ()
{
}

//------------------------------------------------------------------------------

void Color::shallowCopy(const Object::csptr &_source )
{
    Color::csptr other = Color::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );
    m_vRGBA = other->m_vRGBA;
}

//------------------------------------------------------------------------------

void Color::cachedDeepCopy(const Object::csptr &_source, DeepCopyCacheType &cache)
{
    Color::csptr other = Color::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( ::fwData::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );
    m_vRGBA = other->m_vRGBA;
}

//------------------------------------------------------------------------------

void Color::setRGBA( const ColorType red, const ColorType green, const ColorType blue, const ColorType alpha )
{
    m_vRGBA = {red, green, blue, alpha};
}

//------------------------------------------------------------------------------

void Color::setRGBA( std::string hexaColor )
{
    OSLM_ASSERT(
        "Color string should start with '#' and followed by 6 ou 8 "
        "hexadecimal digits. Given color : " << hexaColor,
        hexaColor[0] == '#'
        && ( hexaColor.length() == 7 || hexaColor.length() == 9)
        );

    std::string redString   = hexaColor.substr(1, 2);
    std::string greenString = hexaColor.substr(3, 2);
    std::string blueString  = hexaColor.substr(5, 2);
    int r,g,b, a = 255;

    std::istringstream iss;
    iss.str (redString);
    iss >> std::hex >> r;
    iss.clear();
    iss.str (greenString);
    iss >> std::hex >> g;
    iss.clear();
    iss.str (blueString);
    iss >> std::hex >> b;

    if (hexaColor.length() == 9)
    {
        std::string alphaString = hexaColor.substr(7, 2);
        iss.clear();
        iss.str (alphaString);
        iss >> std::hex >> a;
    }

    this->setRGBA(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
}

//------------------------------------------------------------------------------

Color::ColorType  &Color::red()
{
    return m_vRGBA[0];
}

Color::ColorType  &Color::green()
{
    return m_vRGBA[1];
}

Color::ColorType  &Color::blue()
{
    return m_vRGBA[2];
}

Color::ColorType  &Color::alpha()
{
    return m_vRGBA[3];
}

const Color::ColorType  &Color::red() const
{
    return m_vRGBA[0];
}

const Color::ColorType  &Color::green() const
{
    return m_vRGBA[1];
}

const Color::ColorType  &Color::blue() const
{
    return m_vRGBA[2];
}

const Color::ColorType  &Color::alpha() const
{
    return m_vRGBA[3];
}

//------------------------------------------------------------------------------

} // namespace fwData


