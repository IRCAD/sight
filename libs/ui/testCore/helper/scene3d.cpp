/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "scene3d.hpp"

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void scene3d::rotate(tester& _tester, const selector& _scene, QPointF _rotation)
{
    auto bt = _tester.add_in_backtrace(
        "rotating \"" + _scene.get_description(_tester) + "\" scene by (" + std::to_string(_rotation.x())
        + ", " + std::to_string(_rotation.y()) + ')'
    );
    _scene.select(_tester);
    const QWidget* s = _tester.get<QWidget*>();
    int rotation_x   = _rotation.x() == 0 ? 0 : static_cast<int>(s->width() * _rotation.x());
    int rotation_y   = _rotation.y() == 0 ? 0 : static_cast<int>(s->height() * _rotation.y());
    _tester.interact(
        std::make_unique<mouse_drag>(
            tester::left_of(s),
            tester::left_of(s) + QPoint(rotation_x, rotation_y)
        )
    );
}

//------------------------------------------------------------------------------

void scene3d::zoom(tester& _tester, const selector& _scene, int _mouse_steps)
{
    auto bt = _tester.add_in_backtrace(
        "zooming \"" + _scene.get_description(_tester) + "\" scene by " + std::to_string(_mouse_steps) + " mouse steps"
    );
    _scene.select(_tester);
    _tester.interact(
        std::make_unique<mouse_wheel>(QPoint(0, static_cast<int>(_mouse_steps * 120 / qApp->devicePixelRatio())))
    );
}

} // namespace sight::ui::test_core::helper
