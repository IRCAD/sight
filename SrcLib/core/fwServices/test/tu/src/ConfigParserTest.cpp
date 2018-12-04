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

#include "ConfigParserTest.hpp"

#include "TestServices.hpp"

#include <fwServices/AppConfigManager.hpp>
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
    ::fwServices::AppConfigManager::sptr configManager = ::fwServices::AppConfigManager::New();
    configManager->::fwServices::IAppConfigManager::setConfig( config );
    configManager->create();
    auto image = ::fwData::Image::dynamicCast(configManager->getConfigRoot());

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(objectUUID, image->getID());

    // Test start services
    configManager->start();
    const auto& srv1 = ::fwServices::get(serviceUUID1);
    const auto& srv2 = ::fwServices::get(serviceUUID2);
    CPPUNIT_ASSERT( srv1->isStarted() );
    CPPUNIT_ASSERT( srv2->isStarted() );

    // Test if object's service is created
    CPPUNIT_ASSERT(::fwServices::OSR::isRegistered("data", ::fwServices::IService::AccessType::INPUT, srv1));
    CPPUNIT_ASSERT(image == ::fwServices::OSR::getRegistered("data", ::fwServices::IService::AccessType::INPUT, srv1));

    // Test update services
    configManager->update();
    CPPUNIT_ASSERT( ::fwServices::ut::TestConfigService::dynamicCast( srv1 )->getIsUpdated() );
    CPPUNIT_ASSERT( ::fwServices::ut::TestConfigService::dynamicCast( srv2 )->getIsUpdated() == false );

    // Test stop services
    configManager->stop();
    CPPUNIT_ASSERT( srv1->isStopped() );
    CPPUNIT_ASSERT( srv2->isStopped() );

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
