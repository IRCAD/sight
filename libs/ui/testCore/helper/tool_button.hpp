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

#include <ui/testCore/tester.hpp>

namespace sight::ui::test_core::helper
{

class tool_button
{
public:

    /**
     * Check whether the tooltip of a tool_button exactly matches the expected result.
     * @param tester The current tester
     * @param label How to get the label to check
     * @param expected The expected content of the tooltip
     */
    GUITEST_API static void tool_tip_matches(tester& _tester, const selector& _label, const std::string& _expected);
};

} // namespace sight::ui::testCore::helper
