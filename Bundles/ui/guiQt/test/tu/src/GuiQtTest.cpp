/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwData/String.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/AppConfigManager.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <QApplication>
#include <QMainWindow>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::guiQt::ut::GuiQtTest );

namespace guiQt
{
namespace ut
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

void GuiQtTest::testDefaultFrame()
{
    ::fwData::String::sptr object = ::fwData::String::New();

    ::fwServices::IService::ConfigType frameConfig;

    frameConfig.put("gui.frame.name", "guiQtUnitTest");
    frameConfig.put("gui.frame.minSize.<xmlattr>.width", "800");
    frameConfig.put("gui.frame.minSize.<xmlattr>.height", "600");

    ::fwServices::IService::sptr srv = ::fwServices::add( "::gui::frame::SDefaultFrame" );
    CPPUNIT_ASSERT(srv);

    srv->setConfiguration( frameConfig );
    srv->configure();
    srv->start();

    QMainWindow* window = qobject_cast<QMainWindow*>(qApp->activeWindow());

    CPPUNIT_ASSERT(qApp);
    CPPUNIT_ASSERT(qApp->activeWindow());
    CPPUNIT_ASSERT(window);
    CPPUNIT_ASSERT_EQUAL(std::string("guiQtUnitTest"), window->windowTitle().toStdString());

    srv->stop();
    ::fwServices::OSR::unregisterService( srv );
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace guiQt
