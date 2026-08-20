/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include "add_distance.hpp"

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/scene3d.hpp>

#include <utest_data/data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sight_viewer::uit::add_distance);

namespace sight::sight_viewer::uit
{

//------------------------------------------------------------------------------

void add_distance::test()
{
    namespace helper = sight::ui::test::helper;

    const std::string test_name               = "sightViewerAddDistanceTest";
    const std::string image_name              = test_name + ".png";
    const std::filesystem::path snapshot_path = sight::ui::test::tester::get_image_output_path() / image_name;
    std::filesystem::remove(snapshot_path);

    const std::filesystem::path reference_path = utest_data::dir() / "sight/ui/sight_viewer" / image_name;

    start(
        test_name,
        [&snapshot_path, &reference_path](sight::ui::test::tester& _tester)
        {
            open_file(
                _tester,
                "Inr (.inr.gz) (*.inr.gz)",
                utest_data::dir() / "sight/image/inr/image.inr.gz"
            );
            reset_negatos(_tester);

            // We want to hide the volume, we must click on the Show/hide volume button to achieve this
            helper::button::push(_tester, "top_toolbar_left/volume");

            // Activate the add distance mode
            helper::button::push(_tester, "parameter_toolbar_view/Annotation");
            helper::button::push(_tester, "annotation_tools_toolbar/Add / edit distance");

            // Add distance
            helper::selector::from_parent("top_scenes_view/1", "scene_srv").select(_tester);
            _tester.interact(
                std::make_unique<sight::ui::test::mouse_drag>(
                    QPoint(259, 250),
                    QPoint(409, 250)
                )
            );

            // Modify the second extremity
            helper::selector::from_parent("top_scenes_view/1", "scene_srv").select(_tester);
            _tester.interact(
                std::make_unique<sight::ui::test::mouse_drag>(
                    QPoint(409, 250),
                    QPoint(409, 300)
                )
            );

            // Modify the first extremity
            helper::selector::from_parent("top_scenes_view/1", "scene_srv").select(_tester);
            _tester.interact(
                std::make_unique<sight::ui::test::mouse_drag>(
                    QPoint(259, 250),
                    QPoint(419, 310)
                )
            );

            // The image appears small, zoom in with the mouse to make it bigger
            helper::scene3d::zoom(
                _tester,
                helper::selector::from_parent("top_scenes_view/1", "scene_srv"),
                7
            );

            save_snapshot(_tester, snapshot_path);
            compare_images(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::sight_viewer::uit
