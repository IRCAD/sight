/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

void GuiQtTest::testTuto01()
{
    std::vector<std::string> cfgs = ::fwServices::registry::AppConfig::getDefault()->getAllConfigs();
    CPPUNIT_ASSERT_MESSAGE("Missing available configuration", !cfgs.empty());

    ::fwServices::AppConfigManager::sptr appConfigMng = ::fwServices::AppConfigManager::New();
    appConfigMng->setConfig( "tutoBasicConfig" );

    appConfigMng->launch();
    appConfigMng->stopAndDestroy();
}

//------------------------------------------------------------------------------

void GuiQtTest::testTuto02()
{
    std::vector<std::string> cfgs = ::fwServices::registry::AppConfig::getDefault()->getAllConfigs();
    CPPUNIT_ASSERT_MESSAGE("Missing available configuration", !cfgs.empty());

    ::fwServices::AppConfigManager::sptr appConfigMng = ::fwServices::AppConfigManager::New();
    appConfigMng->setConfig( "tutoDataServiceBasicConfig" );

    appConfigMng->launch();
    appConfigMng->stopAndDestroy();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace guiQt
