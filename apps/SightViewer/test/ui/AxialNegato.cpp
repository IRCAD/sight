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

#include "AxialNegato.hpp"

#include <core/runtime/operations.hpp>

#include <utestData/Data.hpp>

#include <QAction>
#include <QSlider>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::AxialNegato);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void AxialNegato::test()
{
    const std::string testName               = "sightViewerAxialNegatoTest";
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
                "Nifti or Inr images",
                utestData::Data::dir() / "sight/image/inr/image.inr.gz"
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
                "Axial negato slicer",
                [&tester](QObject*) -> QObject*
            {
                return tester.getMainWindow()->findChild<QObject*>("axialNegatoSlicerSrv");
            });
            tester.yields<QSlider*>(
                "Axial negato slider",
                [](QObject* old) -> QSlider* {return old->findChild<QSlider*>();},
                [](QSlider* obj) -> bool {return obj->isEnabled();});
            QSlider* negatoSlider = tester.get<QSlider*>();
            for(int i = 0 ; i < 14 ; i++)
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