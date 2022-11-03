/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "ZoomOutGesture.hpp"

#include <utestData/Data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::ZoomOutGesture);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void ZoomOutGesture::test()
{
    const std::string testName  = "sightViewerZoomOutGestureTest";
    const std::string imageName = testName + ".png";
    const std::filesystem::path snapshotPath(sight::ui::test::Tester::getImageOutputPath() / imageName);
    if(std::filesystem::exists(snapshotPath))
    {
        std::filesystem::remove(snapshotPath);
    }

    // This test should give the same result as ZoomOut
    const std::filesystem::path referencePath(utestData::Data::dir()
                                              / "sight/ui/SightViewer/sightViewerZoomOutTest.png");

    start(
        testName,
        [&snapshotPath, &referencePath](sight::ui::test::Tester& tester)
        {
            openFile(
                tester,
                "VTK",
                utestData::Data::dir() / "sight/mesh/vtk/sphere.vtk"
            );

            /* Pinch the fingers to zoom out */
            // Firstly, we need the 3D scene
            tester.take(
                "ogre scene",
                [&tester]() -> QObject* {return tester.getMainWindow()->findChild<QWidget*>("genericSceneSrv");});
            // We need to a reference to it for later
            auto* ogreScene = tester.get<QWidget*>();
            tester.interact(
                // Pinch fingers...
                std::make_unique<sight::ui::test::PinchGesture>(
                    // ...with the first finger going to the center from 300 pixels higher...
                    std::pair(ogreScene->rect().center() + QPoint(0, 300), ogreScene->rect().center() + QPoint(0, 1)),
                    // ...and the second finger going to the center from 300 pixels lower
                    std::pair(ogreScene->rect().center() - QPoint(0, 300), ogreScene->rect().center() - QPoint(0, 1))
                )
            );

            saveSnapshot(tester, snapshotPath);

            compareImages(snapshotPath, referencePath);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
