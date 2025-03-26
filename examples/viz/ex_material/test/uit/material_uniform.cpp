/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "material_uniform.hpp"

#include <core/runtime/path.hpp>

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/file_dialog.hpp>
#include <ui/test/helper/selector_dialog.hpp>
#include <ui/test/tester.hpp>

#include <utest_data/data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::examples::ui::ex_material::uit::material_uniform);

namespace sight::examples::ui::ex_material::uit
{

namespace helper = sight::ui::test::helper;

//------------------------------------------------------------------------------

std::filesystem::path material_uniform::get_profile_path()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/ex_material/profile.xml";
}

//------------------------------------------------------------------------------

void save_snapshot(sight::ui::test::tester& _tester, const std::filesystem::path& _path)
{
    // Click on the "snapshot" button
    helper::button::push(_tester, "toolbar/Make a snapshot");

    // Fill the file dialog, tap PATH
    helper::file_dialog::fill(_tester, _path);

    // Once we have pressed Enter, the path must be created...
    _tester.doubt(
        "the snapshot is saved",
        [&_path](QObject*) -> bool {return std::filesystem::exists(_path);},
        sight::ui::test::tester::DEFAULT_TIMEOUT*2
    );
    // ...and the image should be valid.
    bool ok = QTest::qWaitFor([&_path]() -> bool {return !QImage(QString::fromStdString(_path.string())).isNull();});
    CPPUNIT_ASSERT_MESSAGE("The writer didn't finish writing", ok);
}

//------------------------------------------------------------------------------

void material_uniform::test()
{
    const auto snapshot_path(sight::ui::test::tester::get_image_output_path() / "ghost_1.png");

    const auto* const  dir = "sight/ui/ex_material";
    const std::filesystem::path reference_path(utest_data::dir() / dir / "ghost_1.png");

    start(
        "material_uniform",
        [&](sight::ui::test::tester& _tester)
        {
            // Click on the "Load series" button
            helper::button::push(_tester, "toolbar/Load a model series");

            // Once we clicked the button, a selection window should appear. We select the format we want.
            helper::selector_dialog::select(_tester, "VTK");

            // Fill the file dialog, tap PATH
            helper::file_dialog::fill(_tester, utest_data::dir() / "sight/mesh/vtk/sphere.vtk");

            // Ensure the image loading is started, otherwise it will hang the test.
            QTest::qWait(1000);

            save_snapshot(_tester, snapshot_path);
            compare_images(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::examples::ui::ex_material::uit
