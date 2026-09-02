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
// cspell:ignore Genou

#include "load_dicom.hpp"

#include <core/os/temp_path.hpp>

#include <ui/test/helper/button.hpp>

#include <utest_data/data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sight_viewer::uit::load_dicom);

namespace sight::sight_viewer::uit
{

//------------------------------------------------------------------------------

void load_dicom::test()
{
    namespace helper = sight::ui::test::helper;

    const std::string test_name               = "sightViewerLoadDicomTest";
    const std::string image_name              = test_name + ".png";
    const std::filesystem::path snapshot_path = sight::ui::test::tester::get_image_output_path() / image_name;
    std::filesystem::remove(snapshot_path);

    const std::filesystem::path reference_path(utest_data::dir() / "sight/ui/sight_viewer" / image_name);
    const auto source_folder = utest_data::dir() / "sight/Patient/Dicom/JMSGenou";

    CPPUNIT_ASSERT_MESSAGE("The DICOM test directory does not exist", std::filesystem::is_directory(source_folder));

    std::filesystem::path source_file;
    for(const auto& entry : std::filesystem::recursive_directory_iterator(source_folder))
    {
        if(entry.is_regular_file())
        {
            source_file = entry.path();
            break;
        }
    }

    CPPUNIT_ASSERT_MESSAGE("The DICOM test directory is empty", !source_file.empty());

    const sight::core::os::temp_dir single_image_directory;
    const auto single_image = single_image_directory.path() / "image.dcm";
    CPPUNIT_ASSERT(std::filesystem::copy_file(source_file, single_image));

    start(
        test_name,
        [&snapshot_path, &reference_path, &single_image_directory](sight::ui::test::tester& _tester)
        {
            open_folder(
                _tester,
                single_image_directory.path()
            );

            helper::button::push(_tester, "top_toolbar_left/volume");

            save_snapshot(_tester, snapshot_path);

            compare_images(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::sight_viewer::uit
