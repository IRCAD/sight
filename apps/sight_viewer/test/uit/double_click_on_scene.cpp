/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

CPPUNIT_TEST_SUITE_REGISTRATION(sight::sight_viewer::uit::double_click_on_scene);

namespace sight::sight_viewer::uit
{

//------------------------------------------------------------------------------

void double_click_on_scene::test()
{
    start(
        "double_click_on_scene",
        [](sight::ui::test::tester& _tester)
        {
            // Initial situation: the 3D scene is maximized
            _tester.take(
                std::to_string(__LINE__) + ": ogre scene",
                [&_tester]() -> QObject* {return _tester.get_main_window()->findChild<QWidget*>("sceneSrv");});

            QSize size;

            _tester.doubt<QWidget*>(
                std::to_string(__LINE__) + ": Get initial maximized size",
                [&size](QWidget* _obj)
            {
                size = _obj->size();
                return size.isValid();
            });

            // Double click on the main scene
            _tester.interact(std::make_unique<sight::ui::test::mouse_double_click>());

            // The 3D scene is restored
            _tester.doubt<QWidget*>(
                std::to_string(__LINE__) + ": Check current size < initial size",
                [&size](QWidget* _obj)
            {
                const auto& current_size = _obj->size();
                return current_size.width() * current_size.height() < size.width() * size.height();
            });

            // Double click on the main scene
            _tester.interact(std::make_unique<sight::ui::test::mouse_double_click>());

            // The 3D scene is maximized again
            _tester.doubt<QWidget*>(
                std::to_string(__LINE__) + ": Check current size == initial size",
                [&size](QWidget* _obj)
            {
                return size == _obj->size();
            });
        },
        true
    );
}

} // namespace sight::sight_viewer::uit
