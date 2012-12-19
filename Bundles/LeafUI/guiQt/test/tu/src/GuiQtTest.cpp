/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QMainWindow>

#include <fwData/String.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwGuiQt/App.hpp>


#include "GuiQtTest.hpp"

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
    ::fwData::String::NewSptr object;

    ::fwServices::IService::ConfigType frameConfig;

    frameConfig.put("service.gui.frame.name", "guiQtUnitTest");
    frameConfig.put("service.gui.frame.minSize.<xmlattr>.width", "800");
    frameConfig.put("service.gui.frame.minSize.<xmlattr>.height", "600");

    ::fwServices::IService::sptr srv;
    srv = ::fwServices::registry::ServiceFactory::getDefault()->create(  "::fwGui::IFrameSrv", "::gui::frame::DefaultFrame" );
    CPPUNIT_ASSERT(srv);

    ::fwServices::OSR::registerService( object , srv );

    srv->setConfiguration( frameConfig ) ;
    srv->configure();
    srv->start();

    QMainWindow *window = qobject_cast<QMainWindow *>(qApp->activeWindow());

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
    ::fwServices::AppConfigManager::sptr appConfigMng = ::fwServices::AppConfigManager::New();
    ::fwRuntime::ConfigurationElement::csptr config = ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( "tutoBasicConfig" );
    appConfigMng->setConfig( ::fwRuntime::ConfigurationElement::constCast( config ) );
    appConfigMng->launch();
    appConfigMng->stopAndDestroy();
}

//------------------------------------------------------------------------------

void GuiQtTest::testTuto02()
{
    ::fwServices::AppConfigManager::sptr appConfigMng = ::fwServices::AppConfigManager::New();
    ::fwRuntime::ConfigurationElement::csptr config = ::fwServices::registry::AppConfig::getDefault()->getAdaptedTemplateConfig( "tutoDataServiceBasicConfig" );
    appConfigMng->setConfig( ::fwRuntime::ConfigurationElement::constCast( config ) );
    appConfigMng->launch();
    appConfigMng->stopAndDestroy();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace guiQt
