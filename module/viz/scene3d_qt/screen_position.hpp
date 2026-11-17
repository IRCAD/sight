/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#pragma once

#include <OGRE/OgreVector.h>

#include <QWidget>

#include <algorithm>
#include <cmath>
#include <utility>

// cspell: ignore QWIDGETSIZE_MAX

namespace sight::module::viz::scene3d_qt
{

/// Projected screen rect (top left, bottom right), as returned by viz::scene3d::helper::scene::compute_bounding_rect().
using screen_rect_t = std::pair<Ogre::Vector2, Ogre::Vector2>;

/// Returns false if the rect can't be projected, i.e. its node lies on or behind the camera plane.
inline bool is_projectable(const screen_rect_t& _rect)
{
    return std::isfinite(_rect.first.x) && std::isfinite(_rect.first.y)
           && std::isfinite(_rect.second.x) && std::isfinite(_rect.second.y);
}

/// Converts a screen coordinate to a widget one, clamped so that Qt geometry arithmetic can't overflow on far
/// off-screen projections. NaN gives 0.
inline int to_widget_coord(qreal _value)
{
    static constexpr auto s_MAX_COORD = static_cast<qreal>(QWIDGETSIZE_MAX);
    return std::isnan(_value) ? 0 : static_cast<int>(std::clamp(_value, -s_MAX_COORD, s_MAX_COORD));
}

/// Moves a popup widget next to a projected rect, in its parent coordinates: left edge on the rect center, above the
/// rect or below it if there isn't enough room, horizontally kept inside the parent.
/// @return false if the rect can't be projected, the widget is then left untouched.
inline bool place_near(QWidget& _widget, const QWidget& _parent, const screen_rect_t& _rect)
{
    if(!is_projectable(_rect))
    {
        return false;
    }

    const qreal ratio = _widget.devicePixelRatioF();
    const int x       = std::clamp(
        to_widget_coord((_rect.first.x + _rect.second.x) / 2 / ratio),
        0,
        std::max(0, _parent.width() - _widget.width())
    );

    int y = to_widget_coord(_rect.first.y / ratio - _widget.height());
    if(y < 0)
    {
        y = to_widget_coord(_rect.second.y / ratio);
    }

    _widget.move(x, y);
    return true;
}

} // namespace sight::module::viz::scene3d_qt
