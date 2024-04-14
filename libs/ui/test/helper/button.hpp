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

class button
{
public:

    /**
     * Click on the specified button
     * @param _tester The current tester
     * @param _button How to get the button to be clicked
     */
    SIGHT_UI_TEST_API static void push(tester& _tester, const selector& _button);

    /**
     * Click on a button in a menu bar
     * @param _tester The current tester
     * @param _button How to get the menu bar button to be clicked
     */
    SIGHT_UI_TEST_API static void push_menu_bar_item(tester& _tester, const selector& _button);

    /**
     * Wait for a button to be clickable
     * @param _tester The current tester
     * @param _button How to get the button to wait
     */
    SIGHT_UI_TEST_API static void wait_for_clickability(
        tester& _tester,
        const selector& _button
    );
};

} // namespace sight::ui::testCore::helper
