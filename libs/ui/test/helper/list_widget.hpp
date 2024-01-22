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

class list_widget
{
public:

    /**
     * Checks if a list widget has the correct number of elements.
     * @param _tester The current tester
     * @param _list How to get the list widget to check
     * @param _count The number of element the list widget should have
     */
    UI_TEST_API static void count_equals(tester& _tester, const selector& _list, int _count);

    /**
     * Sets the row of the specified index as current in a list widget
     * @param _tester The current tester
     * @param _list How to get the list widget to set
     * @param _index The index of the new current row
     */
    UI_TEST_API static void set_current_row(tester& _tester, const selector& _list, int _index);

    /**
     * Sets the row which has the specified text as current in a list widget
     * @param _tester The current tester
     * @param _list How to get the list widget to set
     * @param _text The text of the new current row
     */
    UI_TEST_API static void set_current_text(tester& _tester, const selector& _list, const std::string& _text);
};

} // namespace sight::ui::testCore::helper
