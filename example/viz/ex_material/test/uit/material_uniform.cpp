/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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
#include <ui/test/image.hpp>

#include <utest_data/data.hpp>

#include <string>

SIGHT_GUI_TEST_CASE("material_uniform")
{
    const std::string scene = "scene_srv/view";

    // The reader loads the model in the background: the scene tells when it shows it.
    ui.click("toolbar/Load a model series");
    ui.expect_image_changes(
        scene,
        [&]
    {
        ui.choose_file(sight::utest_data::dir() / "sight/mesh/vtk/sphere.vtk", "VTK Legacy Files(.vtk) (*.vtk)");
    });
    // Mesa, on the Linux CI images, draws the ghost material up to 32 levels away from the reference, which NVIDIA
    // matches. Another state, without the model or with another material, differs by hundreds.
    ui.expect_image(
        scene,
        sight::utest_data::dir() / "sight/ui/ex_material/material_uniform.png",
        sight::ui::test::threshold(40, 0.001, "Mesa draws the ghost material up to 32 levels away from the reference")
    );
}
