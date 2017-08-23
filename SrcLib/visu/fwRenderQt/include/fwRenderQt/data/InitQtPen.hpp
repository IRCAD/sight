/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERQT_DATA_INITQTPEN_HPP__
#define __FWRENDERQT_DATA_INITQTPEN_HPP__

#include "fwRenderQt/config.hpp"

#include <QPen>

namespace fwRenderQt
{
namespace data
{

class FWRENDERQT_CLASS_API InitQtPen
{

public:

    /// Set a pen a color.
    FWRENDERQT_API static void setPenColor(QPen& _pen, std::string _color);

    /// Set a pen a color with opacity.
    FWRENDERQT_API static void setPenColor(QPen& _pen, std::string _color, float _opacity);
};

} // namespace data
} // namespace fwRenderQt

#endif // __FWRENDERQT_DATA_INITQTPEN_HPP__
