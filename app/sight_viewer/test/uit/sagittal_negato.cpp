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

using ui::test::at;

SIGHT_GUI_TEST_CASE("sagittal_negato")
{
    open_image(ui);
    const auto alone = ui.read(MAIN_SCENE, &QWidget::size);

    // The camera turns to face this negato.
    ui.expect_image_changes(MAIN_SCENE, [&]{ui.drag(MAIN_SCENE, at(0.01, 0.5), at(0.26, 0.5));});
    ui.click("top_toolbar_left/volume");
    ui.click("top_toolbar_view/Restore");

    reset_negatos(ui);
    ui.set(negato("sagittal_scene_view"), 250);
    ui.expect(negato("sagittal_scene_view", "label")).text_contains("250 / ");
    show_main_scene_alone(ui, alone);
    ui.expect_image(MAIN_SCENE, reference("sagittal_negato"), edge_on_negatos());
}

} // namespace sight::sight_viewer::uit
