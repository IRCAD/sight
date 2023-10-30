/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "zoom_out_gesture.hpp"

#include <utest_data/Data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::zoom_out_gesture);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void zoom_out_gesture::test()
{
    const std::string test_name  = "sightViewerZoomOutGestureTest";
    const std::string image_name = test_name + ".png";
    const std::filesystem::path snapshot_path(sight::ui::test_core::tester::get_image_output_path() / image_name);
    if(std::filesystem::exists(snapshot_path))
    {
        std::filesystem::remove(snapshot_path);
    }

    // This test should give the same result as ZoomOut
    const std::filesystem::path reference_path(utest_data::dir()
                                               / "sight/ui/SightViewer/sightViewerZoomOutTest.png");

    start(
        test_name,
        [&snapshot_path, &reference_path](sight::ui::test_core::tester& _tester)
        {
            open_file(
                _tester,
                "VTK",
                utest_data::dir() / "sight/mesh/vtk/sphere.vtk"
            );

            /* Pinch the fingers to zoom out */
            // Firstly, we need the 3D scene
            _tester.take("ogre scene", "sceneSrv");
            // We need to a reference to it for later
            auto* ogre_scene = _tester.get<QWidget*>();
            _tester.interact(
                // Pinch fingers...
                std::make_unique<sight::ui::test_core::pinch_gesture>(
                    // ...with the first finger going to the center from 300 pixels higher...
                    std::pair(ogre_scene->rect().center() + QPoint(0, 300), ogre_scene->rect().center() + QPoint(0, 1)),
                    // ...and the second finger going to the center from 300 pixels lower
                    std::pair(ogre_scene->rect().center() - QPoint(0, 300), ogre_scene->rect().center() - QPoint(0, 1))
                )
            );

            save_snapshot(_tester, snapshot_path);

            compare_images(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
