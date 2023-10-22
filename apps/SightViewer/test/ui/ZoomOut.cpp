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

#include <ui/testCore/helper/Scene3d.hpp>

#include <utest_data/Data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::ZoomOut);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void ZoomOut::test()
{
    namespace helper = sight::ui::test_core::helper;

    const std::string test_name  = "sightViewerZoomOutTest";
    const std::string image_name = test_name + ".png";
    const std::filesystem::path snapshot_path(sight::ui::test_core::Tester::getImageOutputPath() / image_name);
    if(std::filesystem::exists(snapshot_path))
    {
        std::filesystem::remove(snapshot_path);
    }

    const std::filesystem::path reference_path(utest_data::Data::dir() / "sight/ui/SightViewer" / image_name);

    start(
        test_name,
        [&snapshot_path, &reference_path](sight::ui::test_core::Tester& _tester)
        {
            openFile(
                _tester,
                "VTK",
                utest_data::Data::dir() / "sight/mesh/vtk/sphere.vtk"
            );

            // Drag the mouse to zoom out
            helper::Scene3d::zoom(_tester, "sceneSrv", -7);

            saveSnapshot(_tester, snapshot_path);

            compareImages(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
