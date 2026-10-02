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

#include "test.hpp"

#include <core/runtime/path.hpp>

#include <ui/test/helper/button.hpp>
#include <ui/test/helper/combo_box.hpp>
#include <ui/test/helper/file_dialog.hpp>
#include <ui/test/helper/select.hpp>
#include <ui/test/helper/slider.hpp>

#include <array>

namespace sight::sight_viewer::uit
{

namespace helper = sight::ui::test::helper;

//------------------------------------------------------------------------------

test::test() :
    sight::ui::test::base(sight::core::runtime::working_path() / "share/sight/sight_viewer/profile.xml")
{
}

//------------------------------------------------------------------------------

void test::open_file(
    sight::ui::test::tester& _tester,
    const std::string& _format,
    const std::filesystem::path& _path
)
{
    // Click on the "Load Files" button
    helper::button::push(_tester, "data_tools_row_1/Load Files");

    // Select the file extension directly in the file dialog.
    helper::combo_box::select(
        _tester,
        helper::selector::from_dialog("fileTypeCombo"),
        _format
    );
    helper::combo_box::value_equals(
        _tester,
        helper::selector::from_dialog("fileTypeCombo"),
        _format
    );

    // Fill the file dialog.
    helper::file_dialog::fill(_tester, _path);

    if(_format == "Inr (.inr) (*.inr *.inr.gz)" || _format == "NIfTI (.nii) (*.nii *.nii.gz)")
    {
        helper::button::wait_for_clickability(
            _tester,
            helper::selector("top_toolbar_left/volume").with_timeout(
                sight::ui::test::tester::DEFAULT_TIMEOUT * 5
            )
        );
    }
    else if(_format == "VTK Legacy Files(.vtk) (*.vtk)")
    {
        // The Show/hide mesh button becomes enabled when the image is loaded.
        helper::button::wait_for_clickability(_tester, "top_toolbar_left/mesh");
    }
}

//-------------------------------------------------------
void test::open_folder(
    sight::ui::test::tester& _tester,
    const std::filesystem::path& _path
)
{
    // sight::ui::test::tester::fail() throws tester_assertion_failed, which base::start() catches on the
    // scenario thread. A doctest assertion must not be used here, since open_folder() runs on that thread.
    if(!std::filesystem::is_directory(_path))
    {
        sight::ui::test::tester::fail("The DICOM test directory does not exist");
    }

    helper::button::push(
        _tester,
        "data_tools_row_1/Load DICOM Folders"
    );

    helper::file_dialog::fill(
        _tester,
        _path
    );

    // Loading the 512x512x404 DICOM volume can exceed 50 seconds on a busy CI runner.
    helper::button::wait_for_clickability(
        _tester,
        helper::selector("top_toolbar_left/volume").with_timeout(
            sight::ui::test::tester::DEFAULT_TIMEOUT * 18
        )
    );
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
    bool ok = QTest::qWaitFor(
        [&_path]() -> bool
        {
            return !QImage(QString::fromStdString(_path.string())).isNull();
        },
        sight::ui::test::tester::DEFAULT_TIMEOUT* 2
    );
    if(!ok)
    {
        sight::ui::test::tester::fail("The writer didn't finish writing");
    }
}

//------------------------------------------------------------------------------

void test::reset_negatos(sight::ui::test::tester& _tester)
{
    const std::array negatos {"top_scenes_view/0", "bottom_scenes_view/0", "bottom_scenes_view/1"};
    for(std::string parent : negatos)
    {
        helper::slider::set(_tester, helper::selector::from_parent(parent, "negato_slicer_srv"), 0);
    }
}

} // namespace sight::sight_viewer::uit
