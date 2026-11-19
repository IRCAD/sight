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

SIGHT_GUI_TEST_CASE("axial_negato")
{
    const auto label = negato("axial_scene_view", "label");

    open_image(ui);
    const auto alone = ui.read(MAIN_SCENE, &QWidget::size);
    ui.click("top_toolbar_left/volume");
    ui.click("top_toolbar_view/Restore");
    ui.expect(label).text("67 / 133");

    reset_negatos(ui);
    ui.expect(label).text("0 / 133");

    ui.set(negato("axial_scene_view"), 133);
    ui.expect(label).text("133 / 133");
    show_main_scene_alone(ui, alone);
    ui.expect_image(MAIN_SCENE, reference("axial_negato"));
    ui.click("top_toolbar_view/Restore");

    // The label also tells the position, in millimetres.
    ui.click(negato("axial_scene_view", "label/toggle"));
    ui.expect(label).text("S : 212.80 mm");

    ui.set(negato("axial_scene_view"), 12);
    ui.expect(label).text("S : 19.20 mm");
}

} // namespace sight::sight_viewer::uit
