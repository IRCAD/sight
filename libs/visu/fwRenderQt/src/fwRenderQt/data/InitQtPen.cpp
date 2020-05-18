/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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
