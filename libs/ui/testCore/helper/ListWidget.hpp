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

class ListWidget
{
public:

    /**
     * Checks if a list widget has the correct number of elements.
     * @param tester The current tester
     * @param list How to get the list widget to check
     * @param count The number of element the list widget should have
     */
    GUITEST_API static void countEquals(Tester& tester, const Select& list, int count);

    /**
     * Sets the row of the specified index as current in a list widget
     * @param tester The current tester
     * @param list How to get the list widget to set
     * @param index The index of the new current row
     */
    GUITEST_API static void setCurrentRow(Tester& tester, const Select& list, int index);

    /**
     * Sets the row which has the specified text as current in a list widget
     * @param tester The current tester
     * @param list How to get the list widget to set
     * @param text The text of the new current row
     */
    GUITEST_API static void setCurrentText(Tester& tester, const Select& list, const std::string& text);
};

} // namespace sight::ui::testCore::helper
