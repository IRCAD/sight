/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ConfigParserTest.hpp"

#include "TestServices.hpp"

#include <core/runtime/Convert.hpp>

#include <data/Image.hpp>
#include <data/Mesh.hpp>

#include <service/AppConfigManager.hpp>
#include <service/base.hpp>
#include <service/parser/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::ConfigParserTest);

//------------------------------------------------------------------------------

namespace sight::service
{

namespace ut
{

//------------------------------------------------------------------------------

void ConfigParserTest::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::getResourceFilePath("tu_exec_service");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    auto& runtime = core::runtime::Runtime::get();
    runtime.addModules(location);

    core::runtime::loadModule("sight::module::service");
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
    core::runtime::ConfigurationElement::sptr config = buildObjectConfig();

    // Create the object and its services from the configuration
    service::AppConfigManager::sptr configManager = service::AppConfigManager::New();
    configManager->service::IAppConfigManager::setConfig(config);
    configManager->create();
    auto image = data::Image::dynamicCast(configManager->getConfigRoot());

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(objectUUID, image->getID());

    // Test start services
    configManager->start();
    const auto& srv1 = service::get(serviceUUID1);
    const auto& srv2 = service::get(serviceUUID2);
    CPPUNIT_ASSERT(srv1->isStarted());
    CPPUNIT_ASSERT(srv2->isStarted());

    // Test if object's service is created
    CPPUNIT_ASSERT(image == srv1->getObject("data", data::Access::in));

    // Test update services
    configManager->update();
    CPPUNIT_ASSERT(service::ut::TestConfigService::dynamicCast(srv1)->getIsUpdated());
    CPPUNIT_ASSERT(service::ut::TestConfigService::dynamicCast(srv2)->getIsUpdated() == false);

    // Test stop services
    configManager->stop();
    CPPUNIT_ASSERT(srv1->isStopped());
    CPPUNIT_ASSERT(srv2->isStopped());

    configManager->destroy();
}

//------------------------------------------------------------------------------

void ConfigParserTest::testImageParser()
{
    const std::string objectUUID = "objectUUID";
    service::IService::ConfigType config;

    // Configuration on core::tools::Object which uid is objectUUID
    service::IService::ConfigType objCfg;
    objCfg.add("<xmlattr>.uid", objectUUID);
    objCfg.add("<xmlattr>.type", "sight::data::Image");
    objCfg.add("color", "#FF459812");
    config.add_child("object", objCfg);

    service::IService::ConfigType serviceCfg;
    serviceCfg.add_child("config", config);

    // Create object configuration
    const auto cfg = core::runtime::Convert::fromPropertyTree(serviceCfg);

    // Create the object and its services from the configuration
    service::AppConfigManager::sptr configManager = service::AppConfigManager::New();
    configManager->service::IAppConfigManager::setConfig(cfg);
    configManager->create();
    auto image = std::dynamic_pointer_cast<data::Image>(core::tools::fwID::getObject(objectUUID));

    // Test object uid
    CPPUNIT_ASSERT_EQUAL(objectUUID, image->getID());
    CPPUNIT_ASSERT_EQUAL(sight::data::Image::RGBA, image->getPixelFormat());
    CPPUNIT_ASSERT_EQUAL(sight::core::tools::Type::s_UINT8, image->getType());

    // We only test the image content, we do not really care about the image size and other attributes for now
    const auto dumpLock = image->dump_lock();
    auto itr            = image->begin<sight::data::iterator::rgba>();
    const auto itrEnd   = image->end<sight::data::iterator::rgba>();

    for( ; itr != itrEnd ; ++itr)
    {
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0xFF), itr->r);
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0x45), itr->g);
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0x98), itr->b);
        CPPUNIT_ASSERT_EQUAL(std::uint8_t(0X12), itr->a);
    }

    configManager->destroy();
}

//------------------------------------------------------------------------------

core::runtime::ConfigurationElement::sptr ConfigParserTest::buildObjectConfig()
{
    service::IService::ConfigType config;

    // Configuration on core::tools::Object which uid is objectUUID
    service::IService::ConfigType objCfg;
    objCfg.add("<xmlattr>.uid", "objectUUID");
    objCfg.add("<xmlattr>.type", "sight::data::Image");
    config.add_child("object", objCfg);

    // Object's service A
    service::IService::ConfigType serviceA;
    serviceA.add("<xmlattr>.uid", "myTestService1");
    serviceA.add("<xmlattr>.type", "sight::service::ut::STest1Image");

    service::IService::ConfigType dataServiceA;
    dataServiceA.add("<xmlattr>.key", "data");
    dataServiceA.add("<xmlattr>.uid", "objectUUID");
    serviceA.add_child("in", dataServiceA);
    config.add_child("service", serviceA);

    // Object's service B
    service::IService::ConfigType serviceB;
    serviceB.add("<xmlattr>.uid", "myTestService2");
    serviceB.add("<xmlattr>.type", "sight::service::ut::STest1Image");
    config.add_child("service", serviceB);

    // Start method from object's services
    service::IService::ConfigType startA;
    startA.add("<xmlattr>.uid", "myTestService1");
    config.add_child("start", startA);
    service::IService::ConfigType startB;
    startB.add("<xmlattr>.uid", "myTestService2");
    config.add_child("start", startB);

    // Update method from object's services
    service::IService::ConfigType update1;
    update1.add("<xmlattr>.uid", "myTestService1");
    config.add_child("update", update1);

    service::IService::ConfigType serviceCfg;
    serviceCfg.add_child("config", config);

    return core::runtime::Convert::fromPropertyTree(serviceCfg);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::service
