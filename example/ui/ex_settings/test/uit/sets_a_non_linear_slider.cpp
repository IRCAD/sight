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

SIGHT_GUI_TEST_CASE("sets_a_non_linear_slider")
{
    // The combo slider of the scrollable panel lies below its fold; both panels edit the same "nonlin" entry.
    const std::string slider = "properties1_srv/nonlin";
    ui.scroll_to(slider);
    ui.expect(slider + "/min_label").text("25");
    ui.expect(slider + "/max_label").text("150");
    ui.expect(slider + "/value_label").text("90");

    // By keyboard, as a user moves it over its values.
    ui.set(slider, 120);
    ui.expect(slider + "/value_label").text("120");
    ui.expect("properties2_srv/nonlin").text("120");
}
