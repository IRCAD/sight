/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_DATA_INITQTPEN_HPP__
#define __SCENE2D_DATA_INITQTPEN_HPP__

#include <QPen>

#include "scene2D/config.hpp"

namespace scene2D
{
namespace data
{

class SCENE2D_CLASS_API InitQtPen
{

public:

    /// Set a pen a color.
    static SCENE2D_API void setPenColor(QPen & _pen, std::string _color);

    /// Set a pen a color with opacity.
    static SCENE2D_API void setPenColor(QPen & _pen, std::string _color, float _opacity);
};

} // namespace data
} // namespace scene2D

#endif // __SCENE2D_DATA_INITQTPEN_HPP__
