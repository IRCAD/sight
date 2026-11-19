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

#include <doctest/doctest.h>

#include <QImage>

namespace sight::sight_viewer::uit
{

//------------------------------------------------------------------------------

/// How many pixels the mesh covers: it is lit white, on a dark background with a faint grid.
static qsizetype mesh_area(const QImage& _image)
{
    qsizetype area = 0;
    for(int y = 0 ; y < _image.height() ; ++y)
    {
        for(int x = 0 ; x < _image.width() ; ++x)
        {
            area += qGray(_image.pixel(x, y)) > 128 ? 1 : 0;
        }
    }

    return area;
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("zoom_out_gesture")
{
    ui.expect_image_changes(MAIN_SCENE, [&]{open_mesh(ui);});
    const auto before = mesh_area(ui.image(MAIN_SCENE));
    REQUIRE(before > 0);

    // How much a pinch zooms depends on how the recognizer coalesces its updates: only the direction is specified.
    ui.expect_image_changes(MAIN_SCENE, [&]{ui.pinch(MAIN_SCENE, 0.3);});
    REQUIRE(static_cast<double>(mesh_area(ui.image(MAIN_SCENE))) < 0.9 * static_cast<double>(before));
}

} // namespace sight::sight_viewer::uit
