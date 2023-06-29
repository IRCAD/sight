/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "MultiConfigControllerTest.hpp"

#include "TestService.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>
#include <core/service/base.hpp>

#include <service/extension/AppConfig.hpp>
#include <service/extension/AppConfigParameters.hpp>

#include <ui/base/parameter.hpp>

#include <utest/wait.hpp>

#include <boost/config.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::MultiConfigControllerTest);

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void MultiConfigControllerTest::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::getResourceFilePath("tu_exec_service");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::addModules(location);
    core::runtime::loadModule("sight::module::service");
}

//------------------------------------------------------------------------------

void MultiConfigControllerTest::tearDown()
{
    // Clean up after the test run.
    // unregister the services that have not been unregistered because a test failed.

    auto services = service::getServices<service::IService>();
    for(const auto& srv : services)
    {
        if(srv->isStarted())
        {
            srv->stop().wait();
        }

        service::unregisterService(srv);
    }
}

//------------------------------------------------------------------------------

void MultiConfigControllerTest::multiConfigTest()
{
    // Initialise the testing service

    std::stringstream config_string;
    config_string << " <appConfig key=\"myKey\" default=\"config1\">"
                     "<config name=\"config1\" id=\"connectionTest\" />"
                     "<config name=\"config2\" id=\"startStopConnectionTest\" />"
                     "</appConfig>";
    service::IService::sptr srv = service::add("sight::service::SMultiConfigController");

    service::IService::ConfigType config;
    boost::property_tree::read_xml(config_string, config);
    srv->setConfiguration(config);
    srv->configure();
    srv->start().wait();

    // create connection to wait the end of the update
    bool serviceUpdated     = false;
    auto slotServiceUpdated = sight::core::com::newSlot(
        [&serviceUpdated]()
        {
            serviceUpdated = true;
        });
    slotServiceUpdated->setWorker(sight::core::thread::getDefaultWorker());
    auto serviceUpdatedConnection = srv->signal("updated")->connect(
        slotServiceUpdated
    );

    // start the test!
    // set config2 =>
    //  connectionTest_2_SGenerateData : not existing
    //  startStopConnectionTest_2_TestService5Uid : started

    srv->slot("setConfig")->run(ui::base::parameter_t("config2"), std::string("myKey"));
    fwTestWithFailWaitMacro(serviceUpdated);
    serviceUpdated = false;

    auto genDataSrv =
        service::ut::TestService::dynamicCast(core::tools::fwID::getObject("connectionTest_6_SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv == nullptr);
    auto noDataSrv =
        service::ut::TestService::dynamicCast(core::tools::fwID::getObject("startStopConnectionTest_6_TestService5Uid"));
    CPPUNIT_ASSERT(noDataSrv != nullptr && noDataSrv->isStarted());

    // set config1 =>
    //  connectionTest_2_SGenerateData : started
    //  startStopConnectionTest_2_TestService5Uid : stopped
    srv->slot("setConfig")->run(ui::base::parameter_t("config1"), std::string("myKey"));
    fwTestWithFailWaitMacro(serviceUpdated);
    serviceUpdated = false;

    genDataSrv = service::ut::TestService::dynamicCast(core::tools::fwID::getObject("connectionTest_8_SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr && genDataSrv->isStarted());
    noDataSrv =
        service::ut::TestService::dynamicCast(core::tools::fwID::getObject("startStopConnectionTest_6_TestService5Uid"));
    CPPUNIT_ASSERT(noDataSrv != nullptr && noDataSrv->isStopped());

    // set config2 =>
    //  connectionTest_2_SGenerateData : stopped
    //  startStopConnectionTest_2_TestService5Uid : started
    srv->slot("setConfig")->run(ui::base::parameter_t("config2"), std::string("myKey"));
    fwTestWithFailWaitMacro(serviceUpdated);
    serviceUpdated = false;

    genDataSrv = service::ut::TestService::dynamicCast(core::tools::fwID::getObject("connectionTest_8_SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr && genDataSrv->isStopped());
    noDataSrv =
        service::ut::TestService::dynamicCast(core::tools::fwID::getObject("startStopConnectionTest_10_TestService5Uid"));
    CPPUNIT_ASSERT(noDataSrv != nullptr && noDataSrv->isStarted());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

void MultiConfigControllerTest::setConfigKeyTest()
{
    // Initialise the testing service

    std::stringstream config_string;
    config_string << " <appConfig key=\"myKey\" default=\"config1\">"
                     "<config name=\"config1\" id=\"connectionTest\" />"
                     "<config name=\"config2\" id=\"startStopConnectionTest\" />"
                     "</appConfig>";
    service::IService::sptr srv = service::add("sight::service::SMultiConfigController");

    service::IService::ConfigType config;
    boost::property_tree::read_xml(config_string, config);
    srv->setConfiguration(config);
    srv->configure();
    srv->start().wait();

    // create connection to wait the end of the update
    bool serviceUpdated     = false;
    auto slotServiceUpdated = sight::core::com::newSlot(
        [&serviceUpdated]()
        {
            serviceUpdated = true;
        });
    slotServiceUpdated->setWorker(sight::core::thread::getDefaultWorker());
    auto serviceUpdatedConnection = srv->signal("updated")->connect(
        slotServiceUpdated
    );

    // start the test!
    // set config2 =>
    //  connectionTest_2_SGenerateData : not existing
    //  startStopConnectionTest_2_TestService5Uid : started

    srv->slot("setConfig")->run(ui::base::parameter_t("config2"), std::string("myKey"));
    fwTestWithFailWaitMacro(serviceUpdated);
    serviceUpdated = false;

    auto genDataSrv =
        service::ut::TestService::dynamicCast(core::tools::fwID::getObject("connectionTest_12_SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv == nullptr);
    auto noDataSrv =
        service::ut::TestService::dynamicCast(core::tools::fwID::getObject("startStopConnectionTest_12_TestService5Uid"));
    CPPUNIT_ASSERT(noDataSrv != nullptr && noDataSrv->isStarted());

    // set config with bad key
    srv->slot("setConfig")->run(ui::base::parameter_t("config2"), std::string("badKey"));
    srv->slot("update")->run();
    fwTestWithFailWaitMacro(serviceUpdated);
    serviceUpdated = false;

    genDataSrv = service::ut::TestService::dynamicCast(core::tools::fwID::getObject("connectionTest_14_SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv == nullptr);
    noDataSrv =
        service::ut::TestService::dynamicCast(core::tools::fwID::getObject("startStopConnectionTest_14_TestService5Uid"));
    CPPUNIT_ASSERT(noDataSrv != nullptr && noDataSrv->isStarted());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

void MultiConfigControllerTest::defaultLoadingTest()
{
    std::stringstream config_string;
    config_string << " <appConfig key=\"myKey\" default=\"config1\">"
                     "<config name=\"config1\" id=\"connectionTest\" />"
                     "<config name=\"config2\" id=\"startStopConnectionTest\" />"
                     "</appConfig>";
    service::IService::sptr srv = service::add("sight::service::SMultiConfigController");

    service::IService::ConfigType config;
    boost::property_tree::read_xml(config_string, config);
    srv->setConfiguration(config);

    srv->configure();
    srv->start().wait();
    srv->update().wait();

    auto genDataSrv =
        service::ut::TestService::dynamicCast(core::tools::fwID::getObject("connectionTest_16_SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr && genDataSrv->isStarted());

    srv->update().wait();
    genDataSrv = service::ut::TestService::dynamicCast(core::tools::fwID::getObject("connectionTest_18_SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr && genDataSrv->isStarted());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
