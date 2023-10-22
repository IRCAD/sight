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

class SpinBox
{
public:

    /**
     * Increment a spin box
     * @param tester The current tester
     * @param spinBox How to get the spin box to increment
     * @param times Number of times to increment
     * @note This method assumes that the spin box uses a style where the buttons are at the left and right.
     */
    GUITEST_API static void increment(Tester& _tester, const Select& _spin_box, int _times = 1);

    /**
     * Decrement a spin box
     * @param tester The current tester
     * @param spinBox How to get the spin box to decrement
     * @param times Number of times to decrement
     * @note This method assumes that the spin box uses a style where the buttons are at the left and right.
     */
    GUITEST_API static void decrement(Tester& _tester, const Select& _spin_box, int _times = 1);

    /**
     * Check whether a spin box has the good value
     * @param tester The current tester
     * @param spinBox How to get the spin box to check
     * @param expected The value the spin box should have
     */
    GUITEST_API static void valueEquals(Tester& _tester, const Select& _spin_box, const std::string& _expected);

    /**
     * Check whether a spin box has the good value
     * @param tester The current tester
     * @param spinBox How to get the spin box to check
     * @param expected The value the spin box should have
     * @note This overload isn't recommanded for double spin boxes, use the string overload instead.
     */
    GUITEST_API static void valueEquals(Tester& _tester, const Select& _spin_box, int _expected);
};

} // namespace sight::ui::testCore::helper
