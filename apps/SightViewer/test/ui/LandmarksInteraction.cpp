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

#include <ui/testCore/helper/Button.hpp>
#include <ui/testCore/helper/Slider.hpp>

#include <utest_data/Data.hpp>

#include <qnamespace.h>
#include <QTreeWidget>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::LandmarksInteraction);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void LandmarksInteraction::test()
{
    namespace helper = sight::ui::test_core::helper;
    using Select     = helper::Select;

    const std::string test_name  = "sightViewerLandmarksInteractionTest";
    const std::string image_name = test_name + ".png";
    const std::filesystem::path first_snapshot_path(sight::ui::test_core::Tester::getImageOutputPath()
                                                    / (test_name + "_1.png"));
    std::filesystem::remove(first_snapshot_path);
    const std::filesystem::path second_snapshot_path(sight::ui::test_core::Tester::getImageOutputPath()
                                                     / (test_name + "_2.png"));
    std::filesystem::remove(second_snapshot_path);
    const std::filesystem::path first_reference_path(utest_data::Data::dir() / "sight/ui/SightViewer"
                                                     / (test_name + "_1.png"));
    const std::filesystem::path second_reference_path(utest_data::Data::dir() / "sight/ui/SightViewer"
                                                      / (test_name + "_2.png"));

    start(
        test_name,
        [&](sight::ui::test_core::Tester& _tester)
        {
            openFile(
                _tester,
                "Nifti or Inr images",
                utest_data::Data::dir() / "sight/image/inr/image.inr.gz"
            );

            {
                auto bt = _tester.addInBacktrace("Create three landmarks groups");
                helper::Button::push(_tester, "toolBarView/Show/hide volume");
                helper::Button::push(_tester, "parameterToolBarView/Show landmarks manager");

                helper::Button::push(_tester, "landmarksParameterSrv/New Group");
                helper::Button::push(_tester, "landmarksParameterSrv/New Group");
                helper::Button::push(_tester, "landmarksParameterSrv/New Group");

                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be 3 groups in the landmarks widget",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItemCount() == 3;
                });
                _tester.yields(
                    "First group color button",
                    [](QObject* _old)
                {
                    auto& tree = dynamic_cast<QTreeWidget&>(*_old);
                    return tree.itemWidget(tree.topLevelItem(0), 1);
                });
                helper::ColorParameter::select(_tester, Select::current(), {255, 0, 0});
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.yields(
                    "Second group color button",
                    [](QObject* _old)
                {
                    auto& tree = dynamic_cast<QTreeWidget&>(*_old);
                    return tree.itemWidget(tree.topLevelItem(1), 1);
                });
                helper::ColorParameter::select(_tester, Select::current(), {0, 255, 0});
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.yields(
                    "Third group color button",
                    [](QObject* _old)
                {
                    auto& tree = dynamic_cast<QTreeWidget&>(*_old);
                    return tree.itemWidget(tree.topLevelItem(2), 1);
                });
                helper::ColorParameter::select(_tester, Select::current(), {0, 0, 255});
            }

            {
                auto bt = _tester.addInBacktrace("Place three landmarks from the first group");
                helper::Button::push(_tester, "toolBarView/Enable landmarks edit mode");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doSomethingAsynchronously<QTreeWidget*>(
                    [](QTreeWidget* _obj)
                {
                    _obj->setCurrentItem(_obj->topLevelItem(0));
                });
                helper::Slider::set(_tester, "landmarksParameterSrv/Size", 100);
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(_tester.get<QWidget*>())
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(_tester.get<QWidget*>()) + QPoint(-150, -150)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(_tester.get<QWidget*>()) + QPoint(0, -150)
                    )
                );
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be three landmarks in the first group",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItem(0)->childCount() == 3;
                });
            }

            {
                auto bt = _tester.addInBacktrace("Place three landmarks from the second group");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doSomethingAsynchronously<QTreeWidget*>(
                    [](QTreeWidget* _obj)
                {
                    _obj->setCurrentItem(_obj->topLevelItem(1));
                });
                helper::Slider::set(_tester, "landmarksParameterSrv/Size", 100);
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(_tester.get<QWidget*>()) + QPoint(150, -150)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(_tester.get<QWidget*>()) + QPoint(-150, 0)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(_tester.get<QWidget*>()) + QPoint(150, 0)
                    )
                );
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be three landmarks in the second group",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItem(1)->childCount() == 3;
                });
            }

            {
                auto bt = _tester.addInBacktrace("Place three landmarks from the third group");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doSomethingAsynchronously<QTreeWidget*>(
                    [](QTreeWidget* _obj)
                {
                    _obj->setCurrentItem(_obj->topLevelItem(2));
                });
                helper::Slider::set(_tester, "landmarksParameterSrv/Size", 100);
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(-150, 150)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(0, 150)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(150, 150)
                    )
                );
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be three landmarks in the third group",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItem(2)->childCount() == 3;
                });
            }

            saveSnapshot(_tester, first_snapshot_path);
            compareImages(first_snapshot_path, first_reference_path);

            {
                auto bt = _tester.addInBacktrace("Remove three landmarks from the first group");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(
                            _tester.get<QWidget*>()
                        )
                    )
                );
                helper::Button::push(_tester, "landmarksAdp/binButton");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(-150, -150)
                    )
                );
                helper::Button::push(_tester, "landmarksAdp/binButton");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(0, -150)
                    )
                );
                helper::Button::push(_tester, "landmarksAdp/binButton");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be no landmarks in the first group",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItem(0)->childCount() == 0;
                });
            }

            {
                auto bt = _tester.addInBacktrace("Remove two landmarks from the second group");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(150, 0)
                    )
                );
                helper::Button::push(_tester, "landmarksAdp/binButton");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(-150, 0)
                    )
                );
                helper::Button::push(_tester, "landmarksAdp/binButton");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be one landmark in the second group",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItem(1)->childCount() == 1;
                });
            }

            {
                auto bt = _tester.addInBacktrace("Remove one landmark from the third group");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::MouseClick>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::Tester::centerOf(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(150, 150)
                    )
                );
                helper::Button::push(_tester, "landmarksAdp/binButton");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be two landmarks in the third group",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItem(2)->childCount() == 2;
                });
            }

            saveSnapshot(_tester, second_snapshot_path);
            compareImages(second_snapshot_path, second_reference_path);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
