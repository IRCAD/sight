/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "LandmarksInteraction.hpp"

#include "ui/testCore/helper/ColorParameter.hpp"
#include "ui/testCore/Tester.hpp"

#include "ZoomOut.hpp"

#include <utestData/Data.hpp>

#include <qnamespace.h>
#include <QTreeWidget>

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/Slider.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::LandmarksInteraction);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void LandmarksInteraction::test()
{
    namespace helper = sight::ui::testCore::helper;
    using Select     = helper::Select;

    const std::string testName  = "sightViewerLandmarksInteractionTest";
    const std::string imageName = testName + ".png";
    const std::filesystem::path firstSnapshotPath(sight::ui::testCore::Tester::getImageOutputPath()
                                                  / (testName + "_1.png"));
    std::filesystem::remove(firstSnapshotPath);
    const std::filesystem::path secondSnapshotPath(sight::ui::testCore::Tester::getImageOutputPath()
                                                   / (testName + "_2.png"));
    std::filesystem::remove(secondSnapshotPath);
    const std::filesystem::path firstReferencePath(utestData::Data::dir() / "sight/ui/SightViewer"
                                                   / (testName + "_1.png"));
    const std::filesystem::path secondReferencePath(utestData::Data::dir() / "sight/ui/SightViewer"
                                                    / (testName + "_2.png"));

    start(
        testName,
        [&](sight::ui::testCore::Tester& tester)
        {
            openFile(
                tester,
                "Nifti or Inr images",
                utestData::Data::dir() / "sight/image/inr/image.inr.gz"
            );

            {
                auto bt = tester.addInBacktrace("Create three landmarks groups");
                helper::Button::push(tester, "toolBarView/Show/hide volume");
                helper::Button::push(tester, "parameterToolBarView/Show landmarks manager");

                helper::Button::push(tester, "landmarksParameterSrv/New Group");
                helper::Button::push(tester, "landmarksParameterSrv/New Group");
                helper::Button::push(tester, "landmarksParameterSrv/New Group");

                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doubt<QTreeWidget*>(
                    "There must be 3 groups in the landmarks widget",
                    [](QTreeWidget* obj)
                {
                    return obj->topLevelItemCount() == 3;
                });
                tester.yields(
                    "First group color button",
                    [](QObject* old)
                {
                    auto& tree = dynamic_cast<QTreeWidget&>(*old);
                    return tree.itemWidget(tree.topLevelItem(0), 1);
                });
                helper::ColorParameter::select(tester, Select::current(), {255, 0, 0});
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.yields(
                    "Second group color button",
                    [](QObject* old)
                {
                    auto& tree = dynamic_cast<QTreeWidget&>(*old);
                    return tree.itemWidget(tree.topLevelItem(1), 1);
                });
                helper::ColorParameter::select(tester, Select::current(), {0, 255, 0});
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.yields(
                    "Third group color button",
                    [](QObject* old)
                {
                    auto& tree = dynamic_cast<QTreeWidget&>(*old);
                    return tree.itemWidget(tree.topLevelItem(2), 1);
                });
                helper::ColorParameter::select(tester, Select::current(), {0, 0, 255});
            }

            {
                auto bt = tester.addInBacktrace("Place three landmarks from the first group");
                helper::Button::push(tester, "toolBarView/Enable landmarks edit mode");
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doSomethingAsynchronously<QTreeWidget*>(
                    [](QTreeWidget* obj)
                {
                    obj->setCurrentItem(obj->topLevelItem(0));
                });
                helper::Slider::set(tester, "landmarksParameterSrv/Size", 100);
                tester.take("3D scene", "sceneSrv");
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(tester.get<QWidget*>())
                    )
                );
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(tester.get<QWidget*>()) + QPoint(-150, -150)
                    )
                );
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(tester.get<QWidget*>()) + QPoint(0, -150)
                    )
                );
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doubt<QTreeWidget*>(
                    "There must be three landmarks in the first group",
                    [](QTreeWidget* obj)
                {
                    return obj->topLevelItem(0)->childCount() == 3;
                });
            }

            {
                auto bt = tester.addInBacktrace("Place three landmarks from the second group");
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doSomethingAsynchronously<QTreeWidget*>(
                    [](QTreeWidget* obj)
                {
                    obj->setCurrentItem(obj->topLevelItem(1));
                });
                helper::Slider::set(tester, "landmarksParameterSrv/Size", 100);
                tester.take("3D scene", "sceneSrv");
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(tester.get<QWidget*>()) + QPoint(150, -150)
                    )
                );
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(tester.get<QWidget*>()) + QPoint(-150, 0)
                    )
                );
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(tester.get<QWidget*>()) + QPoint(150, 0)
                    )
                );
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doubt<QTreeWidget*>(
                    "There must be three landmarks in the second group",
                    [](QTreeWidget* obj)
                {
                    return obj->topLevelItem(1)->childCount() == 3;
                });
            }

            {
                auto bt = tester.addInBacktrace("Place three landmarks from the third group");
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doSomethingAsynchronously<QTreeWidget*>(
                    [](QTreeWidget* obj)
                {
                    obj->setCurrentItem(obj->topLevelItem(2));
                });
                helper::Slider::set(tester, "landmarksParameterSrv/Size", 100);
                tester.take("3D scene", "sceneSrv");
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(
                            tester.get<QWidget*>()
                        )
                        + QPoint(-150, 150)
                    )
                );
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(
                            tester.get<QWidget*>()
                        )
                        + QPoint(0, 150)
                    )
                );
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(
                            tester.get<QWidget*>()
                        )
                        + QPoint(150, 150)
                    )
                );
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doubt<QTreeWidget*>(
                    "There must be three landmarks in the third group",
                    [](QTreeWidget* obj)
                {
                    return obj->topLevelItem(2)->childCount() == 3;
                });
            }

            saveSnapshot(tester, firstSnapshotPath);
            compareImages(firstSnapshotPath, firstReferencePath);

            {
                auto bt = tester.addInBacktrace("Remove three landmarks from the first group");
                tester.take("3D scene", "sceneSrv");
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(
                            tester.get<QWidget*>()
                        )
                    )
                );
                helper::Button::push(tester, "landmarksAdp/binButton");
                tester.take("3D scene", "sceneSrv");
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(
                            tester.get<QWidget*>()
                        )
                        + QPoint(-150, -150)
                    )
                );
                helper::Button::push(tester, "landmarksAdp/binButton");
                tester.take("3D scene", "sceneSrv");
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(
                            tester.get<QWidget*>()
                        )
                        + QPoint(0, -150)
                    )
                );
                helper::Button::push(tester, "landmarksAdp/binButton");
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doubt<QTreeWidget*>(
                    "There must be no landmarks in the first group",
                    [](QTreeWidget* obj)
                {
                    return obj->topLevelItem(0)->childCount() == 0;
                });
            }

            {
                auto bt = tester.addInBacktrace("Remove two landmarks from the second group");
                tester.take("3D scene", "sceneSrv");
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(
                            tester.get<QWidget*>()
                        )
                        + QPoint(150, 0)
                    )
                );
                helper::Button::push(tester, "landmarksAdp/binButton");
                tester.take("3D scene", "sceneSrv");
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(
                            tester.get<QWidget*>()
                        )
                        + QPoint(-150, 0)
                    )
                );
                helper::Button::push(tester, "landmarksAdp/binButton");
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doubt<QTreeWidget*>(
                    "There must be one landmark in the second group",
                    [](QTreeWidget* obj)
                {
                    return obj->topLevelItem(1)->childCount() == 1;
                });
            }

            {
                auto bt = tester.addInBacktrace("Remove one landmark from the third group");
                tester.take("3D scene", "sceneSrv");
                tester.interact(
                    std::make_unique<sight::ui::testCore::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::testCore::Tester::centerOf(
                            tester.get<QWidget*>()
                        )
                        + QPoint(150, 150)
                    )
                );
                helper::Button::push(tester, "landmarksAdp/binButton");
                tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                tester.doubt<QTreeWidget*>(
                    "There must be two landmarks in the third group",
                    [](QTreeWidget* obj)
                {
                    return obj->topLevelItem(2)->childCount() == 2;
                });
            }

            saveSnapshot(tester, secondSnapshotPath);
            compareImages(secondSnapshotPath, secondReferencePath);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
