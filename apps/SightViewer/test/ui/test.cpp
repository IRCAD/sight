/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/FileDialog.hpp>
#include <ui/testCore/helper/Select.hpp>
#include <ui/testCore/helper/selector_dialog.hpp>
#include <ui/testCore/helper/Slider.hpp>

#include <array>

namespace sight::sightviewer::test::ui
{

namespace helper = sight::ui::test_core::helper;

//------------------------------------------------------------------------------

std::filesystem::path test::getProfilePath()
{
    const std::filesystem::path cwd = sight::core::runtime::working_path();
    return cwd / "share/sight/SightViewer/profile.xml";
}

//------------------------------------------------------------------------------

void test::openFile(
    sight::ui::test_core::Tester& _tester,
    const std::string& _format,
    const std::filesystem::path& _path
)
{
    // Click on the "Load series" button
    helper::Button::push(_tester, "toolBarView/Load series");

    // Once we clicked the button, a selection window should appear. We select the format we want.
    helper::selector_dialog::select(_tester, _format);

    // Fill the file dialog, tap PATH
    helper::FileDialog::fill(_tester, _path);

    if(_format == "DICOM" || _format == "Nifti or Inr images")
    {
        // The Show/hide volume button becomes enabled when the image effectively shows up.
        helper::Button::waitForClickability(
            _tester,
            helper::Select("toolBarView/Show/hide volume").withTimeout(sight::ui::test_core::Tester::DEFAULT_TIMEOUT*5)
        );
    }
    else if(_format == "VTK")
    {
        // The Show/hide mesh button becomes enabled when the image is loaded.
        helper::Button::waitForClickability(_tester, "toolBarView/Show/hide mesh");
    }
}

//------------------------------------------------------------------------------

void test::saveSnapshot(sight::ui::test_core::Tester& _tester, const std::filesystem::path& _path)
{
    // Click on the "snapshot" button
    helper::Button::push(_tester, "topToolbarView/Snapshot");

    // Fill the file dialog, tap PATH
    helper::FileDialog::fill(_tester, _path);

    // Once we have pressed Enter, the path must be created...
    _tester.doubt(
        "the snapshot is saved",
        [&_path](QObject*) -> bool {return std::filesystem::exists(_path);},
        sight::ui::test_core::Tester::DEFAULT_TIMEOUT*2
    );
    // ...and the image should be valid.
    bool ok = QTest::qWaitFor([&_path]() -> bool {return !QImage(QString::fromStdString(_path.string())).isNull();});
    CPPUNIT_ASSERT_MESSAGE("The writer didn't finish writing", ok);
}

//------------------------------------------------------------------------------

void test::resetNegatos(sight::ui::test_core::Tester& _tester)
{
    const std::array negatos {"topScenesView/1", "bottomScenesView/0", "bottomScenesView/1"};
    for(std::string parent : negatos)
    {
        helper::Slider::set(_tester, helper::Select::fromParent(parent, "negatoSlicerSrv"), 0);
    }
}

} // namespace sight::sightviewer::test::ui
