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

#include "Scene3d.hpp"

namespace sight::ui::test_core::helper
{

//------------------------------------------------------------------------------

void Scene3d::rotate(Tester& _tester, const Select& _scene, QPointF _rotation)
{
    auto bt = _tester.addInBacktrace(
        "rotating \"" + _scene.getDescription(_tester) + "\" scene by (" + std::to_string(_rotation.x())
        + ", " + std::to_string(_rotation.y()) + ')'
    );
    _scene.select(_tester);
    const QWidget* s = _tester.get<QWidget*>();
    int rotation_x   = _rotation.x() == 0 ? 0 : static_cast<int>(s->width() * _rotation.x());
    int rotation_y   = _rotation.y() == 0 ? 0 : static_cast<int>(s->height() * _rotation.y());
    _tester.interact(
        std::make_unique<MouseDrag>(
            Tester::leftOf(s),
            Tester::leftOf(s) + QPoint(rotation_x, rotation_y)
        )
    );
}

//------------------------------------------------------------------------------

void Scene3d::zoom(Tester& _tester, const Select& _scene, int _mouse_steps)
{
    auto bt = _tester.addInBacktrace(
        "zooming \"" + _scene.getDescription(_tester) + "\" scene by " + std::to_string(_mouse_steps) + " mouse steps"
    );
    _scene.select(_tester);
    _tester.interact(
        std::make_unique<MouseWheel>(QPoint(0, static_cast<int>(_mouse_steps * 120 / qApp->devicePixelRatio())))
    );
}

} // namespace sight::ui::test_core::helper
