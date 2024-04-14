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

#include <ui/test/tester.hpp>

namespace sight::ui::test::helper
{

class selector_dialog
{
public:

    /**
     * Select an item from a selection dialog and close it
     * @param _tester The current tester
     * @param _selection The item to be selected
     * @warning This method assumes that the selection dialog is already open.
     */
    SIGHT_UI_TEST_API static void select(tester& _tester, const std::string& _selection);
};

} // namespace sight::ui::testCore::helper
