/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "ZoomOut.hpp"

#include <utestData/Data.hpp>

#include <ui/testCore/helper/Scene3d.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::ZoomOut);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void ZoomOut::test()
{
    namespace helper = sight::ui::testCore::helper;

    const std::string testName  = "sightViewerZoomOutTest";
    const std::string imageName = testName + ".png";
    const std::filesystem::path snapshotPath(sight::ui::testCore::Tester::getImageOutputPath() / imageName);
    if(std::filesystem::exists(snapshotPath))
    {
        std::filesystem::remove(snapshotPath);
    }

    const std::filesystem::path referencePath(utestData::Data::dir() / "sight/ui/SightViewer" / imageName);

    start(
        testName,
        [&snapshotPath, &referencePath](sight::ui::testCore::Tester& tester)
        {
            openFile(
                tester,
                "VTK",
                utestData::Data::dir() / "sight/mesh/vtk/sphere.vtk"
            );

            // Drag the mouse to zoom out
            helper::Scene3d::zoom(tester, "mainSceneSrv", -7);

            saveSnapshot(tester, snapshotPath);

            compareImages(snapshotPath, referencePath);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
