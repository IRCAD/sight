/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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

#include "zoom_out.hpp"

#include <ui/test/helper/scene3d.hpp>

#include <utest_data/data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sight_viewer::uit::zoom_out);

namespace sight::sight_viewer::uit
{

//------------------------------------------------------------------------------

void zoom_out::test()
{
    namespace helper = sight::ui::test::helper;

    const std::string test_name  = "sightViewerZoomOutTest";
    const std::string image_name = test_name + ".png";
    const std::filesystem::path snapshot_path(sight::ui::test::tester::get_image_output_path() / image_name);
    if(std::filesystem::exists(snapshot_path))
    {
        std::filesystem::remove(snapshot_path);
    }

    const std::filesystem::path reference_path(utest_data::dir() / "sight/ui/sight_viewer" / image_name);

    start(
        test_name,
        [&snapshot_path, &reference_path](sight::ui::test::tester& _tester)
        {
            open_file(
                _tester,
                "VTK",
                utest_data::dir() / "sight/mesh/vtk/sphere.vtk"
            );

            // Drag the mouse to zoom out
            helper::scene3d::zoom(_tester, "scene_srv", -7);

            save_snapshot(_tester, snapshot_path);

            compare_images(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::sight_viewer::uit
