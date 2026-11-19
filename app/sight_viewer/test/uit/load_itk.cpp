/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include "helper.hpp"

#include <ui/test/gui_test.hpp>

#include <chrono>

namespace sight::sight_viewer::uit
{

SIGHT_GUI_TEST_CASE("load_itk")
{
    // Loading adds slice controls and resizes the scene, so only the volume is a same-size change. Each capture of the
    // volume renders it on the GUI thread: with Mesa in a loaded Debug run, the stable frames take tens of seconds.
    open_image(ui);
    ui.expect_image_changes(MAIN_SCENE, [&]{ui.click("top_toolbar_left/volume");}, {}, std::chrono::seconds(180));
    ui.expect_image(MAIN_SCENE, reference("load_itk"));
}

} // namespace sight::sight_viewer::uit
