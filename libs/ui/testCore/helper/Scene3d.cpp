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

namespace sight::ui::testCore::helper
{

//------------------------------------------------------------------------------

void Scene3d::rotate(Tester& tester, const Select& scene, QPointF rotation)
{
    auto bt = tester.addInBacktrace(
        "rotating \"" + scene.getDescription(tester) + "\" scene by (" + std::to_string(rotation.x())
        + ", " + std::to_string(rotation.y()) + ')'
    );
    scene.select(tester);
    const QWidget* s = tester.get<QWidget*>();
    int rotationX    = rotation.x() == 0 ? 0 : static_cast<int>(s->width() * rotation.x());
    int rotationY    = rotation.y() == 0 ? 0 : static_cast<int>(s->height() * rotation.y());
    tester.interact(
        std::make_unique<MouseDrag>(
            Tester::leftOf(s),
            Tester::leftOf(s) + QPoint(rotationX, rotationY)
        )
    );
}

//------------------------------------------------------------------------------

void Scene3d::zoom(Tester& tester, const Select& scene, int mouseSteps)
{
    auto bt = tester.addInBacktrace(
        "zooming \"" + scene.getDescription(tester) + "\" scene by " + std::to_string(mouseSteps) + " mouse steps"
    );
    scene.select(tester);
    tester.interact(
        std::make_unique<MouseWheel>(QPoint(0, static_cast<int>(mouseSteps * 120 / qApp->devicePixelRatio())))
    );
}

} // namespace sight::ui::testCore::helper
