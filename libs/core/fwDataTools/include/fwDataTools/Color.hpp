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

#ifndef __FWDATATOOLS_COLOR_HPP__
#define __FWDATATOOLS_COLOR_HPP__

#include "fwDataTools/config.hpp"

#include <string>
#include <cstdint>

namespace fwDataTools
{

/**
 * @brief   This class contains helper to manipulate colors.
 * @class   Color
 */
class Color
{
public:

    /**
     * @brief Convert a color coded as an hexadecimal string into an array of four bytes (RGBA)
     * @param _hexaColor[in] Color string that start with '#' and followed by 6 ou 8 hexadecimal digits (ex: #FF81EC)
     * @param _rgba[out] RGBA values
     */
    FWDATATOOLS_API static void hexaStringToRGBA( const std::string& _hexaColor, std::uint8_t _rgba[4] );
};

} // end namespace fwDataTools

#endif // __FWDATATOOLS_COLOR_HPP__
