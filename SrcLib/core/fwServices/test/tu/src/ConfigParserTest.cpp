/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
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

#include <fwRuntime/Convert.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ConfigParserTest );

//------------------------------------------------------------------------------

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

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
    ::fwServices::IService::ConfigType config;

    // Configuration on fwTools::Object which uid is objectUUID
    ::fwServices::IService::ConfigType objCfg;
    objCfg.add("<xmlattr>.uid", "objectUUID");
    objCfg.add("<xmlattr>.type", "::fwData::Image");
    config.add_child("object", objCfg);

    // Object's service A
    ::fwServices::IService::ConfigType serviceA;
    serviceA.add("<xmlattr>.uid", "myTestService1");
    serviceA.add("<xmlattr>.type", "::fwServices::ut::TestServiceImplementationImage");

    ::fwServices::IService::ConfigType dataServiceA;
    dataServiceA.add("<xmlattr>.key", "data");
    dataServiceA.add("<xmlattr>.uid", "objectUUID");
    serviceA.add_child("in", dataServiceA);
    config.add_child("service", serviceA);

    // Object's service B
    ::fwServices::IService::ConfigType serviceB;
    serviceB.add("<xmlattr>.uid", "myTestService2");
    serviceB.add("<xmlattr>.type", "::fwServices::ut::TestServiceImplementationImage");
    config.add_child("service", serviceB);

    // Start method from object's services
    ::fwServices::IService::ConfigType startA;
    startA.add("<xmlattr>.uid", "myTestService1");
    config.add_child("start", startA);
    ::fwServices::IService::ConfigType startB;
    startB.add("<xmlattr>.uid", "myTestService2");
    config.add_child("start", startB);

    // Update method from object's services
    ::fwServices::IService::ConfigType update1;
    update1.add("<xmlattr>.uid", "myTestService1");
    config.add_child("update", update1);

    ::fwServices::IService::ConfigType serviceCfg;
    serviceCfg.add_child("config", config);

    return ::fwRuntime::Convert::fromPropertyTree(serviceCfg);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
