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

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/Scene3d.hpp>

#include <utest_data/Data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::AddDistance);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void AddDistance::test()
{
    namespace helper = sight::ui::test_core::helper;

    const std::string test_name               = "sightViewerAddDistanceTest";
    const std::string image_name              = test_name + ".png";
    const std::filesystem::path snapshot_path = sight::ui::test_core::Tester::getImageOutputPath() / image_name;
    std::filesystem::remove(snapshot_path);

    const std::filesystem::path reference_path = utest_data::Data::dir() / "sight/ui/SightViewer" / image_name;

    start(
        test_name,
        [&snapshot_path, &reference_path](sight::ui::test_core::Tester& _tester)
        {
            openFile(
                _tester,
                "Nifti or Inr images",
                utest_data::Data::dir() / "sight/image/inr/image.inr.gz"
            );
            resetNegatos(_tester);

            // We want to hide the volume, we must click on the Show/hide volume button to achieve this
            helper::Button::push(_tester, "toolBarView/Show/hide volume");

            // Activate the add distance mode
            helper::Button::push(_tester, "toolBarView/Add/Edit distance");

            // Add distance
            _tester.take(
                "ogre scene",
                [&_tester]() -> QObject* {return _tester.getMainWindow()->findChild<QWidget*>("sceneSrv");});
            _tester.interact(std::make_unique<sight::ui::test_core::MouseDrag>(QPoint(150, 250), QPoint(300, 250)));

            // Modify the distance by moving one extremity
            _tester.take(
                "ogre scene",
                [&_tester]() -> QObject* {return _tester.getMainWindow()->findChild<QWidget*>("sceneSrv");});
            _tester.interact(std::make_unique<sight::ui::test_core::MouseDrag>(QPoint(300, 250), QPoint(300, 300)));

            // Modify the distance by moving one extremity
            _tester.take(
                "ogre scene",
                [&_tester]() -> QObject* {return _tester.getMainWindow()->findChild<QWidget*>("sceneSrv");});
            _tester.interact(std::make_unique<sight::ui::test_core::MouseDrag>(QPoint(150, 250), QPoint(310, 310)));

            // The image appears small, zoom in with the mouse to make it bigger
            helper::Scene3d::zoom(_tester, "sceneSrv", 7);

            saveSnapshot(_tester, snapshot_path);
            compareImages(snapshot_path, reference_path);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
