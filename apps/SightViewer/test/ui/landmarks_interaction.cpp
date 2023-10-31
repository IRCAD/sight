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

#include "landmarks_interaction.hpp"

#include "ui/testCore/helper/color_parameter.hpp"
#include "ui/testCore/tester.hpp"

#include "zoom_out.hpp"

#include <ui/testCore/helper/button.hpp>
#include <ui/testCore/helper/slider.hpp>

#include <utest_data/data.hpp>

#include <qnamespace.h>
#include <QTreeWidget>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::landmarks_interaction);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void landmarks_interaction::test()
{
    namespace helper = sight::ui::test_core::helper;
    using selector   = helper::selector;

    const std::string test_name  = "sightViewerLandmarksInteractionTest";
    const std::string image_name = test_name + ".png";
    const std::filesystem::path first_snapshot_path(sight::ui::test_core::tester::get_image_output_path()
                                                    / (test_name + "_1.png"));
    std::filesystem::remove(first_snapshot_path);
    const std::filesystem::path second_snapshot_path(sight::ui::test_core::tester::get_image_output_path()
                                                     / (test_name + "_2.png"));
    std::filesystem::remove(second_snapshot_path);
    const std::filesystem::path first_reference_path(utest_data::dir() / "sight/ui/SightViewer"
                                                     / (test_name + "_1.png"));
    const std::filesystem::path second_reference_path(utest_data::dir() / "sight/ui/SightViewer"
                                                      / (test_name + "_2.png"));

    start(
        test_name,
        [&](sight::ui::test_core::tester& _tester)
        {
            open_file(
                _tester,
                "Nifti or Inr images",
                utest_data::dir() / "sight/image/inr/image.inr.gz"
            );

            {
                auto bt = _tester.add_in_backtrace("Create three landmarks groups");
                helper::button::push(_tester, "toolBarView/Show/hide volume");
                helper::button::push(_tester, "parameterToolBarView/Show landmarks manager");

                helper::button::push(_tester, "landmarksParameterSrv/New Group");
                helper::button::push(_tester, "landmarksParameterSrv/New Group");
                helper::button::push(_tester, "landmarksParameterSrv/New Group");

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
                helper::color_parameter::select(_tester, selector::current(), {255, 0, 0});
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.yields(
                    "Second group color button",
                    [](QObject* _old)
                {
                    auto& tree = dynamic_cast<QTreeWidget&>(*_old);
                    return tree.itemWidget(tree.topLevelItem(1), 1);
                });
                helper::color_parameter::select(_tester, selector::current(), {0, 255, 0});
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.yields(
                    "Third group color button",
                    [](QObject* _old)
                {
                    auto& tree = dynamic_cast<QTreeWidget&>(*_old);
                    return tree.itemWidget(tree.topLevelItem(2), 1);
                });
                helper::color_parameter::select(_tester, selector::current(), {0, 0, 255});
            }

            {
                auto bt = _tester.add_in_backtrace("Place three landmarks from the first group");
                helper::button::push(_tester, "toolBarView/Enable landmarks edit mode");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.do_something_asynchronously<QTreeWidget*>(
                    [](QTreeWidget* _obj)
                {
                    _obj->setCurrentItem(_obj->topLevelItem(0));
                });
                helper::slider::set(_tester, "landmarksParameterSrv/Size", 100);
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(_tester.get<QWidget*>())
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(_tester.get<QWidget*>()) + QPoint(-150, -150)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(_tester.get<QWidget*>()) + QPoint(0, -150)
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
                auto bt = _tester.add_in_backtrace("Place three landmarks from the second group");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.do_something_asynchronously<QTreeWidget*>(
                    [](QTreeWidget* _obj)
                {
                    _obj->setCurrentItem(_obj->topLevelItem(1));
                });
                helper::slider::set(_tester, "landmarksParameterSrv/Size", 100);
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(_tester.get<QWidget*>()) + QPoint(150, -150)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(_tester.get<QWidget*>()) + QPoint(-150, 0)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(_tester.get<QWidget*>()) + QPoint(150, 0)
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
                auto bt = _tester.add_in_backtrace("Place three landmarks from the third group");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.do_something_asynchronously<QTreeWidget*>(
                    [](QTreeWidget* _obj)
                {
                    _obj->setCurrentItem(_obj->topLevelItem(2));
                });
                helper::slider::set(_tester, "landmarksParameterSrv/Size", 100);
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(-150, 150)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(0, 150)
                    )
                );
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(
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

            save_snapshot(_tester, first_snapshot_path);
            compare_images(first_snapshot_path, first_reference_path);

            {
                auto bt = _tester.add_in_backtrace("Remove three landmarks from the first group");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(
                            _tester.get<QWidget*>()
                        )
                    )
                );
                helper::button::push(_tester, "landmarksAdp/binButton");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(-150, -150)
                    )
                );
                helper::button::push(_tester, "landmarksAdp/binButton");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(0, -150)
                    )
                );
                helper::button::push(_tester, "landmarksAdp/binButton");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be no landmarks in the first group",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItem(0)->childCount() == 0;
                });
            }

            {
                auto bt = _tester.add_in_backtrace("Remove two landmarks from the second group");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(150, 0)
                    )
                );
                helper::button::push(_tester, "landmarksAdp/binButton");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(-150, 0)
                    )
                );
                helper::button::push(_tester, "landmarksAdp/binButton");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be one landmark in the second group",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItem(1)->childCount() == 1;
                });
            }

            {
                auto bt = _tester.add_in_backtrace("Remove one landmark from the third group");
                _tester.take("3D scene", "sceneSrv");
                _tester.interact(
                    std::make_unique<sight::ui::test_core::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        sight::ui::test_core::tester::center_of(
                            _tester.get<QWidget*>()
                        )
                        + QPoint(150, 150)
                    )
                );
                helper::button::push(_tester, "landmarksAdp/binButton");
                _tester.take("Landmarks tree widget", "landmarksParameterSrv/treeWidget");
                _tester.doubt<QTreeWidget*>(
                    "There must be two landmarks in the third group",
                    [](QTreeWidget* _obj)
                {
                    return _obj->topLevelItem(2)->childCount() == 2;
                });
            }

            save_snapshot(_tester, second_snapshot_path);
            compare_images(second_snapshot_path, second_reference_path);
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
