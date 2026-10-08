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

#include "test.hpp"

#include <core/os/temp_path.hpp>

#include <ui/test/helper/button.hpp>

#include <utest_data/data.hpp>

#include <doctest/doctest.h>

namespace sight::sight_viewer::uit
{

TEST_SUITE("sight_viewer")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(test, "load_dicom")
    {
        namespace helper = sight::ui::test::helper;

        const std::string test_name               = "sightViewerLoadDicomTest";
        const std::string image_name              = test_name + ".png";
        const std::filesystem::path snapshot_path = sight::ui::test::tester::get_image_output_path(test_name)
                                                    / image_name;
        std::filesystem::remove(snapshot_path);

        const std::filesystem::path reference_path(utest_data::dir() / "sight/ui/sight_viewer" / image_name);
        const auto source_folder = utest_data::dir() / "sight/Patient/Dicom/JMSGenou";

        REQUIRE_MESSAGE(std::filesystem::is_directory(source_folder), "The DICOM test directory does not exist");

        std::filesystem::path source_file;
        for(const auto& entry : std::filesystem::recursive_directory_iterator(source_folder))
        {
            if(entry.is_regular_file())
            {
                source_file = entry.path();
                break;
            }
        }

        REQUIRE_MESSAGE(!source_file.empty(), "The DICOM test directory is empty");

        const sight::core::os::temp_dir single_image_directory;
        const auto single_image = single_image_directory.path() / "image.dcm";
        REQUIRE(std::filesystem::copy_file(source_file, single_image));

        const std::string failure_message = start(
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

        // Runs on the main thread, after start() has returned: the only doctest assertion for
        // this scenario. See sight::ui::test::base::start().
        INFO(failure_message);
        REQUIRE(failure_message.empty());
    }
} // TEST_SUITE

} // namespace sight::sight_viewer::uit
