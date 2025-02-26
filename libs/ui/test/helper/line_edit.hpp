/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

class line_edit
{
public:

    /**
     * Check whether the line edit exactly matches the expected result.
     * @param _tester The current tester
     * @param _edit How to get the line edit to check
     * @param _expected The expected content of the label
     */
    SIGHT_UI_TEST_API static void match(tester& _tester, const selector& _edit, const std::string& _expected);

    /**
     * Sets the row which has the specified text as current in a line edit widget
     * @param _tester The current tester
     * @param _edit How to get the line edit to set
     * @param _text The text of the new current row
     */
    SIGHT_UI_TEST_API static void set_current_text(tester& _tester, const selector& _edit, const std::string& _text);
};

} // namespace sight::ui::testCore::helper
