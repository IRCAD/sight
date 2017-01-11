/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ConfigParserTest.hpp"

#include "TestServices.hpp"

#include <fwServices/AppConfigManager2.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ConfigParserTest );

//------------------------------------------------------------------------------

namespace fwServices
{
namespace ut
{

void ConfigParserTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ConfigParserTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ConfigParserTest::testObjectCreationWithConfig()
{
    const std::string objectUUID   = "objectUUID";
    const std::string serviceUUID1 = "myTestService1";
    const std::string serviceUUID2 = "myTestService2";

    // Create object configuration
    ::fwRuntime::ConfigurationElement::sptr config = buildObjectConfig();

    // Create the object and its services from the configuration
    ::fwServices::AppConfigManager2::sptr configManager = ::fwServices::AppConfigManager2::New();
    configManager->::fwServices::IAppConfigManager::setConfig( config );
    configManager->create();
    auto image = ::fwData::Image::dynamicCast(configManager->getConfigRoot());

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(objectUUID, image->getID());

    // Test if object's service is created
    CPPUNIT_ASSERT( ::fwServices::OSR::has(image, "::fwServices::ut::TestConfigService"));

    // Test start services
    configManager->start();
    CPPUNIT_ASSERT( ::fwServices::get(serviceUUID1)->isStarted() );
    CPPUNIT_ASSERT( ::fwServices::get(serviceUUID2)->isStarted() );

    // Test update services
    configManager->update();
    CPPUNIT_ASSERT( ::fwServices::ut::TestConfigService::dynamicCast( ::fwServices::get(serviceUUID1) )->getIsUpdated() );
    CPPUNIT_ASSERT( ::fwServices::ut::TestConfigService::dynamicCast( ::fwServices::get(
                                                                          serviceUUID2) )->getIsUpdated() ==
                    false );

    // Test stop services
    configManager->stop();
    CPPUNIT_ASSERT( ::fwServices::get(serviceUUID1)->isStopped() );
    CPPUNIT_ASSERT( ::fwServices::get(serviceUUID2)->isStopped() );

    configManager->destroy();
}

//------------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr ConfigParserTest::buildObjectConfig()
{
    std::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("config"));

    // Configuration on fwTools::Object which uid is objectUUID
    std::shared_ptr< ::fwRuntime::EConfigurationElement > objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "objectUUID");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    // Object's service A
    std::shared_ptr< ::fwRuntime::EConfigurationElement > serviceA = cfg->addConfigurationElement("service");
    serviceA->setAttributeValue( "uid", "myTestService1" );
    serviceA->setAttributeValue( "type", "::fwServices::ut::TestServiceImplementationImage" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataServiceA = serviceA->addConfigurationElement("in");
    dataServiceA->setAttributeValue( "key", "data" );
    dataServiceA->setAttributeValue( "uid", "objectUUID" );

    // Object's service B
    std::shared_ptr< ::fwRuntime::EConfigurationElement > serviceB = cfg->addConfigurationElement("service");
    serviceB->setAttributeValue( "uid", "myTestService2" );
    serviceB->setAttributeValue( "type", "::fwServices::ut::TestServiceImplementationImage" );

    // Start method from object's services
    std::shared_ptr< ::fwRuntime::EConfigurationElement > startA = cfg->addConfigurationElement("start");
    startA->setAttributeValue( "uid", "myTestService1" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > startB = cfg->addConfigurationElement("start");
    startB->setAttributeValue( "uid", "myTestService2" );

    // Update method from object's services
    std::shared_ptr< ::fwRuntime::EConfigurationElement > updateA = cfg->addConfigurationElement("update");
    updateA->setAttributeValue( "uid", "myTestService1" );

    return cfg;
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
