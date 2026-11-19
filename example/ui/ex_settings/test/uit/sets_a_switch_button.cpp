/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <ui/test/gui_test.hpp>

#include <string>

SIGHT_GUI_TEST_CASE("sets_a_switch_button")
{
    const std::string first  = "properties1_srv/bool_2";
    const std::string second = "properties2_srv/bool_2";

    // Both switches edit the same boolean through independent settings services.
    ui.expect(first).unchecked();
    ui.expect(second).unchecked();
    ui.set(first, true);
    ui.expect(first).checked();
    ui.expect(second).checked();

    // The reverse direction must also reach the shared value.
    ui.set(second, false);
    ui.expect(second).unchecked();
    ui.expect(first).unchecked();

    ui.set("properties1_srv/enable", false);
    ui.expect(first).disabled();
    ui.expect(second).disabled();
    ui.set("properties1_srv/enable", true);
    ui.expect(first).enabled();
    ui.expect(second).enabled();
    ui.expect(first).unchecked();
    ui.expect(second).unchecked();
}
