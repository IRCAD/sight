/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderQt/data/InitQtPen.hpp"

#include <fwCore/base.hpp>

namespace fwRenderQt
{
namespace data
{

//---------------------------------------------------------------------------------------

void InitQtPen::setPenColor(QPen& _pen, std::string _color)
{
    QColor color(QString::fromStdString(_color));
    if (color.isValid())
    {
        _pen.setColor(color);
    }
    else
    {
        // Default value: black (if the color id cannot be parsed)
        _pen.setColor(Qt::GlobalColor(Qt::black));
    }
    _pen.setCosmetic(true);
}

//---------------------------------------------------------------------------------------

void InitQtPen::setPenColor(QPen& _pen, std::string _color, float _opacity)
{
    InitQtPen::setPenColor( _pen, _color );

    QColor color = _pen.color();
    color.setAlphaF( _opacity );
    _pen.setColor( color );
}

} // namespace data
} // namespace fwRenderQt
