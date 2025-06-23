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

#include "sagittal_negato.hpp"

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/scene3d.hpp>
#include <ui/test/helper/slider.hpp>

#include <utest_data/data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sight_viewer::uit::sagittal_negato);

namespace sight::sight_viewer::uit
{

//------------------------------------------------------------------------------

void sagittal_negato::test()
{
    namespace helper = sight::ui::test::helper;

    const std::string test_name               = "sightViewerSagittalNegatoTest";
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
                "Nifti or Inr images",
                utest_data::dir() / "sight/image/inr/image.inr.gz"
            );

            // Firstly, we must drag the mouse in the 3D scene so that the camera is "next to" the image
            helper::scene3d::rotate(_tester, "scene_srv", {1 / 4., 0});

            // We want to hide the volume, we must click on the Show/hide volume button to achieve this
            helper::button::push(_tester, "toolbar_view/Show/hide volume");

            // Then we want to display the negato views, we must click on the restore button from the 3d scene toolbar
            helper::button::push(_tester, "top_toolbar_view/Restore");

            // For the test to work, we must first reset all negatos to 0
            reset_negatos(_tester);

            // We want to move the negato, we must click in the negato slider to do that
            helper::slider::set(
                _tester,
                helper::selector::from_parent("bottom_scenes_view/0", "negato_slicer_srv"),
                250
            );

            save_snapshot(_tester, snapshot_path);

            compare_images(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::sight_viewer::uit
