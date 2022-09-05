/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "LoadDicom.hpp"

#include <core/runtime/operations.hpp>

#include <utestData/Data.hpp>

#include <QAction>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::LoadDicom);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void LoadDicom::test()
{
    const std::string testName               = "sightViewerLoadDicomTest";
    const std::string imageName              = testName + ".png";
    const std::filesystem::path snapshotPath = sight::ui::test::Tester::getImageOutputPath() / imageName;
    std::filesystem::remove(snapshotPath);

    const std::filesystem::path referencePath(utestData::Data::dir() / "sight/ui/SightViewer" / imageName);

    start(
        testName,
        [&snapshotPath, &referencePath](sight::ui::test::Tester& tester)
        {
            openFile(
                tester,
                "DICOM",
                utestData::Data::dir() / "sight/Patient/Dicom/JMSGenou"
            );

            tester.take<QWidget*>(
                "Show/hide volume button",
                [&tester]() -> QWidget*
            {
                return sight::ui::test::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Show/hide volume"
                    )
                );
            },
                [](QWidget* obj) -> bool {return obj->isEnabled();});
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());

            saveSnapshot(tester, snapshotPath);

            compareImages(snapshotPath, referencePath);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
