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

#include "Select.hpp"

#include <ui/testCore/Tester.hpp>

namespace sight::ui::testCore::helper
{

class Button
{
public:

    /**
     * Click on the specified button
     * @param tester The current tester
     * @param button How to get the button to be clicked
     */
    GUITEST_API static void push(Tester& tester, const Select& button);

    /**
     * Click on a button in a menu bar
     * @param tester The current tester
     * @param button How to get the menu bar button to be clicked
     */
    GUITEST_API static void pushMenuBarItem(Tester& tester, const Select& button);

    /**
     * Wait for a button to be clickable
     * @param tester The current tester
     * @param button How to get the button to wait
     */
    GUITEST_API static void waitForClickability(
        Tester& tester,
        const Select& button
    );
};

} // namespace sight::ui::testCore::helper
