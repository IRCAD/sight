/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "config_test.hpp"

#include "core/com/slot.hxx"

#include "service/extension/config.hpp"
#include "service/registry.hpp"

#include "test_service.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/runtime/helper.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>
#include <core/time_stamp.hpp>

#include <data/boolean.hpp>
#include <data/image.hpp>
#include <data/string.hpp>

#include <utest/wait.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::ut::config_test);

namespace sight::app::ut
{

//------------------------------------------------------------------------------

static inline void wait_service_started(const std::string& srv)
{
    auto service = core::tools::id::get_object(srv);
    SIGHT_TEST_WAIT(
        service != nullptr
        && std::dynamic_pointer_cast<service::base>(service)->started() == service::base::STARTED
    );
}

//------------------------------------------------------------------------------

void config_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("tu_exec_app");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
    core::runtime::load_module("config_test");

    app::extension::config::sptr appConfig = app::extension::config::getDefault();
    appConfig->clear_registry();
    appConfig->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void config_test::tearDown()
{
    // Clean up after the test run.
    if(m_appConfigMgr)
    {
        // If everything went well, the manager should have been destroyed
        // This means a test failed, thus we need to clean everything properly, otherwise
        // We will get an assert from the destructor and we will not get the cppunit report in the console
        m_appConfigMgr->stopAndDestroy();
        m_appConfigMgr = nullptr;
    }
}

//------------------------------------------------------------------------------

void config_test::addConfigTest()
{
    app::extension::config::sptr currentAppConfig = app::extension::config::getDefault();

    const std::string configId(app::extension::config::getUniqueIdentifier());
    const std::string group("TestGroup");
    const std::string desc("Description");
    const std::string moduleId("sight::module::service");
    app::extension::app_info::ParametersType parameters;

    const service::config_t config = buildConfig();

    currentAppConfig->addapp_info(configId, group, desc, parameters, config, moduleId);

    std::vector<std::string> allConfigs = currentAppConfig->getAllConfigs();
    CPPUNIT_ASSERT_EQUAL(false, allConfigs.empty());

    std::vector<std::string> configs = currentAppConfig->getConfigsFromGroup(group);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), configs.size());
    CPPUNIT_ASSERT_EQUAL(configId, configs.front());

    auto module = currentAppConfig->get_module(configId);
    CPPUNIT_ASSERT(module);
    CPPUNIT_ASSERT_EQUAL(moduleId, module->identifier());

    app::field_adaptor_t replaceFields;

    core::runtime::config_t configAdapted = currentAppConfig->getAdaptedTemplateConfig(configId, replaceFields, false);
    const auto uid                        = configAdapted.get<std::string>("object.<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("image"), uid);

    const auto serviceUid1 = configAdapted.get<std::string>("service.<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("myTestService1"), serviceUid1);
}

//------------------------------------------------------------------------------

void config_test::parametersConfigTest()
{
    app::extension::config::sptr currentAppConfig = app::extension::config::getDefault();

    const std::string configId("parametersConfigTest1");
    const std::string group("parametersGroup");

    app::field_adaptor_t replaceFields;
    replaceFields["TEST_IMAGE"] = "objectUUID";

    std::vector<std::string> allConfigs = currentAppConfig->getAllConfigs();
    auto it                             = std::find(allConfigs.begin(), allConfigs.end(), configId);
    CPPUNIT_ASSERT(it != allConfigs.end());

    core::runtime::config_t configAdapted = currentAppConfig->getAdaptedTemplateConfig(configId, replaceFields, false);

    const auto uid = configAdapted.get<std::string>("object.<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("objectUUID"), uid);

    auto servicesCfg = configAdapted.equal_range("service");

    const auto serviceUid1 = servicesCfg.first->second.get<std::string>("<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("myTestService1"), serviceUid1);

    servicesCfg.first++;
    const auto serviceUid2 = servicesCfg.first->second.get<std::string>("<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("myTestService2"), serviceUid2);
}

//------------------------------------------------------------------------------

app::config_manager::sptr config_test::launchAppConfigMgr(
    const std::string& name,
    bool autoPrefix
)
{
    auto appConfigMgr = app::config_manager::make();

    const app::field_adaptor_t fields;
    appConfigMgr->setConfig(name, fields, autoPrefix);
    appConfigMgr->launch();

    return appConfigMgr;
}

//------------------------------------------------------------------------------

void config_test::startStopTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("startStopTest");

    // =================================================================================================================
    // Test manual start and stop of services, with or without data
    // =================================================================================================================

    auto data1 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // This service doesn't exist in the config
    CPPUNIT_ASSERT(core::tools::id::get_object("TestService142Uid") == nullptr);

    auto genDataSrv = std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    // This service has no data and is started by the config
    {
        core::tools::object::sptr gn_srv1 = core::tools::id::get_object("TestService1Uid");
        auto srv1                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv1->status());
    }

    // This service has no data and is NOT started by the config
    {
        core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
        auto srv2                         = std::dynamic_pointer_cast<service::base>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv2->status());
        srv2->stop().wait();
        CPPUNIT_ASSERT_EQUAL(service::base::STOPPED, srv2->status());
        srv2->start().wait();
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv2->status());
    }

    // This service has a data and is NOT started by the config
    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3                         = std::dynamic_pointer_cast<service::base>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv3->status());
    }

    // This service has a data that is not present yet (WID), so it is even not created
    {
        core::tools::object::sptr gn_srv4 = core::tools::id::get_object("TestService4Uid");
        CPPUNIT_ASSERT(gn_srv4 == nullptr);
    }

    // =================================================================================================================
    // Test automatic start and stop of the service, with a single data
    // =================================================================================================================

    // Create the data
    data::boolean::sptr data2 = std::make_shared<data::boolean>();
    {
        genDataSrv->set_output("out2", data2);
        wait_service_started("TestService4Uid");

        // Now the service should have been started automatically
        {
            auto gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4    = std::dynamic_pointer_cast<service::base>(gn_srv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv4->status());
        }

        // Remove the data
        genDataSrv->set_output("out2", nullptr);
        SIGHT_TEST_WAIT(core::tools::id::exist("TestService4Uid") == false);

        // Now the service should have been stopped and destroyed automatically
        {
            auto gn_srv4 = core::tools::id::get_object("TestService4Uid");
            CPPUNIT_ASSERT(gn_srv4 == nullptr);
        }

        // Register the data once again
        genDataSrv->set_output("out2", data2);
        wait_service_started("TestService4Uid");

        // Check again that the service was started automatically
        {
            auto gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4    = std::dynamic_pointer_cast<service::base>(gn_srv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv4->status());
        }
    }

    // =================================================================================================================
    // Test automatic start and stop of the service, with multiple data (3)
    // =================================================================================================================

    {
        // Still one data is not yet available thus the service is not created
        {
            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            CPPUNIT_ASSERT(gn_srv5 == nullptr);
        }

        // Create the remaining data
        data::boolean::sptr data4 = std::make_shared<data::boolean>();

        genDataSrv->set_output("out4", data4);
        wait_service_started("TestService5Uid");

        // Now the service should have been started automatically
        {
            auto gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5    = std::dynamic_pointer_cast<service::base>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv5->status());
        }

        // Remove one data
        genDataSrv->set_output("out2", nullptr);

        // Now the service should have been stopped and destroyed automatically
        {
            SIGHT_TEST_WAIT(core::tools::id::exist("TestService5Uid") == false);
            auto gn_srv5 = core::tools::id::get_object("TestService5Uid");
            CPPUNIT_ASSERT(gn_srv5 == nullptr);

            // Test as well service 4, just to be sure
            SIGHT_TEST_WAIT(core::tools::id::exist("TestService4Uid") == false);
            auto gn_srv4 = core::tools::id::get_object("TestService4Uid");
            CPPUNIT_ASSERT(gn_srv4 == nullptr);
        }

        // Put everything back
        app::ut::test_service::s_START_COUNTER  = 0;
        app::ut::test_service::s_UPDATE_COUNTER = 0;
        genDataSrv->set_output("out2", data2);
        wait_service_started("TestService5Uid");

        // Now the service should have been started automatically, check start order as well
        {
            auto gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5    = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv5->status());

            // We have started, yet we may not have been updated, so wait for it just in case
            SIGHT_TEST_WAIT(1U == srv5->getUpdateOrder());
            CPPUNIT_ASSERT_EQUAL(0U, srv5->getStartOrder());
            CPPUNIT_ASSERT_EQUAL(1U, srv5->getUpdateOrder());

            // Test as well service 4, just to be sure
            auto gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4    = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv4);
            CPPUNIT_ASSERT(gn_srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(1U, srv4->getStartOrder());
            CPPUNIT_ASSERT_EQUAL(0U, srv4->getUpdateOrder());
        }
    }

    // =================================================================================================================
    // Test data swapping
    // =================================================================================================================

    {
        // Test initial status (started because of the previous test)
        {
            auto gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5    = std::dynamic_pointer_cast<service::base>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv5->status());
        }

        // Swap the data
        data::boolean::sptr data5 = std::make_shared<data::boolean>();

        genDataSrv->set_output("out2", nullptr);
        SIGHT_TEST_WAIT(core::tools::id::exist("TestService5Uid") == false);
        genDataSrv->set_output("out2", data5);
        wait_service_started("TestService5Uid");

        {
            auto gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5    = std::dynamic_pointer_cast<service::base>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv5->status());
        }
    }

    m_appConfigMgr->stopAndDestroy();

    m_appConfigMgr = nullptr;
}

//------------------------------------------------------------------------------

void config_test::autoConnectTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("autoConnectTest");

    // =================================================================================================================
    // Test autoconnect with available data
    // =================================================================================================================

    auto data1 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data1Id"));
    auto data2 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data2Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    {
        core::tools::object::sptr gn_srv1 = core::tools::id::get_object("TestService1Uid");
        auto srv1                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv1->status());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());

        core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
        auto srv2                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv2->status());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3                         = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv3->status());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getReceived());

        auto sig1 = data1->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig1->async_emit();

        SIGHT_TEST_WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getReceived());

        srv2->resetIsUpdated();
        srv3->resetIsUpdated();

        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig2 = data2->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig2->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated() && srv3->getReceived());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getReceived());
    }

    // =================================================================================================================
    // Test autoconnect with unavailable data at start
    // =================================================================================================================

    // Service used to generate data
    auto genDataSrv = std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);
    {
        // Check that dependent services are not created
        {
            core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService4Uid");
            CPPUNIT_ASSERT(gn_srv3 == nullptr);
            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            CPPUNIT_ASSERT(gn_srv5 == nullptr);
        }

        // Create the data
        data::boolean::sptr data3 = std::make_shared<data::boolean>();
        genDataSrv->set_output("out3", data3);

        wait_service_started("TestService4Uid");
        wait_service_started("TestService5Uid");
        {
            core::tools::object::sptr gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv4->status());
            CPPUNIT_ASSERT(!srv4->getIsUpdated());

            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5                         = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv5->status());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getReceived());

            auto sig = data3->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
            sig->async_emit();
            SIGHT_TEST_WAIT(srv5->getReceived());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(srv5->getReceived());
        }

        // Remove one data
        genDataSrv->set_output("out3", nullptr);
        SIGHT_TEST_WAIT(
            core::tools::id::exist("TestService4Uid") == false
            && core::tools::id::exist("TestService5Uid") == false
        );
        {
            core::tools::object::sptr gn_srv4 = core::tools::id::get_object("TestService4Uid");
            CPPUNIT_ASSERT(gn_srv4 == nullptr);
            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            CPPUNIT_ASSERT(gn_srv5 == nullptr);
        }

        // Emit, that should be ok
        genDataSrv->set_output("out3", data3);

        wait_service_started("TestService4Uid");
        wait_service_started("TestService5Uid");
        {
            core::tools::object::sptr gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv4->status());
            CPPUNIT_ASSERT(!srv4->getIsUpdated());

            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5                         = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv5->status());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getReceived());

            auto sig = data3->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
            sig->async_emit();
            SIGHT_TEST_WAIT(srv5->getReceived());
            CPPUNIT_ASSERT(srv5->getReceived());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(srv5->getReceived());

            auto sig1 = data1->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
            sig1->async_emit();
            SIGHT_TEST_WAIT(srv5->getIsUpdated());

            CPPUNIT_ASSERT(srv5->getIsUpdated());
        }
    }
}

//------------------------------------------------------------------------------

void config_test::connectionTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("connectionTest");

    // =================================================================================================================
    // Test connection without data
    // =================================================================================================================

    data::composite::sptr composite;

    auto data1 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test connection without data or with available data
    // =================================================================================================================

    core::tools::object::sptr gn_srv1 = core::tools::id::get_object("TestService1Uid");
    auto srv1                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv1);
    CPPUNIT_ASSERT(srv1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv1->status());
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
    auto srv2                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv2);
    CPPUNIT_ASSERT(srv2 != nullptr);
    CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv2->status());
    CPPUNIT_ASSERT(!srv2->getIsUpdated());

    core::tools::object::sptr gn_srv4 = core::tools::id::get_object("TestService4Uid");
    auto srv4                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv4);
    CPPUNIT_ASSERT(srv4 != nullptr);
    SIGHT_TEST_WAIT(service::base::STARTED == srv4->status());
    CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv4->status());

    // Check connection
    auto sig = data1->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig->async_emit();
    SIGHT_TEST_WAIT(srv1->getIsUpdated() && srv2->getIsUpdated());

    CPPUNIT_ASSERT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv2->getIsUpdated());

    // Service used to generate data
    auto genDataSrv = std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    // =================================================================================================================
    // Test connection with unavailable data at start
    // =================================================================================================================

    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        CPPUNIT_ASSERT(gn_srv3 == nullptr);
    }

    // Emit a signal just for fun, anyway the service doesn't exist
    sig = data1->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig->emit();

    srv2->resetIsUpdated();
    CPPUNIT_ASSERT(!srv2->getIsUpdated());

    // Check connection data4 -> srv2
    auto data4 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data4Id"));
    CPPUNIT_ASSERT(data4 != nullptr);
    auto sig4 = data4->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig4->async_emit();
    SIGHT_TEST_WAIT(srv2->getIsUpdated());

    CPPUNIT_ASSERT(srv2->getIsUpdated());

    SIGHT_TEST_WAIT(!srv4->getIsUpdated());
    CPPUNIT_ASSERT(!srv4->getIsUpdated());
    CPPUNIT_ASSERT(!srv4->getIsUpdated2());

    // Create the missing data
    data::boolean::sptr data2 = std::make_shared<data::boolean>();
    data::boolean::sptr data3 = std::make_shared<data::boolean>();
    genDataSrv->set_output("out2", data2);
    genDataSrv->set_output("out3", data3);
    wait_service_started("TestService3Uid");
    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        SIGHT_TEST_WAIT(service::base::STARTED == srv3->status());
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv3->status());
        srv2->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        // Check "started" signal
        SIGHT_TEST_WAIT(srv4->getIsUpdated())
        CPPUNIT_ASSERT(srv4->getIsUpdated());
        CPPUNIT_ASSERT(!srv4->getIsUpdated2());

        sig->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        srv1->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig2 = data2->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig2->async_emit();
        SIGHT_TEST_WAIT(srv1->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data4 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        sig4->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data3 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig3 = data3->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig3->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());
    }

    // Remove one data
    genDataSrv->set_output("out3", nullptr);

    SIGHT_TEST_WAIT(core::tools::id::exist("TestService3Uid") == false);

    // Service 3 should be removed
    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        CPPUNIT_ASSERT(gn_srv3 == nullptr);
    }

    // Check that connection is still there for the working services
    srv2->resetIsUpdated();
    CPPUNIT_ASSERT(!srv2->getIsUpdated());
    sig->async_emit();
    SIGHT_TEST_WAIT(srv2->getIsUpdated());
    CPPUNIT_ASSERT(srv2->getIsUpdated());

    srv1->resetIsUpdated();
    CPPUNIT_ASSERT(!srv1->getIsUpdated());
    auto sig2 = data2->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig2->async_emit();
    SIGHT_TEST_WAIT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    // Check connection data4 -> srv2
    srv2->resetIsUpdated();
    CPPUNIT_ASSERT(!srv2->getIsUpdated());

    sig4->async_emit();
    SIGHT_TEST_WAIT(srv2->getIsUpdated());

    CPPUNIT_ASSERT(srv2->getIsUpdated());

    // Check "stopped" signal
    CPPUNIT_ASSERT(srv4->getIsUpdated());
    CPPUNIT_ASSERT(srv4->getIsUpdated2());
    srv4->resetIsUpdated();
    srv4->resetIsUpdated2();
    CPPUNIT_ASSERT(!srv4->getIsUpdated2());

    // Add back data 3 and check connection again
    genDataSrv->set_output("out3", data3);
    wait_service_started("TestService3Uid");

    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv3->status());
        srv2->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        SIGHT_TEST_WAIT(srv4->getIsUpdated())
        CPPUNIT_ASSERT(srv4->getIsUpdated());
        CPPUNIT_ASSERT(!srv4->getIsUpdated2());

        sig->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        srv1->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto modifiedSig2 = data2->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        modifiedSig2->async_emit();
        SIGHT_TEST_WAIT(srv1->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data4 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        sig4->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data3 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig3 = data3->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig3->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());
    }
}

//------------------------------------------------------------------------------

void config_test::startStopConnectionTest()
{
    {
        m_appConfigMgr = this->launchAppConfigMgr("startStopConnectionTest");

        // Check TestService5 starts TestService6
        // Check TestService5 stops TestService6
        {
            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv5->status());

            {
                core::tools::object::sptr gn_srv6 = core::tools::id::get_object("TestService6Uid");
                auto srv6                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv6);
                CPPUNIT_ASSERT(srv6 != nullptr);
                CPPUNIT_ASSERT_EQUAL(service::base::STOPPED, srv6->status());

                srv5->update().wait();
                SIGHT_TEST_WAIT(srv6->started());
                CPPUNIT_ASSERT_EQUAL(true, srv6->started());

                auto sig =
                    srv5->signal<app::ut::test_srv::signals::int_sent_t>(app::ut::test_srv::signals::SIG_1);
                sig->async_emit(0);

                SIGHT_TEST_WAIT(srv6->stopped());
                CPPUNIT_ASSERT_EQUAL(true, srv6->stopped());
            }
        }
        m_appConfigMgr->stopAndDestroy();
        m_appConfigMgr.reset();
    }

    {
        m_appConfigMgr = this->launchAppConfigMgr("startStopConnectionTest");

        // Check TestService5 starts TestService6
        // Check TestService5 stops TestService6
        // Check TestService6 is well stopped by the config_manager
        // Check TestService6 stops TestService7
        {
            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv5->status());

            {
                core::tools::object::sptr gn_srv6 = core::tools::id::get_object("TestService6Uid");
                auto srv6                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv6);
                CPPUNIT_ASSERT(srv6 != nullptr);
                CPPUNIT_ASSERT_EQUAL(service::base::STOPPED, srv6->status());

                srv5->update().wait();
                SIGHT_TEST_WAIT(srv6->started());
                CPPUNIT_ASSERT_EQUAL(true, srv6->started());

                auto sig =
                    srv5->signal<app::ut::test_srv::signals::int_sent_t>(app::ut::test_srv::signals::SIG_1);
                sig->async_emit(0);

                SIGHT_TEST_WAIT(srv6->stopped());
                CPPUNIT_ASSERT_EQUAL(true, srv6->stopped());

                srv5->update().wait();
                SIGHT_TEST_WAIT(srv6->started());
                CPPUNIT_ASSERT_EQUAL(true, srv6->started());

                core::tools::object::sptr gn_srv7 = core::tools::id::get_object("TestService7Uid");
                auto srv7                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv7);
                srv6->update().wait();
                SIGHT_TEST_WAIT(srv7->stopped());
                CPPUNIT_ASSERT_EQUAL(true, srv7->stopped());
            }
        }
        m_appConfigMgr->stopAndDestroy();
        m_appConfigMgr.reset();
    }
}

//------------------------------------------------------------------------------

void config_test::optionalKeyTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("optionalKeyTest");

    // Service used to generate data
    auto genDataSrv = std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    auto data1 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test service with two optional attributes and available data at start
    // =================================================================================================================

    core::tools::object::sptr gn_srv1 = core::tools::id::get_object("TestService1Uid");
    auto srv1                         = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv1);
    CPPUNIT_ASSERT(srv1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv1->status());
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    // Check connection
    auto sig = data1->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig->async_emit();
    SIGHT_TEST_WAIT(srv1->getIsUpdated());

    CPPUNIT_ASSERT(srv1->getIsUpdated());
    srv1->resetIsUpdated();

    CPPUNIT_ASSERT(srv1->input("data1").lock() == data1);
    CPPUNIT_ASSERT(srv1->input("data2").expired());
    CPPUNIT_ASSERT(srv1->input("data3").expired());

    // Create data 2
    data::boolean::sptr data2 = std::make_shared<data::boolean>();

    genDataSrv->set_output("out2", data2);
    SIGHT_TEST_WAIT(
        !srv1->input("data2").expired()
        && srv1->input("data2").lock() == data2
    );

    CPPUNIT_ASSERT(srv1->input("data2").lock() == data2);
    CPPUNIT_ASSERT(srv1->input("data3").expired());

    SIGHT_TEST_WAIT("data2" == srv1->getSwappedObjectKey());
    CPPUNIT_ASSERT_EQUAL(std::string("data2"), srv1->getSwappedObjectKey());
    CPPUNIT_ASSERT(data2 == srv1->getSwappedObject());

    // Check no connection with data 2
    CPPUNIT_ASSERT(!srv1->getIsUpdated());
    auto sig2 = data2->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig2->async_emit();
    SIGHT_TEST_WAIT(!srv1->getIsUpdated());
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    // Create data 3 and 4
    data::boolean::sptr data3 = std::make_shared<data::boolean>();
    data::boolean::sptr data4 = std::make_shared<data::boolean>();

    genDataSrv->set_output("out3", data3);
    genDataSrv->set_output("out4", data4);

    SIGHT_TEST_WAIT(
        !srv1->input("data3").expired()
        && !srv1->input("data4").expired()
        && srv1->input("data3").lock() == data3
        && srv1->input("data4").lock() == data4
    );

    CPPUNIT_ASSERT(srv1->input("data3").lock() == data3);
    CPPUNIT_ASSERT(srv1->input("data4").lock() == data4);
    CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv1->status());

    // Check connection with data 3
    srv1->resetIsUpdated();
    auto sig3 = data3->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig3->async_emit();
    SIGHT_TEST_WAIT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    // Check connection with data 4
    srv1->resetIsUpdated();
    auto sig4 = data4->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig4->async_emit();
    SIGHT_TEST_WAIT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    // Remove data 2 and 3
    genDataSrv->set_output("out2", nullptr);
    SIGHT_TEST_WAIT(srv1->input("data2").expired());

    SIGHT_TEST_WAIT("data2" == srv1->getSwappedObjectKey());
    CPPUNIT_ASSERT_EQUAL(std::string("data2"), srv1->getSwappedObjectKey());
    CPPUNIT_ASSERT(nullptr == srv1->getSwappedObject());
    CPPUNIT_ASSERT(srv1->input("data2").expired());

    genDataSrv->set_output("out3", nullptr);
    SIGHT_TEST_WAIT(
        !srv1->input(
            "data3"
        ).expired() && srv1->input("data3").lock() == nullptr
    );

    SIGHT_TEST_WAIT("data3" == srv1->getSwappedObjectKey());
    CPPUNIT_ASSERT_EQUAL(std::string("data3"), srv1->getSwappedObjectKey());
    CPPUNIT_ASSERT(nullptr == srv1->getSwappedObject());
    CPPUNIT_ASSERT(srv1->input("data3").expired());

    CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv1->status());

    // Create data 3
    genDataSrv->set_output("out3", data3);
    SIGHT_TEST_WAIT(
        !srv1->input(
            "data3"
        ).expired() && srv1->input("data3").lock() == data3
    );

    CPPUNIT_ASSERT(srv1->input("data2").expired());
    CPPUNIT_ASSERT(srv1->input("data3").lock() == data3);
    CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv1->status());

    // =================================================================================================================
    // Test service with two optional attributes and unavailable data at start
    // =================================================================================================================

    // Create data 5
    data::boolean::sptr data5 = std::make_shared<data::boolean>();
    {
        core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
        CPPUNIT_ASSERT(gn_srv2 == nullptr);

        genDataSrv->set_output("out5", data5);
        wait_service_started("TestService2Uid");

        gn_srv2 = core::tools::id::get_object("TestService2Uid");
        CPPUNIT_ASSERT(gn_srv2 != nullptr);
        auto srv2 = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv2->status());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        CPPUNIT_ASSERT(srv2->input("data1").lock() == data5);
        CPPUNIT_ASSERT(srv2->input("data2").expired());
        CPPUNIT_ASSERT(srv2->input("data3").lock() == data3);
        CPPUNIT_ASSERT(srv2->input("data4").lock() == data4);

        // Check connection with data 4
        srv2->resetIsUpdated();
        auto modifiedSig4 = data4->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        modifiedSig4->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated());

        // Remove data 3 and 4
        genDataSrv->set_output("out3", nullptr);
        genDataSrv->set_output("out4", nullptr);

        SIGHT_TEST_WAIT(
            !srv2->input("data3").expired()
            && srv2->input("data4").expired()
        );

        CPPUNIT_ASSERT(srv2->input("data1").lock() == data5);
        CPPUNIT_ASSERT(srv2->input("data2").expired());
        CPPUNIT_ASSERT(srv2->input("data3").expired());
        CPPUNIT_ASSERT(srv2->input("data4").expired());

        // Create data 3
        genDataSrv->set_output("out3", data3);
        SIGHT_TEST_WAIT(!srv2->input("data3").expired());

        CPPUNIT_ASSERT(srv2->input("data1").lock() == data5);
        CPPUNIT_ASSERT(srv2->input("data2").expired());
        CPPUNIT_ASSERT(srv2->input("data3").lock() == data3);
        CPPUNIT_ASSERT(srv2->input("data4").expired());
    }

    // Remove data 5
    {
        genDataSrv->set_output("out5", nullptr);
        SIGHT_TEST_WAIT(false == core::tools::id::exist("TestService2Uid"));

        core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService2Uid");
        CPPUNIT_ASSERT(gn_srv5 == nullptr);
    }

    {
        // Create data 5
        genDataSrv->set_output("out5", data5);
        wait_service_started("TestService2Uid");

        auto gn_srv2 = core::tools::id::get_object("TestService2Uid");
        auto srv2    = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv2->status());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        CPPUNIT_ASSERT(srv2->input("data1").lock() == data5);
        CPPUNIT_ASSERT(srv2->input("data2").expired());
        CPPUNIT_ASSERT(srv2->input("data3").lock() == data3);
        CPPUNIT_ASSERT(srv2->input("data4").expired());

        // Check connection with data 3
        srv2->resetIsUpdated();
        auto modifiedSig3 = data3->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        modifiedSig3->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());

        // Create data 2
        data::boolean::sptr data2b = std::make_shared<data::boolean>();

        genDataSrv->set_output("out2", data2b);
        SIGHT_TEST_WAIT(
            !srv2->input(
                "data2"
            ).expired()
            && srv2->input("data2").lock() == data2b
        );

        CPPUNIT_ASSERT(srv2->input("data1").lock() == data5);
        CPPUNIT_ASSERT(srv2->input("data2").lock() == data2b);
        CPPUNIT_ASSERT(srv2->input("data3").lock() == data3);
        CPPUNIT_ASSERT(srv2->input("data4").expired());

        // Check no connection with data 2
        srv2->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        auto modifiedSig2 = data2b->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        modifiedSig2->async_emit();
        SIGHT_TEST_WAIT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        // Overwrite data 2 with a new data generated by an another service
        data::boolean::sptr data2bis = std::make_shared<data::boolean>();

        auto genDataSrv2 =
            std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData2"));
        CPPUNIT_ASSERT(genDataSrv2 != nullptr);

        genDataSrv2->set_output("out", data2bis);
        SIGHT_TEST_WAIT(
            !srv2->input(
                "data2"
            ).expired() && srv2->input(
                "data2"
            ).lock() == data2bis
        );

        CPPUNIT_ASSERT(srv2->input("data2").lock() == data2bis);

        SIGHT_TEST_WAIT("data2" == srv2->getSwappedObjectKey() && data2bis == srv2->getSwappedObject());
        CPPUNIT_ASSERT("data2" == srv2->getSwappedObjectKey());
        CPPUNIT_ASSERT(data2bis == srv2->getSwappedObject());

        // Check that the output of SGenerateData changed as well
        SIGHT_TEST_WAIT(data2bis == genDataSrv->data::has_data::output("out2").lock().get_shared());
        CPPUNIT_ASSERT(data2bis == genDataSrv->data::has_data::output("out2").lock().get_shared());

        // Revert that
        genDataSrv2->set_output("out", data2b);
        SIGHT_TEST_WAIT(
            !srv2->input("data2").expired()
            && srv2->input("data2").lock() == data2b
        );

        CPPUNIT_ASSERT(srv2->input("data2").lock() == data2b);

        SIGHT_TEST_WAIT("data2" == srv2->getSwappedObjectKey() && data2b == srv2->getSwappedObject());
        CPPUNIT_ASSERT("data2" == srv2->getSwappedObjectKey());
        CPPUNIT_ASSERT(data2b == srv2->getSwappedObject());

        // Check that the output of SGenerateData changed as well
        SIGHT_TEST_WAIT(data2b == genDataSrv->data::has_data::object("out2", data::Access::out));
        CPPUNIT_ASSERT(data2b == genDataSrv->data::has_data::object("out2", data::Access::out));
    }
}

//------------------------------------------------------------------------------

void config_test::keyGroupTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("keyGroupTest");

    // Service used to generate data
    auto genDataSrv = std::dynamic_pointer_cast<app::ut::STestOut>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    auto data1 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    auto data4 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data4Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    auto data5 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data5Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test service with one key group of two data
    // =================================================================================================================

    data::image::sptr data3;
    {
        core::tools::object::sptr gn_srv1 = core::tools::id::get_object("TestService1Uid");
        CPPUNIT_ASSERT(gn_srv1 == nullptr);

        // Create data 2b
        data::boolean::sptr data2b = std::make_shared<data::boolean>();
        genDataSrv->set_output("out2", data2b);
        wait_service_started("TestService1Uid");

        gn_srv1 = core::tools::id::get_object("TestService1Uid");
        auto srv1 = std::dynamic_pointer_cast<app::ut::STest1Input1InputGroup>(gn_srv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv1->status());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());

        CPPUNIT_ASSERT(srv1->input("data1").lock() == data1);
        CPPUNIT_ASSERT(srv1->input("dataGroup", 0).lock() == data2b);
        CPPUNIT_ASSERT(srv1->input("dataGroup", 1).expired());
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), srv1->m_inputGroup.size());

        // Check connection with data 2
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        auto sig2 = data2b->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig2->async_emit();
        SIGHT_TEST_WAIT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv1->getIsUpdated());

        // Create data 3
        data3 = std::make_shared<data::image>();

        srv1->resetIsUpdated();

        // Create a slot to wait for the swap to be completed
        bool srv1Swapped = false;
        std::function fn = [&]()
                           {
                               srv1Swapped = true;
                           };
        auto swappedSlot = core::com::new_slot(fn);
        swappedSlot->set_worker(core::thread::get_default_worker());
        core::com::connection connection = srv1->signal(service::signals::SWAPPED)->connect(swappedSlot);

        genDataSrv->set_output("out3", data3);

        SIGHT_TEST_WAIT(
            !srv1->input(
                "dataGroup",
                1
            ).expired() && srv1->input(
                "dataGroup",
                1
            ).lock() == data3
        );

        CPPUNIT_ASSERT(srv1->input("dataGroup", 0).lock() == data2b);
        CPPUNIT_ASSERT(srv1->input("dataGroup", 1).lock() == data3);
        CPPUNIT_ASSERT(2 == srv1->m_inputGroup.size());

        SIGHT_TEST_WAIT(srv1Swapped);
        CPPUNIT_ASSERT(srv1Swapped);

        // Check connection with data 3
        auto sig3 = data3->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig3->async_emit();
        SIGHT_TEST_WAIT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv1->getIsUpdated());

        connection.disconnect();
    }

    // Remove data 2
    {
        genDataSrv->set_output("out2", nullptr);

        SIGHT_TEST_WAIT(false == core::tools::id::exist("TestService1Uid"));

        core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService1Uid");
        CPPUNIT_ASSERT(gn_srv5 == nullptr);
    }

    {
        wait_service_started("TestService2Uid");
        core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
        auto srv2                         = std::dynamic_pointer_cast<app::ut::STest2InputGroups>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv2->status());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        SIGHT_TEST_WAIT(
            !srv2->input(
                "dataGroup0",
                1
            ).expired() && srv2->input(
                "dataGroup0",
                1
            ).lock() == data3
        );

        CPPUNIT_ASSERT_EQUAL(std::size_t(1), srv2->m_input1.size());
        CPPUNIT_ASSERT(srv2->input("dataGroup0", 0).lock() == data1);
        CPPUNIT_ASSERT(srv2->input("dataGroup0", 1).expired());

        CPPUNIT_ASSERT_EQUAL(std::size_t(3), srv2->m_input2.size());
        CPPUNIT_ASSERT(srv2->input("dataGroup1", 0).lock() == data3);
        CPPUNIT_ASSERT(srv2->input("dataGroup1", 1).lock() == data4);
        CPPUNIT_ASSERT(srv2->input("dataGroup1", 2).lock() == data5);

        // Check connection with data 1
        srv2->resetIsUpdated();
        auto sig1 = data1->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig1->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());

        // Check no connection with data 3
        srv2->resetIsUpdated();
        auto sig3 = data3->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig3->async_emit();
        SIGHT_TEST_WAIT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        // Check connection with data 4
        srv2->resetIsUpdated();
        auto sig4 = data4->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig4->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated(), 2500);
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        auto sigIm4 = data4->signal<data::image::BufferModifiedSignalType>(data::image::BUFFER_MODIFIED_SIG);
        sigIm4->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());

        // Check no connection with data 5
        srv2->resetIsUpdated();
        auto sig5 = data5->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig5->async_emit();
        SIGHT_TEST_WAIT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        auto sigIm5 = data5->signal<data::image::BufferModifiedSignalType>(data::image::BUFFER_MODIFIED_SIG);
        sigIm5->async_emit();
        SIGHT_TEST_WAIT(srv2->getIsUpdated(), 2500);
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
    }

    // Test output data group
    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        CPPUNIT_ASSERT(gn_srv3 == nullptr);

        auto data6 = std::make_shared<data::image>();
        genDataSrv->m_outGroup[0] = data6;

        gn_srv3 = core::tools::id::get_object("TestService3Uid");
        CPPUNIT_ASSERT(gn_srv3 == nullptr);

        auto data7 = std::make_shared<data::image>();
        genDataSrv->m_outGroup[1] = data7;

        wait_service_started("TestService3Uid");
        gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3 = std::dynamic_pointer_cast<app::ut::STest1Input1InputGroup>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::STARTED, srv3->status());

        CPPUNIT_ASSERT(srv3->input("dataGroup", 0).lock() == data6);
        CPPUNIT_ASSERT(srv3->input("dataGroup", 1).lock() == data7);

        genDataSrv->m_outGroup[0] = nullptr;
    }
    {
        SIGHT_TEST_WAIT(false == core::tools::id::exist("TestService3Uid"));

        auto gn_srv3 = core::tools::id::get_object("TestService3Uid");
        CPPUNIT_ASSERT(gn_srv3 == nullptr);
    }
}

//-----------------------------------------------------------------------------

void config_test::concurrent_access_to_config_test()
{
    std::vector<std::future<void> > futures;
    for(unsigned int i = 0 ; i < 20 ; ++i)
    {
        futures.push_back(std::async(std::launch::async, parametersConfigTest));
    }

    for(auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }

    app::extension::config::getDefault()->clear_registry();
    std::vector<std::string> allConfigs = app::extension::config::getDefault()->getAllConfigs();
    CPPUNIT_ASSERT(allConfigs.empty());
}

//------------------------------------------------------------------------------

void config_test::parameterReplaceTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("parameterReplaceTest", true);

    unsigned int i = 0;
    unsigned int j = 0;
    core::tools::object::sptr gn_srv1;
    core::tools::object::sptr gn_srv2;

    // Not really elegant, but we have to "guess" how it is replaced
    while(gn_srv1 == nullptr && i++ < 200)
    {
        gn_srv1 = core::tools::id::get_object("parameterReplaceTest_" + std::to_string(i) + "_TestService1Uid");
    }

    auto srv1 = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv1);
    CPPUNIT_ASSERT(srv1 != nullptr);

    gn_srv2 = core::tools::id::get_object("parameterReplaceTest_" + std::to_string(i) + "_TestService2Uid");
    auto srv2          = std::dynamic_pointer_cast<service::base>(gn_srv2);
    auto adaptedConfig = srv2->get_config();

    const auto params = adaptedConfig.equal_range("parameter");

    std::vector<service::config_t> paramsCfg;
    std::for_each(params.first, params.second, [&paramsCfg](const auto& p){paramsCfg.push_back(p.second);});

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), paramsCfg.size());

    std::string replaceBy;
    CPPUNIT_ASSERT_EQUAL(std::string("patient"), paramsCfg[0].get<std::string>("<xmlattr>.replace"));
    CPPUNIT_ASSERT_EQUAL(std::string("name"), paramsCfg[0].get<std::string>("<xmlattr>.by"));

    replaceBy = paramsCfg[1].get<std::string>("<xmlattr>.by");
    CPPUNIT_ASSERT_EQUAL(std::string("parameterReplaceTest_" + std::to_string(i) + "_Channel No5"), replaceBy);

    replaceBy = paramsCfg[2].get<std::string>("<xmlattr>.by");
    CPPUNIT_ASSERT_EQUAL(std::string("parameterReplaceTest_" + std::to_string(i) + "_disneyChannel"), replaceBy);

    replaceBy = paramsCfg[3].get<std::string>("<xmlattr>.by");
    CPPUNIT_ASSERT_EQUAL(std::string("parameterReplaceTest_" + std::to_string(i) + "_view1"), replaceBy);

    core::tools::object::sptr gn_sub_srv;

    // Not really elegant, but we have to "guess" how it is replaced
    while(gn_sub_srv == nullptr && j++ < 200)
    {
        gn_sub_srv = core::tools::id::get_object(
            "parameterReplaceTestSubConfig_" + std::to_string(j)
            + "_TestServiceUid"
        );
    }

    auto srvInSubConfig = std::dynamic_pointer_cast<app::ut::test_service>(gn_sub_srv);
    CPPUNIT_ASSERT(srvInSubConfig != nullptr);

    CPPUNIT_ASSERT(srvInSubConfig->started());

    auto data1 = srv1->input("data1").lock();
    CPPUNIT_ASSERT(data1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(std::string("data1Id"), data1->get_id());

    auto data2 = srv1->input("data2").lock();
    CPPUNIT_ASSERT(data2 != nullptr);

    auto data1SubSrv = srvInSubConfig->input("data1").lock();
    CPPUNIT_ASSERT(data1 == data1SubSrv);

    auto data2SubSrv = srvInSubConfig->input("data2").lock();
    CPPUNIT_ASSERT(data2 == data2SubSrv);

    // check connections through the subconfig channel
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    auto sig1 = data1SubSrv->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig1->async_emit();

    SIGHT_TEST_WAIT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    CPPUNIT_ASSERT(!srvInSubConfig->getIsUpdated());

    auto sig2 = data2->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig2->async_emit();

    SIGHT_TEST_WAIT(srvInSubConfig->getIsUpdated());
    CPPUNIT_ASSERT(srvInSubConfig->getIsUpdated());
}

//------------------------------------------------------------------------------

void config_test::objectConfigTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("objectConfigTest");

    // =================================================================================================================
    // Test a service with an external configuration and test Composite with sub-object parsing
    // =================================================================================================================

    auto compo1 = std::dynamic_pointer_cast<data::composite>(core::tools::id::get_object("compo1Id"));
    CPPUNIT_ASSERT(compo1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), compo1->count("dataInComposite"));
    auto data2 = compo1->get<data::string>("dataInComposite");
    CPPUNIT_ASSERT(data2);
    CPPUNIT_ASSERT_EQUAL(std::string("data2Id"), data2->get_id());
    CPPUNIT_ASSERT_EQUAL(std::string("Hello"), data2->value());

    // This service should have a composite data and contain an external configuration with 2 parameters
    core::tools::object::sptr service = core::tools::id::get_object("TestService1Uid");
    auto srv1                         = std::dynamic_pointer_cast<app::ut::test_service>(service);
    CPPUNIT_ASSERT(srv1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(service::base::CONFIGURED, srv1->config_status());

    auto srvData1 = srv1->input<data::composite>("data1");
    CPPUNIT_ASSERT(!srvData1.expired());
    CPPUNIT_ASSERT(srvData1.lock() == compo1);

    auto config = srv1->get_config();
    CPPUNIT_ASSERT_EQUAL(std::string("value1"), config.get<std::string>("param1"));
    CPPUNIT_ASSERT_EQUAL(std::string("value2"), config.get<std::string>("param2"));
}

//------------------------------------------------------------------------------

service::config_t config_test::buildConfig()
{
    service::config_t cfg;

    // Object
    service::config_t objCfg;
    objCfg.add("<xmlattr>.uid", "image");
    objCfg.add("<xmlattr>.type", "sight::data::image");
    cfg.add_child("object", objCfg);

    // Service
    service::config_t srvCfg;
    srvCfg.add("<xmlattr>.uid", "myTestService1");
    srvCfg.add("<xmlattr>.type", "sight::app::ut::STest1Image");
    srvCfg.add("<xmlattr>.auto_connect", "false");
    cfg.add_child("service", srvCfg);

    // Connections
    service::config_t connectCfg;
    connectCfg.add("<xmlattr>.channel", "channel1");
    connectCfg.add("signal", "image/modified");
    connectCfg.add("slot", "myTestService1/update");
    cfg.add_child("connect", connectCfg);

    // Start method from object's services
    service::config_t startCfg;
    startCfg.add("<xmlattr>.uid", "myTestService1");
    cfg.add_child("start", startCfg);

    // Update method from object's services
    service::config_t updateCfg;
    updateCfg.add("<xmlattr>.uid", "myTestService1");
    cfg.add_child("update", updateCfg);

    return cfg;
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
