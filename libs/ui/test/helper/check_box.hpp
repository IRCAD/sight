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

class check_box
{
public:

    /**
     * Toggle a check box by clicking on it
     * @param _tester The current tester
     * @param _check_box How to get the check box to toggle
     */
    UI_TEST_API static void toggle(tester& _tester, const selector& _check_box);

    /**
     * Check whether a check box is checked
     * @param _tester The current tester
     * @param _check_box How to get the check box to verify
     */
    UI_TEST_API static void should_be_checked(tester& _tester, const selector& _check_box);

    /**
     * Check whether a check box isn't checked
     * @param _tester The current tester
     * @param _check_box How to get the check box to verify
     */
    UI_TEST_API static void should_not_be_checked(tester& _tester, const selector& _check_box);
};

} // namespace sight::ui::testCore::helper
