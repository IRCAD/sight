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

#include <QPoint>
#include <QRect>
#include <QWidget>
#include <QWindow>

namespace sight::ui::test::detail
{

/// The part of _widget's rectangle that its parents leave visible, in its own coordinates: scroll areas and collapsed
/// sections clip it. Unlike QWidget::visibleRegion(), nothing on top is removed (opaque children such as a scroll
/// area's viewport, siblings, overlays): what covers a widget is the hit test's business, which names the obstacle.
inline QRect visible_rect(const QWidget& _widget)
{
    QRect rect = _widget.rect();
    QPoint offset;
    for(const QWidget* widget = &_widget ; !widget->isWindow() && !rect.isEmpty() ; widget = widget->parentWidget())
    {
        offset += widget->pos();
        rect   &= widget->parentWidget()->rect().translated(-offset);
    }

    return rect;
}

//------------------------------------------------------------------------------

/// Visible as a user sees it: shown, in an exposed window, and not clipped away by its parents.
inline bool is_visible(const QWidget& _widget)
{
    const QWindow* const window = _widget.window()->windowHandle();
    return _widget.isVisible() && window != nullptr && window->isExposed() && !visible_rect(_widget).isEmpty();
}

} // namespace sight::ui::test::detail
