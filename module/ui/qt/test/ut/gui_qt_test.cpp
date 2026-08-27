/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <ui/test/gui_fixture.hpp>

#include <doctest/doctest.h>

#include <QApplication>
#include <QMainWindow>

#include <core/thread/worker.hpp>

TEST_SUITE("sight::module::ui::qt::gui_qt_test")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "default_frame")
    {
        test_service(
            "sight::module::ui::frame",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t frame_config;
                frame_config.put("gui.frame.name", "guiQtUnitTest");
                frame_config.put("gui.frame.min_size.<xmlattr>.width", "800");
                frame_config.put("gui.frame.min_size.<xmlattr>.height", "600");

                _service->set_config(frame_config);
                _service->configure();
                _service->start().wait();
            }).get();

            auto* window = sight::core::thread::get_default_worker()->post_task<QMainWindow*>(
                []
            {
                return qobject_cast<QMainWindow*>(qApp->activeWindow());
            }).get();

            CHECK(window != nullptr);
            CHECK_EQ(std::string("guiQtUnitTest"), window->windowTitle().toStdString());
        });
    }

    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "fullscreen_frame")
    {
        test_service(
            "sight::module::ui::frame",
            [](const sight::service::base::sptr& _service)
        {
            sight::core::thread::get_default_worker()->post_task<void>(
                [_service]
            {
                sight::service::config_t frame_config;
                frame_config.put("gui.frame.name", "gui_qt_test_test_fullscreen_frame");
                frame_config.put("gui.frame.min_size.<xmlattr>.width", "800");
                frame_config.put("gui.frame.min_size.<xmlattr>.height", "600");
#ifndef _WIN32
                frame_config.put("gui.frame.style.<xmlattr>.mode", "FULLSCREEN");
#else
                frame_config.put("gui.frame.style.<xmlattr>.mode", "FRAMELESS");
#endif
                frame_config.put("gui.frame.screen.<xmlattr>.index", "0");

                _service->set_config(frame_config);
                _service->configure();
                _service->start().wait();
            }).get();

            auto* window = sight::core::thread::get_default_worker()->post_task<QMainWindow*>(
                []
            {
                return qobject_cast<QMainWindow*>(qApp->activeWindow());
            }).get();

            CHECK(window != nullptr);
            CHECK_EQ(std::string("gui_qt_test_test_fullscreen_frame"), window->windowTitle().toStdString());
        });
    }
}
