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

#pragma once

#include "select.hpp"

#include <ui/testCore/tester.hpp>

namespace sight::ui::test_core::helper
{

class scene3d
{
public:

    /**
     * Rotates the camera in the specified scene
     * @param tester The current tester
     * @param scene How to get the scene
     * @param rotation the rotation to be applied, in radian divided by pi. (eg to apply an horizontal rotation of PI/2,
     *                 use QPointF(1/2., 0)
     */
    GUITEST_API static void rotate(tester& _tester, const selector& _scene, QPointF _rotation);

    /**
     * Zooms the camera in the specified scene using the mouse wheel
     * @param tester The current tester
     * @param scene How to get the scene
     * @param mouseSteps the number of mouse steps to be applied. A positive value zooms in while a negative value zooms
     *                   out. Don't be mistaken: the value is NOT the scale, for example, mouseSteps=2 will zoom in with
     *                   two mouse wheel steps, not making the image twice as large.
     */
    GUITEST_API static void zoom(tester& _tester, const selector& _scene, int _mouse_steps);
};

} // namespace sight::ui::testCore::helper
