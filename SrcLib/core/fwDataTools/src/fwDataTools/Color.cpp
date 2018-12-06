/************************************************************************
 *
 * Copyright (C) 2014-2015 IRCAD France
 * Copyright (C) 2014-2015 IHU Strasbourg
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

#include "fwDataTools/Color.hpp"

#include <fwCore/exceptionmacros.hpp>
#include "fwCore/spyLog.hpp"

#include <sstream>

namespace fwDataTools
{

//------------------------------------------------------------------------------

void Color::hexaStringToRGBA( const std::string& _hexaColor, std::uint8_t _rgba[4] )
{
    FW_RAISE_IF(
        "Color string should start with '#' and followed by 6 ou 8 "
        "hexadecimal digits. Given color : " + _hexaColor,
        _hexaColor[0] != '#'
        || ( _hexaColor.length() != 7 && _hexaColor.length() != 9)
        );

    std::string redString   = _hexaColor.substr(1, 2);
    std::string greenString = _hexaColor.substr(3, 2);
    std::string blueString  = _hexaColor.substr(5, 2);
    int r,g,b,a = 255;

    std::istringstream iss;
    iss.str (redString);
    iss >> std::hex >> r;
    iss.clear();
    iss.str (greenString);
    iss >> std::hex >> g;
    iss.clear();
    iss.str (blueString);
    iss >> std::hex >> b;

    _rgba[3] = 255;
    if (_hexaColor.length() == 9)
    {
        std::string alphaString = _hexaColor.substr(7, 2);
        iss.clear();
        iss.str (alphaString);
        iss >> std::hex >> a;
    }

    _rgba[0] = static_cast< std::uint8_t >(r);
    _rgba[1] = static_cast< std::uint8_t >(g);
    _rgba[2] = static_cast< std::uint8_t >(b);
    _rgba[3] = static_cast< std::uint8_t >(a);
}

//------------------------------------------------------------------------------

} // end namespace fwDataTools
