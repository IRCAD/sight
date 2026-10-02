/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include "fixture.hpp"

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/combo_box.hpp>
#include <ui/test/helper/file_dialog.hpp>
#include <ui/test/tester.hpp>

#include <utest_data/data.hpp>

#include <doctest/doctest.h>

namespace sight::example::ui::ex_material::uit
{

namespace helper = sight::ui::test::helper;

TEST_SUITE("ex_material")
{
//------------------------------------------------------------------------------

    static void save_snapshot(sight::ui::test::tester& _tester, const std::filesystem::path& _path)
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
        bool ok = QTest::qWaitFor(
            [&_path]() -> bool
            {
                return !QImage(QString::fromStdString(_path.string())).isNull();
            },
            sight::ui::test::tester::DEFAULT_TIMEOUT* 2
        );
        if(!ok)
        {
            // Called from the scenario thread: report through tester::fail(), never a doctest assertion.
            sight::ui::test::tester::fail("The writer didn't finish writing");
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "material_uniform")
    {
        const auto snapshot_path(sight::ui::test::tester::get_image_output_path("material_uniform") / "ghost_1.png");

        const auto* const  dir = "sight/ui/ex_material";
        const std::filesystem::path reference_path(utest_data::dir() / dir / "ghost_1.png");

        const std::string failure_message = start(
            "material_uniform",
            [&](sight::ui::test::tester& _tester)
            {
                // Click on the "Load series" button
                helper::button::push(_tester, "toolbar/Load a model series");

                helper::combo_box::select(
                    _tester,
                    helper::selector::from_dialog("fileTypeCombo"),
                    "VTK Legacy Files(.vtk) (*.vtk)"
                );
                // Fill the file dialog, tap PATH
                helper::file_dialog::fill(
                    _tester,
                    utest_data::dir() / "sight/mesh/vtk/sphere.vtk"
                );
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

} // namespace sight::example::ui::ex_material::uit
