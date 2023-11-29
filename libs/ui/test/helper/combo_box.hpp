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

#include <ui/test/tester.hpp>

namespace sight::ui::test::helper
{

class combo_box
{
public:

    /**
     * Select an entry in a combo box by text
     * @param tester The current tester
     * @param comboBox How to get the combo box to modify
     * @param text The text of the entry to be selected
     */
    UI_TEST_API static void select(tester& _tester, const selector& _combo_box, const std::string& _text);

    /**
     * Check whether a combo box has the good value
     * @param tester The current tester
     * @param comboBox How to get the combo box to check
     * @param expected The text the combo box must have
     */
    UI_TEST_API static void value_equals(tester& _tester, const selector& _combo_box, const std::string& _expected);
};

} // namespace sight::ui::testCore::helper
