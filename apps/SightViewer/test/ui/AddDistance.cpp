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

#include "AddDistance.hpp"

#include <utestData/Data.hpp>

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/Scene3d.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::AddDistance);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void AddDistance::test()
{
    namespace helper = sight::ui::testCore::helper;

    const std::string testName               = "sightViewerAddDistanceTest";
    const std::string imageName              = testName + ".png";
    const std::filesystem::path snapshotPath = sight::ui::testCore::Tester::getImageOutputPath() / imageName;
    std::filesystem::remove(snapshotPath);

    const std::filesystem::path referencePath = utestData::Data::dir() / "sight/ui/SightViewer" / imageName;

    start(
        testName,
        [&snapshotPath, &referencePath](sight::ui::testCore::Tester& tester)
        {
            openFile(
                tester,
                "Nifti or Inr images",
                utestData::Data::dir() / "sight/image/inr/image.inr.gz"
            );
            resetNegatos(tester);

            // We want to hide the volume, we must click on the Show/hide volume button to achieve this
            helper::Button::push(tester, "toolBarView/Show/hide volume");

            // Activate the add distance mode
            helper::Button::push(tester, "toolBarView/Add/Edit distance");

            // Add distance
            tester.take(
                "ogre scene",
                [&tester]() -> QObject* {return tester.getMainWindow()->findChild<QWidget*>("sceneSrv");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseDrag>(QPoint(150, 250), QPoint(300, 250)));

            // Modify the distance by moving one extremity
            tester.take(
                "ogre scene",
                [&tester]() -> QObject* {return tester.getMainWindow()->findChild<QWidget*>("sceneSrv");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseDrag>(QPoint(300, 250), QPoint(300, 300)));

            // Modify the distance by moving one extremity
            tester.take(
                "ogre scene",
                [&tester]() -> QObject* {return tester.getMainWindow()->findChild<QWidget*>("sceneSrv");});
            tester.interact(std::make_unique<sight::ui::testCore::MouseDrag>(QPoint(150, 250), QPoint(310, 310)));

            // The image appears small, zoom in with the mouse to make it bigger
            helper::Scene3d::zoom(tester, "sceneSrv", 7);

            saveSnapshot(tester, snapshotPath);
            compareImages(snapshotPath, referencePath);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
