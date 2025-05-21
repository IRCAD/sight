/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "gui_qt_test.hpp"

#include <data/string.hpp>

#include <service/op.hpp>

#include <QApplication>
#include <QMainWindow>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::gui_qt_test);

namespace sight::module::ui::qt::ut
{

//------------------------------------------------------------------------------

void gui_qt_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void gui_qt_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

#define ASSERT_NOT_NULL(expr) if((expr) == nullptr){throw std::runtime_error(#expr " is null.");}

//------------------------------------------------------------------------------

void gui_qt_test::test_default_frame()
{
    data::string::sptr object = std::make_shared<data::string>();

    service::config_t frame_config;

    frame_config.put("gui.frame.name", "guiQtUnitTest");
    frame_config.put("gui.frame.minSize.<xmlattr>.width", "800");
    frame_config.put("gui.frame.minSize.<xmlattr>.height", "600");

    service::base::sptr srv = service::add("sight::module::ui::frame");
    ASSERT_NOT_NULL(srv);

    srv->set_config(frame_config);
    srv->configure();
    srv->start();

    auto* window = qobject_cast<QMainWindow*>(qApp->activeWindow());

    ASSERT_NOT_NULL(qApp);
    ASSERT_NOT_NULL(qApp->activeWindow());
    ASSERT_NOT_NULL(window);
    CPPUNIT_ASSERT_EQUAL(std::string("guiQtUnitTest"), window->windowTitle().toStdString());

    srv->stop();
    service::unregister_service(srv);
}

//------------------------------------------------------------------------------

void gui_qt_test::test_fullscreen_frame()
{
    data::string::sptr object = std::make_shared<data::string>();

    service::config_t frame_config;

    frame_config.put("gui.frame.name", "gui_qt_test_test_fullscreen_frame");
    frame_config.put("gui.frame.minSize.<xmlattr>.width", "800");
    frame_config.put("gui.frame.minSize.<xmlattr>.height", "600");
#ifndef _WIN32
    frame_config.put("gui.frame.style.<xmlattr>.mode", "FULLSCREEN");
#else
    frame_config.put("gui.frame.style.<xmlattr>.mode", "FRAMELESS");
#endif
    frame_config.put("gui.frame.screen.<xmlattr>.index", "0");

    service::base::sptr srv = service::add("sight::module::ui::frame");
    ASSERT_NOT_NULL(srv);

    srv->set_config(frame_config);
    srv->configure();
    srv->start();

    auto* window = qobject_cast<QMainWindow*>(qApp->activeWindow());

    ASSERT_NOT_NULL(qApp);
    ASSERT_NOT_NULL(qApp->activeWindow());
    ASSERT_NOT_NULL(window);
    CPPUNIT_ASSERT_EQUAL(std::string("guiQtUnitTest"), window->windowTitle().toStdString());

    srv->stop();
    service::unregister_service(srv);
}

} // namespace sight::module::ui::qt::ut
