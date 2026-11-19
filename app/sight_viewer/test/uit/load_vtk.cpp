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

namespace sight::sight_viewer::uit
{

SIGHT_GUI_TEST_CASE("load_vtk")
{
    open_mesh(ui);
    ui.expect_image(MAIN_SCENE, reference("load_vtk"));

    // The organs are listed in a panel of their own.
    ui.set("parameter_toolbar_view/organs manager", true);
    ui.click("list_organ_editor_srv/hide_all_organs");
    ui.expect_image(MAIN_SCENE, reference("load_vtk_hidden"));

    // Shown again, it is the same as before.
    ui.click("list_organ_editor_srv/hide_all_organs");
    ui.expect_image(MAIN_SCENE, reference("load_vtk"));
}

} // namespace sight::sight_viewer::uit
