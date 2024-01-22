/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <ui/test/tester.hpp>

namespace sight::ui::test::helper
{

class color_parameter
{
public:

    /**
     * Select a color in a color parameter
     * @param _tester The current tester
     * @param _color_param How to get the color parameter to modify
     * @param _color The color to be selected
     */
    UI_TEST_API static void select(tester& _tester, const selector& _color_param, const QColor& _color);

    /**
     * Check whether the color parameter has the good color
     * @param _tester The current tester
     * @param _color_param How to get the color parameter to check
     * @param _color The color it should have
     */
    UI_TEST_API static void color_equals(tester& _tester, const selector& _color_param, const QColor& _color);
};

} // namespace sight::ui::testCore::helper
