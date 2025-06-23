/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include "test.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/file_dialog.hpp>
#include <ui/test/helper/select.hpp>
#include <ui/test/helper/selector_dialog.hpp>
#include <ui/test/helper/slider.hpp>

#include <array>

namespace sight::sight_viewer::uit
{

namespace helper = sight::ui::test::helper;

//------------------------------------------------------------------------------

std::filesystem::path test::get_profile_path()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/sight_viewer/profile.xml";
}

//------------------------------------------------------------------------------

void test::open_file(
    sight::ui::test::tester& _tester,
    const std::string& _format,
    const std::filesystem::path& _path
)
{
    // Click on the "Load series" button
    helper::button::push(_tester, "toolbar_view/Load series");

    // Once we clicked the button, a selection window should appear. We select the format we want.
    helper::selector_dialog::select(_tester, _format);

    // Fill the file dialog, tap PATH
    helper::file_dialog::fill(_tester, _path);

    // Ensure the image loading is started, otherwise it will hang the test.
    QTest::qWait(1000);

    if(_format == "DICOM" || _format == "Nifti or Inr images")
    {
        // The Show/hide volume button becomes enabled when the image effectively shows up.
        helper::button::wait_for_clickability(
            _tester,
            helper::selector("toolbar_view/Show/hide volume").with_timeout(sight::ui::test::tester::DEFAULT_TIMEOUT * 5)
        );
    }
    else if(_format == "VTK")
    {
        // The Show/hide mesh button becomes enabled when the image is loaded.
        helper::button::wait_for_clickability(_tester, "toolbar_view/Show/hide mesh");
    }
}

//------------------------------------------------------------------------------

void test::save_snapshot(sight::ui::test::tester& _tester, const std::filesystem::path& _path)
{
    // Click on the "snapshot" button
    helper::button::push(_tester, "top_toolbar_view/Snapshot");

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

void test::reset_negatos(sight::ui::test::tester& _tester)
{
    const std::array negatos {"top_scenes_view/1", "bottom_scenes_view/0", "bottom_scenes_view/1"};
    for(std::string parent : negatos)
    {
        helper::slider::set(_tester, helper::selector::from_parent(parent, "negato_slicer_srv"), 0);
    }
}

} // namespace sight::sight_viewer::uit
