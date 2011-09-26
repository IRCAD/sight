/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "scene2D/data/InitQtPen.hpp"

namespace scene2D
{
namespace data
{

//---------------------------------------------------------------------------------------

void InitQtPen::setPenColor(QPen & _pen, std::string _color)
{
    // If the color id correspond, set the pen the qt corresponding color
    if (_color == "black")
    {
        _pen.setColor(Qt::GlobalColor(Qt::black));
    }
    else if (_color == "blue")
    {
        _pen.setColor(Qt::GlobalColor(Qt::blue));
    }
    else if (_color == "darkGray")
    {
        _pen.setColor(Qt::GlobalColor(Qt::darkGray));
    }
    else if (_color == "gray")
    {
        _pen.setColor(Qt::GlobalColor(Qt::gray));
    }
    else if (_color == "yellow")
    {
        _pen.setColor(Qt::GlobalColor(Qt::yellow));
    }
    else if (_color == "cyan")
    {
        _pen.setColor(Qt::GlobalColor(Qt::cyan));
    }
    else if (_color == "green")
    {
        _pen.setColor(Qt::GlobalColor(Qt::green));
    }
    else if (_color == "lightGray")
    {
        _pen.setColor(Qt::GlobalColor(Qt::lightGray));
    }
    else if (_color == "red")
    {
        _pen.setColor(Qt::GlobalColor(Qt::red));
    }
    else if (_color == "white")
    {
        _pen.setColor(Qt::GlobalColor(Qt::white));
    }
    else if (_color == "transparent" )
    {
        _pen.setColor( Qt::transparent );
    }
    else
    {
        // Default value: black (if the color id isn't recency)
        _pen.setColor(Qt::GlobalColor(Qt::black));
    }
}

//---------------------------------------------------------------------------------------

void InitQtPen::setPenColor(QPen & _pen, std::string _color, float _opacity)
{
    InitQtPen::setPenColor( _pen, _color );

    QColor color = _pen.color();
    color.setAlphaF( _opacity );
    _pen.setColor( color );
}

} // namespace data
} // namespace scene2D
