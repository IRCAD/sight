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

#include "SagittalNegato.hpp"

#include <utestData/Data.hpp>

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/Scene3d.hpp>
#include <ui/testCore/helper/Slider.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::SagittalNegato);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void SagittalNegato::test()
{
    namespace helper = sight::ui::testCore::helper;

    const std::string testName               = "sightViewerSagittalNegatoTest";
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

            // Firstly, we must drag the mouse in the 3D scene so that the camera is "next to" the image
            helper::Scene3d::rotate(tester, "mainSceneSrv", {1 / 2., 0});

            // We want to hide the volume, we must click on the Show/hide volume button to achieve this
            helper::Button::push(tester, "toolBarView/Show/hide volume");

            // Then we want to display the negato view, we must click on the Negato view button to achieve this
            helper::Button::push(tester, "toolBarView/Negato view");

            // For the test to work, we must first reset all negatos to 0
            resetNegatos(tester);

            // We want to move the negato, we must click in the negato slider to do that
            helper::Slider::set(tester, "sagittalNegatoSlicerSrv", 250);

            saveSnapshot(tester, snapshotPath);

            compareImages(snapshotPath, referencePath);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
