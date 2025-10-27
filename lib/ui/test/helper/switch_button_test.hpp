/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

class switch_button_test
{
public:

    /**
     * Toggle a switch button by clicking on it
     * @param _tester The current tester
     * @param _switch_button How to get the switch_button to toggle
     */
    SIGHT_UI_TEST_API static void toggle(tester& _tester, const selector& _switch_button);

    /**
     * Check whether a switch button  is checked
     * @param _tester The current tester
     * @param _switch_button How to get the switch button to verify
     */
    SIGHT_UI_TEST_API static void should_be_checked(tester& _tester, const selector& _switch_button);

    /**
     * Check whether a switch button isn't checked
     * @param _tester The current tester
     * @param _switch_button How to get the switch button to verify
     */
    SIGHT_UI_TEST_API static void should_not_be_checked(tester& _tester, const selector& _switch_button);
};

} // namespace sight::ui::testCore::helper
