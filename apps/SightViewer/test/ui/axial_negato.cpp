/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "axial_negato.hpp"

#include <ui/testCore/helper/button.hpp>
#include <ui/testCore/helper/slider.hpp>

#include <utest_data/data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::axial_negato);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void axial_negato::test()
{
    namespace helper = sight::ui::test_core::helper;

    const std::string test_name               = "sightViewerAxialNegatoTest";
    const std::string image_name              = test_name + ".png";
    const std::filesystem::path snapshot_path = sight::ui::test_core::tester::get_image_output_path() / image_name;
    std::filesystem::remove(snapshot_path);

    const std::filesystem::path reference_path = utest_data::dir() / "sight/ui/SightViewer" / image_name;

    start(
        test_name,
        [&snapshot_path, &reference_path](sight::ui::test_core::tester& _tester)
        {
            open_file(
                _tester,
                "Nifti or Inr images",
                utest_data::dir() / "sight/image/inr/image.inr.gz"
            );

            // We want to hide the volume, we must click on the Show/hide volume button to achieve this
            helper::button::push(_tester, "toolBarView/Show/hide volume");

            // Then we want to display the negato view, we must click on the Negato view button to achieve this
            helper::button::push(_tester, "toolBarView/Negato view");

            // For the test to work, we must first reset all negatos to 0
            reset_negatos(_tester);

            // We want to move the negato, we must click in the negato slider to do that
            helper::slider::set(
                _tester,
                helper::selector::from_parent("topScenesView/1", "negatoSlicerSrv"),
                133
            );

            save_snapshot(_tester, snapshot_path);

            compare_images(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
