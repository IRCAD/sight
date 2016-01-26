/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
