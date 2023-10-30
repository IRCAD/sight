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

#include <ui/testCore/tester.hpp>

namespace sight::ui::test_core::helper
{

class preferences_configuration
{
public:

    /**
     * Fill the preferences configuration window and close it.
     * @param tester The current tester
     * @param values The values to be set
     * @warning This method assumes that the preferences configuration window is already open.
     */
    GUITEST_API static void fill(tester& _tester, const std::map<std::string, std::string>& _values);
};

} // namespace sight::ui::testCore::helper
