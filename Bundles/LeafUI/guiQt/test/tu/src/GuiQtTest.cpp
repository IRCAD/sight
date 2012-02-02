/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QApplication>
#include <QMainWindow>

#include <fwData/Object.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwGuiQt/App.hpp>


#include "GuiQtTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( GuiQtTest );

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
    ::fwData::Object::NewSptr object;

    ::fwRuntime::EConfigurationElement::NewSptr frameCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr guiCfg("gui");
    ::fwRuntime::EConfigurationElement::NewSptr guiFrameCfg("frame");

    ::fwRuntime::EConfigurationElement::NewSptr guiFrameNameCfg("name");
    guiFrameNameCfg->setValue("guiQtUnitTest");
    ::fwRuntime::EConfigurationElement::NewSptr guiFrameMinSizeCfg("minSize");
    guiFrameMinSizeCfg->setAttributeValue("width", "800");
    guiFrameMinSizeCfg->setAttributeValue("height", "600");

    guiFrameCfg->addConfigurationElement(guiFrameNameCfg);
    guiFrameCfg->addConfigurationElement(guiFrameMinSizeCfg);
    guiCfg->addConfigurationElement(guiFrameCfg);

    frameCfg->addConfigurationElement(guiCfg);


    ::fwServices::IService::sptr srv = ::fwServices::registry::ServiceFactory::getDefault()->create(  "::fwGui::IFrameSrv", "::gui::frame::DefaultFrame" );
    CPPUNIT_ASSERT(srv);

    ::fwServices::OSR::registerService( object , srv );

    srv->setConfiguration( frameCfg ) ;
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
    ::fwRuntime::ConfigurationElement::csptr config = ::fwServices::registry::AppConfig::getDefault()->getStandardConfig( "tutoBasicConfig" );
    appConfigMng->setConfig( ::fwRuntime::ConfigurationElement::constCast( config ) );
    appConfigMng->launch();
    appConfigMng->stopAndDestroy();
}

//------------------------------------------------------------------------------

void GuiQtTest::testTuto02()
{
    ::fwServices::AppConfigManager::sptr appConfigMng = ::fwServices::AppConfigManager::New();
    ::fwRuntime::ConfigurationElement::csptr config = ::fwServices::registry::AppConfig::getDefault()->getStandardConfig( "tutoDataServiceBasicConfig" );
    appConfigMng->setConfig( ::fwRuntime::ConfigurationElement::constCast( config ) );
    appConfigMng->launch();
    appConfigMng->stopAndDestroy();
}

//------------------------------------------------------------------------------
