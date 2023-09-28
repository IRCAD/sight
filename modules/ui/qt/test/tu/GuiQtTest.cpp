/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "GuiQtTest.hpp"

#include <data/String.hpp>

#include <service/op/Add.hpp>

#include <QApplication>
#include <QMainWindow>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::GuiQtTest);

namespace sight::module::ui::qt::ut
{

//------------------------------------------------------------------------------

void GuiQtTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void GuiQtTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

#define ASSERT_NOT_NULL(expr) if((expr) == nullptr){throw std::runtime_error(#expr " is null.");}

//------------------------------------------------------------------------------

void GuiQtTest::testDefaultFrame()
{
    data::String::sptr object = std::make_shared<data::String>();

    service::config_t frameConfig;

    frameConfig.put("gui.frame.name", "guiQtUnitTest");
    frameConfig.put("gui.frame.minSize.<xmlattr>.width", "800");
    frameConfig.put("gui.frame.minSize.<xmlattr>.height", "600");

    service::base::sptr srv = service::add("sight::module::ui::SFrame");
    ASSERT_NOT_NULL(srv);

    srv->setConfiguration(frameConfig);
    srv->configure();
    srv->start();

    auto* window = qobject_cast<QMainWindow*>(qApp->activeWindow());

    ASSERT_NOT_NULL(qApp);
    ASSERT_NOT_NULL(qApp->activeWindow());
    ASSERT_NOT_NULL(window);
    CPPUNIT_ASSERT_EQUAL(std::string("guiQtUnitTest"), window->windowTitle().toStdString());

    srv->stop();
    service::unregisterService(srv);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut
