/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include "test.hpp"

#include <QAction>

#include <doctest/doctest.h>

namespace sight::sight_viewer::uit
{

TEST_SUITE("sight_viewer")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(test, "double_click_on_scene")
    {
        const std::string failure_message = start(
            "double_click_on_scene",
            [](sight::ui::test::tester& _tester)
            {
                // Initial situation: the 3D scene is maximized
                _tester.take("main 3D view", "top_scenes_view/1");
                _tester.yields(std::to_string(__LINE__) + ": ogre scene", "scene_srv");

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

        // Runs on the main thread, after start() has returned: the only doctest assertion for
        // this scenario. See sight::ui::test::base::start().
        INFO(failure_message);
        REQUIRE(failure_message.empty());
    }
} // TEST_SUITE

} // namespace sight::sight_viewer::uit
