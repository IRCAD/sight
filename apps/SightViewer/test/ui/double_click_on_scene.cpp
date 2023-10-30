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

#include "double_click_on_scene.hpp"

#include <QAction>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sightviewer::test::ui::double_click_on_scene);

namespace sight::sightviewer::test::ui
{

//------------------------------------------------------------------------------

void double_click_on_scene::test()
{
    start(
        "double_click_on_scene",
        [](sight::ui::test_core::tester& _tester)
        {
            // Initial situation: The negato view button is visible and the volume view button isn't
            _tester.take(
                "Negato view button",
                [&_tester]
            {
                return sight::ui::test_core::tester::get_widget_from_action(
                    _tester.get_main_window()->findChild<QAction*>(
                        "toolBarView/Negato view"
                    )
                );
            });
            _tester.doubt<QWidget*>("Negato view button is visible", [](QWidget* _obj){return _obj->isVisible();});
            _tester.take(
                "Volume view button",
                [&_tester]
            {
                return sight::ui::test_core::tester::get_widget_from_action(
                    _tester.get_main_window()->findChild<QAction*>(
                        "toolBarView/Volume view"
                    )
                );
            });
            _tester.doubt<QWidget*>("Volume view button is invisible", [](QWidget* _obj){return !_obj->isVisible();});

            // Double click on the main scene
            _tester.take(
                "ogre scene",
                [&_tester]() -> QObject* {return _tester.get_main_window()->findChild<QWidget*>("sceneSrv");});
            _tester.interact(std::make_unique<sight::ui::test_core::mouse_double_click>());

            // Now, the negato view button should be visible and the volume view button shouldn't
            _tester.take(
                "Negato view button",
                [&_tester]
            {
                return sight::ui::test_core::tester::get_widget_from_action(
                    _tester.get_main_window()->findChild<QAction*>(
                        "toolBarView/Negato view"
                    )
                );
            });
            _tester.doubt<QWidget*>("Negato view button is visible", [](QWidget* _obj){return !_obj->isVisible();});
            _tester.take(
                "Volume view button",
                [&_tester]
            {
                return sight::ui::test_core::tester::get_widget_from_action(
                    _tester.get_main_window()->findChild<QAction*>(
                        "toolBarView/Volume view"
                    )
                );
            });
            _tester.doubt<QWidget*>("Volume view button is invisible", [](QWidget* _obj){return _obj->isVisible();});

            // Double click again on the main scene
            _tester.take(
                "ogre scene",
                [&_tester]() -> QObject* {return _tester.get_main_window()->findChild<QWidget*>("sceneSrv");});
            _tester.interact(std::make_unique<sight::ui::test_core::mouse_double_click>());

            // We should return to the initial situation
            _tester.take(
                "Negato view button",
                [&_tester]
            {
                return sight::ui::test_core::tester::get_widget_from_action(
                    _tester.get_main_window()->findChild<QAction*>(
                        "toolBarView/Negato view"
                    )
                );
            });
            _tester.doubt<QWidget*>("Negato view button is visible", [](QWidget* _obj){return _obj->isVisible();});
            _tester.take(
                "Volume view button",
                [&_tester]
            {
                return sight::ui::test_core::tester::get_widget_from_action(
                    _tester.get_main_window()->findChild<QAction*>(
                        "toolBarView/Volume view"
                    )
                );
            });
            _tester.doubt<QWidget*>("Volume view button is invisible", [](QWidget* _obj){return !_obj->isVisible();});

            // Simple click on the main scene
            _tester.take(
                "ogre scene",
                [&_tester]() -> QObject* {return _tester.get_main_window()->findChild<QWidget*>("sceneSrv");});
            _tester.interact(std::make_unique<sight::ui::test_core::mouse_click>());

            // It's a no-op; we should stay in the same situation
            _tester.take(
                "Negato view button",
                [&_tester]
            {
                return sight::ui::test_core::tester::get_widget_from_action(
                    _tester.get_main_window()->findChild<QAction*>(
                        "toolBarView/Negato view"
                    )
                );
            });
            _tester.doubt<QWidget*>("Negato view button is visible", [](QWidget* _obj){return _obj->isVisible();});
            _tester.take(
                "Volume view button",
                [&_tester]
            {
                return sight::ui::test_core::tester::get_widget_from_action(
                    _tester.get_main_window()->findChild<QAction*>(
                        "toolBarView/Volume view"
                    )
                );
            });
            _tester.doubt<QWidget*>("Volume view button is invisible", [](QWidget* _obj){return !_obj->isVisible();});
        },
        true
    );
}

} // namespace sight::sightviewer::test::ui
