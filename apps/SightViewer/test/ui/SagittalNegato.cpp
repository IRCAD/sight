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

#include "SagittalNegato.hpp"

#include <core/runtime/operations.hpp>

#include <utestData/Data.hpp>

#include <QAction>
#include <QSlider>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::SagittalNegato);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void SagittalNegato::test()
{
    const std::string testName               = "sightViewerSagittalNegatoTest";
    const std::string imageName              = testName + ".png";
    const std::filesystem::path snapshotPath = sight::ui::test::Tester::getImageOutputPath() / imageName;
    std::filesystem::remove(snapshotPath);

    const std::filesystem::path referencePath = utestData::Data::dir() / "sight/ui/SightViewer" / imageName;

    start(
        testName,
        [&snapshotPath, &referencePath](sight::ui::test::Tester& tester)
        {
            openFile(
                tester,
                "SeriesDB image Reader (ITK)",
                utestData::Data::dir() / "sight/image/inr/image.inr.gz"
            );

            // Firstly, we must drag the mouse in the 3D scene so that the camera is "next to" the image
            tester.take(
                "ogre scene",
                [&tester]() -> QObject*
            {
                return tester.getMainWindow()->findChild<QWidget*>("genericSceneSrv");
            });
            const QWidget* ogreScene = tester.get<QWidget*>();
            const int width          = ogreScene->width();
            tester.interact(
                std::make_unique<sight::ui::test::MouseDrag>(
                    sight::ui::test::Tester::leftOf(ogreScene),
                    sight::ui::test::Tester::leftOf(ogreScene) + QPoint(
                        width / 2,
                        0
                    )
                )
            );

            // We want to hide the volume, we must click on the Show/hide volume button to achieve this
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
                // We want to click on it, so it should be clickable
                [](QWidget* obj) -> bool {return obj->isEnabled();});
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());

            // Then we want to display the negato view, we must click on the Negato view button to achieve this
            tester.take(
                "Negato view button",
                [&tester]() -> QObject*
            {
                return sight::ui::test::Tester::getWidgetFromAction(
                    tester.getMainWindow()->findChild<QAction*>(
                        "toolBarView/Negato view"
                    )
                );
            });
            tester.interact(std::make_unique<sight::ui::test::MouseClick>());

            // For the test to work, we must first reset all negatos to 0
            resetNegatos(tester);

            // We want to move the negato, we must click in the negato slider to do that
            tester.yields(
                "Sagittal negato slicer",
                [&tester](QObject*) -> QObject*
            {
                return tester.getMainWindow()->findChild<QObject*>("sagittalNegatoSlicerSrv");
            });
            tester.yields<QSlider*>(
                "Sagittal negato slider",
                [](QObject* old) -> QSlider* {return old->findChild<QSlider*>();},
                [](QSlider* obj) -> bool {return obj->isEnabled();});
            const QSlider* negatoSlider = tester.get<QSlider*>();
            for(int i = 0 ; i < 25 ; i++)
            {
                tester.interact(
                    std::make_unique<sight::ui::test::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test::Tester::rightOf(negatoSlider)
                    )
                );
            }

            saveSnapshot(tester, snapshotPath);

            compareImages(snapshotPath, referencePath);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
