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

#include "helper.hpp"

#include <core/os/temp_path.hpp>

#include <ui/test/gui_test.hpp>

#include <utest_data/data.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <filesystem>
#include <vector>

namespace sight::sight_viewer::uit
{

SIGHT_GUI_TEST_CASE("load_dicom")
{
    using namespace std::chrono_literals;

    // A single image: the whole series takes a minute and proves nothing more.
    const auto series = utest_data::dir() / "sight/Patient/Dicom/JMSGenou";
    REQUIRE(std::filesystem::is_directory(series));
    std::vector<std::filesystem::path> files;
    for(const auto& entry : std::filesystem::recursive_directory_iterator(series))
    {
        if(entry.is_regular_file())
        {
            files.push_back(entry.path());
        }
    }

    REQUIRE_FALSE(files.empty());

    // A directory lists its files in an order of its own on each system: the first by name is the same everywhere.
    const core::os::temp_dir single_image;
    REQUIRE(std::filesystem::copy_file(std::ranges::min(files), single_image.path() / "image.dcm"));

    ui.click("data_tools_row_1/Load DICOM Folders");
    ui.choose_folder(single_image.path());
    ui.expect("top_toolbar_left/volume").within(180s).enabled();

    ui.click("top_toolbar_left/volume");
    // The two other negatos of a single slice are seen edge on.
    ui.expect_image(MAIN_SCENE, reference("load_dicom"), edge_on_negatos());
}

} // namespace sight::sight_viewer::uit
