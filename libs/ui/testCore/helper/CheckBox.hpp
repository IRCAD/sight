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

namespace sight::ui::test_core::helper
{

class CheckBox
{
public:

    /**
     * Toggle a check box by clicking on it
     * @param tester The current tester
     * @param checkBox How to get the check box to toggle
     */
    GUITEST_API static void toggle(Tester& _tester, const Select& _check_box);

    /**
     * Check whether a check box is checked
     * @param tester The current tester
     * @param checkBox How to get the check box to verify
     */
    GUITEST_API static void shouldBeChecked(Tester& _tester, const Select& _check_box);

    /**
     * Check whether a check box isn't checked
     * @param tester The current tester
     * @param checkBox How to get the check box to verify
     */
    GUITEST_API static void shouldNotBeChecked(Tester& _tester, const Select& _check_box);
};

} // namespace sight::ui::testCore::helper
