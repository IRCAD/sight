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

#include <ui/testCore/Tester.hpp>

namespace sight::ui::test_core::helper
{

class selector_dialog
{
public:

    /**
     * Select an item from a selection dialog and close it
     * @param tester The current tester
     * @param selection The item to be selected
     * @warning This method assumes that the selection dialog is already open.
     */
    GUITEST_API static void select(Tester& _tester, const std::string& _selection);
};

} // namespace sight::ui::testCore::helper
