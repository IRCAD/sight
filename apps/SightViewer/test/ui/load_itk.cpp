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

#include "load_itk.hpp"

#include <ui/testCore/helper/button.hpp>

#include <utest_data/data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::load_itk);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void load_itk::test()
{
    namespace helper = sight::ui::test_core::helper;
    const std::string test_name               = "sightViewerLoadItkTest";
    const std::string image_name              = test_name + ".png";
    const std::filesystem::path snapshot_path = sight::ui::test_core::tester::get_image_output_path() / image_name;
    std::filesystem::remove(snapshot_path);

    const std::filesystem::path reference_path(utest_data::dir() / "sight/ui/SightViewer" / image_name);

    start(
        test_name,
        [&snapshot_path, &reference_path](sight::ui::test_core::tester& _tester)
        {
            open_file(
                _tester,
                "Nifti or Inr images",
                utest_data::dir() / "sight/image/inr/image.inr.gz"
            );

            helper::button::push(_tester, "toolBarView/Show/hide volume");

            save_snapshot(_tester, snapshot_path);

            compare_images(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
