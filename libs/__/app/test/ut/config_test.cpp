/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

static inline void wait_service_started(const std::string& _srv)
{
    auto service = core::tools::id::get_object(_srv);
    SIGHT_TEST_WAIT(service != nullptr && std::dynamic_pointer_cast<service::base>(service)->started());
}

//------------------------------------------------------------------------------

void config_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("app_ut");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
    core::runtime::load_module("config_test");

    auto app_config = app::extension::config::get_default();
    app_config->clear_registry();
    app_config->parse_plugin_infos();

    auto srv_config = sight::service::extension::config::get_default();
    srv_config->clear_registry();
    srv_config->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void config_test::tearDown()
{
    // Clean up after the test run.
    if(m_app_config_mgr)
    {
        // If everything went well, the manager should have been destroyed
        // This means a test failed, thus we need to clean everything properly, otherwise
        // We will get an assert from the destructor and we will not get the cppunit report in the console
        m_app_config_mgr->stop_and_destroy();
        m_app_config_mgr = nullptr;
    }
}

//------------------------------------------------------------------------------

void config_test::add_config_test()
{
    auto current_app_config = app::extension::config::get_default();

    const std::string config_id(app::extension::config::get_unique_identifier());
    const std::string group("TestGroup");
    const std::string desc("Description");
    const std::string module_id("sight::module::service");
    app::extension::app_info::parameters_t parameters;

    const service::config_t config = build_config();

    current_app_config->addapp_info(config_id, group, desc, parameters, config, module_id);

    std::vector<std::string> all_configs = current_app_config->get_all_configs();
    CPPUNIT_ASSERT_EQUAL(false, all_configs.empty());

    std::vector<std::string> configs = current_app_config->get_configs_from_group(group);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), configs.size());
    CPPUNIT_ASSERT_EQUAL(config_id, configs.front());

    auto module = current_app_config->get_module(config_id);
    CPPUNIT_ASSERT(module);
    CPPUNIT_ASSERT_EQUAL(module_id, module->identifier());

    app::field_adaptor_t replace_fields;

    core::runtime::config_t config_adapted = current_app_config->get_adapted_template_config(
        config_id,
        replace_fields,
        false
    );
    const auto uid = config_adapted.get<std::string>("object.<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("image"), uid);

    const auto service_uid1 = config_adapted.get<std::string>("service.<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("myTestService1"), service_uid1);
}

//------------------------------------------------------------------------------

void config_test::parameters_config_test()
{
    app::extension::config::sptr current_app_config = app::extension::config::get_default();

    const std::string config_id("parametersConfigTest1");
    const std::string group("parametersGroup");

    app::field_adaptor_t replace_fields;
    replace_fields["TEST_IMAGE"] = "objectUUID";

    std::vector<std::string> all_configs = current_app_config->get_all_configs();
    auto it                              = std::find(all_configs.begin(), all_configs.end(), config_id);
    CPPUNIT_ASSERT(it != all_configs.end());

    core::runtime::config_t config_adapted = current_app_config->get_adapted_template_config(
        config_id,
        replace_fields,
        false
    );

    const auto uid = config_adapted.get<std::string>("object.<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("objectUUID"), uid);

    auto services_cfg = config_adapted.equal_range("service");

    const auto service_uid1 = services_cfg.first->second.get<std::string>("<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("myTestService1"), service_uid1);

    services_cfg.first++;
    const auto service_uid2 = services_cfg.first->second.get<std::string>("<xmlattr>.uid");
    CPPUNIT_ASSERT_EQUAL(std::string("myTestService2"), service_uid2);
}

//------------------------------------------------------------------------------

app::config_manager::sptr config_test::launch_app_config_mgr(
    const std::string& _name,
    bool _auto_prefix
)
{
    auto app_config_mgr = app::config_manager::make();

    const app::field_adaptor_t fields;
    app_config_mgr->set_config(_name, fields, _auto_prefix);
    app_config_mgr->launch();

    return app_config_mgr;
}

//------------------------------------------------------------------------------

void config_test::start_stop_test()
{
    m_app_config_mgr = this->launch_app_config_mgr("startStopTest");

    // =================================================================================================================
    // Test manual start and stop of services, with or without data
    // =================================================================================================================

    auto data1 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // This service doesn't exist in the config
    CPPUNIT_ASSERT(core::tools::id::get_object("TestService142Uid") == nullptr);

    auto gen_data_srv = std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(gen_data_srv != nullptr);

    // This service has no data and is started by the config
    {
        core::tools::object::sptr gn_srv1 = core::tools::id::get_object("TestService1Uid");
        auto srv1                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv1->status());
    }

    // This service has no data and is NOT started by the config
    {
        core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
        auto srv2                         = std::dynamic_pointer_cast<service::base>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv2->status());
        srv2->stop().wait();
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::stopped, srv2->status());
        srv2->start().wait();
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv2->status());
    }

    // This service has a data and is NOT started by the config
    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3                         = std::dynamic_pointer_cast<service::base>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv3->status());
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
        gen_data_srv->set_output("out2", data2);
        wait_service_started("TestService4Uid");

        // Now the service should have been started automatically
        {
            auto gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4    = std::dynamic_pointer_cast<service::base>(gn_srv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv4->status());
        }

        // Remove the data
        gen_data_srv->set_output("out2", nullptr);
        SIGHT_TEST_WAIT(core::tools::id::exist("TestService4Uid") == false);

        // Now the service should have been stopped and destroyed automatically
        {
            auto gn_srv4 = core::tools::id::get_object("TestService4Uid");
            CPPUNIT_ASSERT(gn_srv4 == nullptr);
        }

        // Register the data once again
        gen_data_srv->set_output("out2", data2);
        wait_service_started("TestService4Uid");

        // Check again that the service was started automatically
        {
            auto gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4    = std::dynamic_pointer_cast<service::base>(gn_srv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv4->status());
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

        gen_data_srv->set_output("out4", data4);
        wait_service_started("TestService5Uid");

        // Now the service should have been started automatically
        {
            auto gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5    = std::dynamic_pointer_cast<service::base>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv5->status());
        }

        // Remove one data
        gen_data_srv->set_output("out2", nullptr);

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
        app::ut::test_service::s_start_counter  = 0;
        app::ut::test_service::s_update_counter = 0;
        gen_data_srv->set_output("out2", data2);
        wait_service_started("TestService5Uid");

        // Now the service should have been started automatically, check start order as well
        {
            auto gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5    = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv5->status());

            // We have started, yet we may not have been updated, so wait for it just in case
            SIGHT_TEST_WAIT(1U == srv5->get_update_order());
            CPPUNIT_ASSERT_EQUAL(0U, srv5->get_start_order());
            CPPUNIT_ASSERT_EQUAL(1U, srv5->get_update_order());

            // Test as well service 4, just to be sure
            auto gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4    = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv4);
            CPPUNIT_ASSERT(gn_srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(1U, srv4->get_start_order());
            CPPUNIT_ASSERT_EQUAL(0U, srv4->get_update_order());
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
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv5->status());
        }

        // Swap the data
        data::boolean::sptr data5 = std::make_shared<data::boolean>();

        gen_data_srv->set_output("out2", nullptr);
        SIGHT_TEST_WAIT(core::tools::id::exist("TestService5Uid") == false);
        gen_data_srv->set_output("out2", data5);
        wait_service_started("TestService5Uid");

        {
            auto gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5    = std::dynamic_pointer_cast<service::base>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv5->status());
        }
    }

    m_app_config_mgr->stop_and_destroy();

    m_app_config_mgr = nullptr;
}

//------------------------------------------------------------------------------

void config_test::auto_connect_test()
{
    m_app_config_mgr = this->launch_app_config_mgr("autoConnectTest");

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
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv1->status());
        CPPUNIT_ASSERT(!srv1->get_is_updated());

        core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
        auto srv2                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv2->status());
        CPPUNIT_ASSERT(!srv2->get_is_updated());

        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3                         = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv3->status());
        CPPUNIT_ASSERT(!srv3->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_received());

        auto sig1 = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig1->async_emit();

        SIGHT_TEST_WAIT(srv2->get_is_updated() && srv3->get_is_updated());
        CPPUNIT_ASSERT(!srv1->get_is_updated());
        CPPUNIT_ASSERT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_received());

        srv2->reset_is_updated();
        srv3->reset_is_updated();

        CPPUNIT_ASSERT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());

        auto sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig2->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated() && srv3->get_received());
        CPPUNIT_ASSERT(!srv1->get_is_updated());
        CPPUNIT_ASSERT(srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_received());
    }

    // =================================================================================================================
    // Test autoconnect with unavailable data at start
    // =================================================================================================================

    // Service used to generate data
    auto gen_data_srv = std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(gen_data_srv != nullptr);
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
        gen_data_srv->set_output("out3", data3);

        wait_service_started("TestService4Uid");
        wait_service_started("TestService5Uid");
        {
            core::tools::object::sptr gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv4->status());
            CPPUNIT_ASSERT(!srv4->get_is_updated());

            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5                         = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv5->status());

            CPPUNIT_ASSERT(!srv4->get_is_updated());
            CPPUNIT_ASSERT(!srv5->get_is_updated());
            CPPUNIT_ASSERT(!srv5->get_received());

            auto sig = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig->async_emit();
            SIGHT_TEST_WAIT(srv5->get_received());

            CPPUNIT_ASSERT(!srv4->get_is_updated());
            CPPUNIT_ASSERT(!srv5->get_is_updated());
            CPPUNIT_ASSERT(srv5->get_received());
        }

        // Remove one data
        gen_data_srv->set_output("out3", nullptr);
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
        gen_data_srv->set_output("out3", data3);

        wait_service_started("TestService4Uid");
        wait_service_started("TestService5Uid");
        {
            core::tools::object::sptr gn_srv4 = core::tools::id::get_object("TestService4Uid");
            auto srv4                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv4->status());
            CPPUNIT_ASSERT(!srv4->get_is_updated());

            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5                         = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv5->status());

            CPPUNIT_ASSERT(!srv4->get_is_updated());
            CPPUNIT_ASSERT(!srv5->get_is_updated());
            CPPUNIT_ASSERT(!srv5->get_received());

            auto sig = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig->async_emit();
            SIGHT_TEST_WAIT(srv5->get_received());
            CPPUNIT_ASSERT(srv5->get_received());

            CPPUNIT_ASSERT(!srv4->get_is_updated());
            CPPUNIT_ASSERT(!srv5->get_is_updated());
            CPPUNIT_ASSERT(srv5->get_received());

            auto sig1 = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            sig1->async_emit();
            SIGHT_TEST_WAIT(srv5->get_is_updated());

            CPPUNIT_ASSERT(srv5->get_is_updated());
        }
    }
}

//------------------------------------------------------------------------------

void config_test::connection_test()
{
    m_app_config_mgr = this->launch_app_config_mgr("connectionTest");

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
    CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv1->status());
    CPPUNIT_ASSERT(!srv1->get_is_updated());

    core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
    auto srv2                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv2);
    CPPUNIT_ASSERT(srv2 != nullptr);
    CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv2->status());
    CPPUNIT_ASSERT(!srv2->get_is_updated());

    core::tools::object::sptr gn_srv4 = core::tools::id::get_object("TestService4Uid");
    auto srv4                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv4);
    CPPUNIT_ASSERT(srv4 != nullptr);
    SIGHT_TEST_WAIT(service::base::global_status::started == srv4->status());
    CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv4->status());

    // Check connection
    auto sig = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig->async_emit();
    SIGHT_TEST_WAIT(srv1->get_is_updated() && srv2->get_is_updated());

    CPPUNIT_ASSERT(srv1->get_is_updated());
    CPPUNIT_ASSERT(srv2->get_is_updated());

    // Service used to generate data
    auto gen_data_srv = std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(gen_data_srv != nullptr);

    // =================================================================================================================
    // Test connection with unavailable data at start
    // =================================================================================================================

    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        CPPUNIT_ASSERT(gn_srv3 == nullptr);
    }

    // Emit a signal just for fun, anyway the service doesn't exist
    sig = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig->emit();

    srv2->reset_is_updated();
    CPPUNIT_ASSERT(!srv2->get_is_updated());

    // Check connection data4 -> srv2
    auto data4 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data4Id"));
    CPPUNIT_ASSERT(data4 != nullptr);
    auto sig4 = data4->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig4->async_emit();
    SIGHT_TEST_WAIT(srv2->get_is_updated());

    CPPUNIT_ASSERT(srv2->get_is_updated());

    SIGHT_TEST_WAIT(!srv4->get_is_updated());
    CPPUNIT_ASSERT(!srv4->get_is_updated());
    CPPUNIT_ASSERT(!srv4->get_is_updated2());

    // Create the missing data
    data::boolean::sptr data2 = std::make_shared<data::boolean>();
    data::boolean::sptr data3 = std::make_shared<data::boolean>();
    gen_data_srv->set_output("out2", data2);
    gen_data_srv->set_output("out3", data3);
    wait_service_started("TestService3Uid");
    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        SIGHT_TEST_WAIT(service::base::global_status::started == srv3->status());
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv3->status());
        srv2->reset_is_updated();
        CPPUNIT_ASSERT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());

        // Check "started" signal
        SIGHT_TEST_WAIT(srv4->get_is_updated())
        CPPUNIT_ASSERT(srv4->get_is_updated());
        CPPUNIT_ASSERT(!srv4->get_is_updated2());

        sig->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated() && srv3->get_is_updated());

        CPPUNIT_ASSERT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_is_updated());

        srv1->reset_is_updated();
        srv3->reset_is_updated();
        CPPUNIT_ASSERT(!srv1->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());

        auto sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig2->async_emit();
        SIGHT_TEST_WAIT(srv1->get_is_updated() && srv3->get_is_updated());

        CPPUNIT_ASSERT(srv1->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_is_updated());

        // Check connection data4 -> srv2,srv3
        srv2->reset_is_updated();
        srv3->reset_is_updated();
        CPPUNIT_ASSERT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());

        sig4->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated() && srv3->get_is_updated());

        CPPUNIT_ASSERT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_is_updated());

        // Check connection data3 -> srv2,srv3
        srv2->reset_is_updated();
        srv3->reset_is_updated();
        CPPUNIT_ASSERT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());

        auto sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig3->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated() && srv3->get_is_updated());

        CPPUNIT_ASSERT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_is_updated());
    }

    // Remove one data
    gen_data_srv->set_output("out3", nullptr);

    SIGHT_TEST_WAIT(core::tools::id::exist("TestService3Uid") == false);

    // Service 3 should be removed
    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        CPPUNIT_ASSERT(gn_srv3 == nullptr);
    }

    // Check that connection is still there for the working services
    srv2->reset_is_updated();
    CPPUNIT_ASSERT(!srv2->get_is_updated());
    sig->async_emit();
    SIGHT_TEST_WAIT(srv2->get_is_updated());
    CPPUNIT_ASSERT(srv2->get_is_updated());

    srv1->reset_is_updated();
    CPPUNIT_ASSERT(!srv1->get_is_updated());
    auto sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig2->async_emit();
    SIGHT_TEST_WAIT(srv1->get_is_updated());
    CPPUNIT_ASSERT(srv1->get_is_updated());

    // Check connection data4 -> srv2
    srv2->reset_is_updated();
    CPPUNIT_ASSERT(!srv2->get_is_updated());

    sig4->async_emit();
    SIGHT_TEST_WAIT(srv2->get_is_updated());

    CPPUNIT_ASSERT(srv2->get_is_updated());

    // Check "stopped" signal
    CPPUNIT_ASSERT(srv4->get_is_updated());
    CPPUNIT_ASSERT(srv4->get_is_updated2());
    srv4->reset_is_updated();
    srv4->reset_is_updated2();
    CPPUNIT_ASSERT(!srv4->get_is_updated2());

    // Add back data 3 and check connection again
    gen_data_srv->set_output("out3", data3);
    wait_service_started("TestService3Uid");

    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv3->status());
        srv2->reset_is_updated();
        CPPUNIT_ASSERT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());

        SIGHT_TEST_WAIT(srv4->get_is_updated())
        CPPUNIT_ASSERT(srv4->get_is_updated());
        CPPUNIT_ASSERT(!srv4->get_is_updated2());

        sig->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated() && srv3->get_is_updated());

        CPPUNIT_ASSERT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_is_updated());

        srv1->reset_is_updated();
        srv3->reset_is_updated();
        CPPUNIT_ASSERT(!srv1->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());

        auto modified_sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        modified_sig2->async_emit();
        SIGHT_TEST_WAIT(srv1->get_is_updated() && srv3->get_is_updated());

        CPPUNIT_ASSERT(srv1->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_is_updated());

        // Check connection data4 -> srv2,srv3
        srv2->reset_is_updated();
        srv3->reset_is_updated();
        CPPUNIT_ASSERT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());

        sig4->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated() && srv3->get_is_updated());

        CPPUNIT_ASSERT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_is_updated());

        // Check connection data3 -> srv2,srv3
        srv2->reset_is_updated();
        srv3->reset_is_updated();
        CPPUNIT_ASSERT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv3->get_is_updated());

        auto sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig3->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated() && srv3->get_is_updated());

        CPPUNIT_ASSERT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv3->get_is_updated());
    }
}

//------------------------------------------------------------------------------

void config_test::start_stop_connection_test()
{
    {
        m_app_config_mgr = this->launch_app_config_mgr("startStopConnectionTest");

        // Check TestService5 starts TestService6
        // Check TestService5 stops TestService6
        {
            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv5->status());

            {
                core::tools::object::sptr gn_srv6 = core::tools::id::get_object("TestService6Uid");
                auto srv6                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv6);
                CPPUNIT_ASSERT(srv6 != nullptr);
                CPPUNIT_ASSERT_EQUAL(service::base::global_status::stopped, srv6->status());

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
        m_app_config_mgr->stop_and_destroy();
        m_app_config_mgr.reset();
    }

    {
        m_app_config_mgr = this->launch_app_config_mgr("startStopConnectionTest");

        // Check TestService5 starts TestService6
        // Check TestService5 stops TestService6
        // Check TestService6 is well stopped by the config_manager
        // Check TestService6 stops TestService7
        {
            core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService5Uid");
            auto srv5                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv5->status());

            {
                core::tools::object::sptr gn_srv6 = core::tools::id::get_object("TestService6Uid");
                auto srv6                         = std::dynamic_pointer_cast<app::ut::test_service>(gn_srv6);
                CPPUNIT_ASSERT(srv6 != nullptr);
                CPPUNIT_ASSERT_EQUAL(service::base::global_status::stopped, srv6->status());

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
        m_app_config_mgr->stop_and_destroy();
        m_app_config_mgr.reset();
    }
}

//------------------------------------------------------------------------------

void config_test::optional_key_test()
{
    m_app_config_mgr = this->launch_app_config_mgr("optionalKeyTest");

    // Service used to generate data
    auto gen_data_srv = std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(gen_data_srv != nullptr);

    auto data1 = std::dynamic_pointer_cast<data::object>(core::tools::id::get_object("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test service with two optional attributes and available data at start
    // =================================================================================================================

    core::tools::object::sptr gn_srv1 = core::tools::id::get_object("TestService1Uid");
    auto srv1                         = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv1);
    CPPUNIT_ASSERT(srv1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv1->status());
    CPPUNIT_ASSERT(!srv1->get_is_updated());

    // Check connection
    auto sig = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig->async_emit();
    SIGHT_TEST_WAIT(srv1->get_is_updated());

    CPPUNIT_ASSERT(srv1->get_is_updated());
    srv1->reset_is_updated();

    CPPUNIT_ASSERT(srv1->input("data1").lock() == data1);
    CPPUNIT_ASSERT(srv1->input("data2").expired());
    CPPUNIT_ASSERT(srv1->input("data3").expired());

    // Create data 2
    data::boolean::sptr data2 = std::make_shared<data::boolean>();

    gen_data_srv->set_output("out2", data2);
    SIGHT_TEST_WAIT(
        !srv1->input("data2").expired()
        && srv1->input("data2").lock() == data2
    );

    CPPUNIT_ASSERT(srv1->input("data2").lock() == data2);
    CPPUNIT_ASSERT(srv1->input("data3").expired());

    SIGHT_TEST_WAIT("data2" == srv1->get_swapped_object_key());
    CPPUNIT_ASSERT_EQUAL(std::string("data2"), srv1->get_swapped_object_key());
    CPPUNIT_ASSERT(data2 == srv1->get_swapped_object());

    // Check no connection with data 2
    CPPUNIT_ASSERT(!srv1->get_is_updated());
    auto sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig2->async_emit();
    SIGHT_TEST_WAIT(!srv1->get_is_updated());
    CPPUNIT_ASSERT(!srv1->get_is_updated());

    // Create data 3 and 4
    data::boolean::sptr data3 = std::make_shared<data::boolean>();
    data::boolean::sptr data4 = std::make_shared<data::boolean>();

    gen_data_srv->set_output("out3", data3);
    gen_data_srv->set_output("out4", data4);

    SIGHT_TEST_WAIT(
        !srv1->input("data3").expired()
        && !srv1->input("data4").expired()
        && srv1->input("data3").lock() == data3
        && srv1->input("data4").lock() == data4
    );

    CPPUNIT_ASSERT(srv1->input("data3").lock() == data3);
    CPPUNIT_ASSERT(srv1->input("data4").lock() == data4);
    CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv1->status());

    // Check connection with data 3
    srv1->reset_is_updated();
    auto sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig3->async_emit();
    SIGHT_TEST_WAIT(srv1->get_is_updated());
    CPPUNIT_ASSERT(srv1->get_is_updated());

    // Check connection with data 4
    srv1->reset_is_updated();
    auto sig4 = data4->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig4->async_emit();
    SIGHT_TEST_WAIT(srv1->get_is_updated());
    CPPUNIT_ASSERT(srv1->get_is_updated());

    // Remove data 2 and 3
    gen_data_srv->set_output("out2", nullptr);
    SIGHT_TEST_WAIT(srv1->input("data2").expired());

    SIGHT_TEST_WAIT("data2" == srv1->get_swapped_object_key());
    CPPUNIT_ASSERT_EQUAL(std::string("data2"), srv1->get_swapped_object_key());
    CPPUNIT_ASSERT(nullptr == srv1->get_swapped_object());
    CPPUNIT_ASSERT(srv1->input("data2").expired());

    gen_data_srv->set_output("out3", nullptr);
    SIGHT_TEST_WAIT(
        !srv1->input(
            "data3"
        ).expired() && srv1->input("data3").lock() == nullptr
    );

    SIGHT_TEST_WAIT("data3" == srv1->get_swapped_object_key());
    CPPUNIT_ASSERT_EQUAL(std::string("data3"), srv1->get_swapped_object_key());
    CPPUNIT_ASSERT(nullptr == srv1->get_swapped_object());
    CPPUNIT_ASSERT(srv1->input("data3").expired());

    CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv1->status());

    // Create data 3
    gen_data_srv->set_output("out3", data3);
    SIGHT_TEST_WAIT(
        !srv1->input(
            "data3"
        ).expired() && srv1->input("data3").lock() == data3
    );

    CPPUNIT_ASSERT(srv1->input("data2").expired());
    CPPUNIT_ASSERT(srv1->input("data3").lock() == data3);
    CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv1->status());

    // =================================================================================================================
    // Test service with two optional attributes and unavailable data at start
    // =================================================================================================================

    // Create data 5
    data::boolean::sptr data5 = std::make_shared<data::boolean>();
    {
        core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
        CPPUNIT_ASSERT(gn_srv2 == nullptr);

        gen_data_srv->set_output("out5", data5);
        wait_service_started("TestService2Uid");

        gn_srv2 = core::tools::id::get_object("TestService2Uid");
        CPPUNIT_ASSERT(gn_srv2 != nullptr);
        auto srv2 = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv2->status());
        CPPUNIT_ASSERT(!srv2->get_is_updated());

        CPPUNIT_ASSERT(srv2->input("data1").lock() == data5);
        CPPUNIT_ASSERT(srv2->input("data2").expired());
        CPPUNIT_ASSERT(srv2->input("data3").lock() == data3);
        CPPUNIT_ASSERT(srv2->input("data4").lock() == data4);

        // Check connection with data 4
        srv2->reset_is_updated();
        auto modified_sig4 = data4->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        modified_sig4->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated());

        // Remove data 3 and 4
        gen_data_srv->set_output("out3", nullptr);
        gen_data_srv->set_output("out4", nullptr);

        SIGHT_TEST_WAIT(
            !srv2->input("data3").expired()
            && srv2->input("data4").expired()
        );

        CPPUNIT_ASSERT(srv2->input("data1").lock() == data5);
        CPPUNIT_ASSERT(srv2->input("data2").expired());
        CPPUNIT_ASSERT(srv2->input("data3").expired());
        CPPUNIT_ASSERT(srv2->input("data4").expired());

        // Create data 3
        gen_data_srv->set_output("out3", data3);
        SIGHT_TEST_WAIT(!srv2->input("data3").expired());

        CPPUNIT_ASSERT(srv2->input("data1").lock() == data5);
        CPPUNIT_ASSERT(srv2->input("data2").expired());
        CPPUNIT_ASSERT(srv2->input("data3").lock() == data3);
        CPPUNIT_ASSERT(srv2->input("data4").expired());
    }

    // Remove data 5
    {
        gen_data_srv->set_output("out5", nullptr);
        SIGHT_TEST_WAIT(false == core::tools::id::exist("TestService2Uid"));

        core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService2Uid");
        CPPUNIT_ASSERT(gn_srv5 == nullptr);
    }

    {
        // Create data 5
        gen_data_srv->set_output("out5", data5);
        wait_service_started("TestService2Uid");

        auto gn_srv2 = core::tools::id::get_object("TestService2Uid");
        auto srv2    = std::dynamic_pointer_cast<app::ut::test_srv>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv2->status());
        CPPUNIT_ASSERT(!srv2->get_is_updated());

        CPPUNIT_ASSERT(srv2->input("data1").lock() == data5);
        CPPUNIT_ASSERT(srv2->input("data2").expired());
        CPPUNIT_ASSERT(srv2->input("data3").lock() == data3);
        CPPUNIT_ASSERT(srv2->input("data4").expired());

        // Check connection with data 3
        srv2->reset_is_updated();
        auto modified_sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        modified_sig3->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv2->get_is_updated());

        // Create data 2
        data::boolean::sptr data2b = std::make_shared<data::boolean>();

        gen_data_srv->set_output("out2", data2b);
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
        srv2->reset_is_updated();
        CPPUNIT_ASSERT(!srv2->get_is_updated());
        auto modified_sig2 = data2b->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        modified_sig2->async_emit();
        SIGHT_TEST_WAIT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv2->get_is_updated());

        // Overwrite data 2 with a new data generated by an another service
        data::boolean::sptr data2bis = std::make_shared<data::boolean>();

        auto gen_data_srv2 =
            std::dynamic_pointer_cast<app::ut::test_service>(core::tools::id::get_object("SGenerateData2"));
        CPPUNIT_ASSERT(gen_data_srv2 != nullptr);

        gen_data_srv2->set_output("out", data2bis);
        SIGHT_TEST_WAIT(
            !srv2->input(
                "data2"
            ).expired() && srv2->input(
                "data2"
            ).lock() == data2bis
        );

        CPPUNIT_ASSERT(srv2->input("data2").lock() == data2bis);

        SIGHT_TEST_WAIT("data2" == srv2->get_swapped_object_key() && data2bis == srv2->get_swapped_object());
        CPPUNIT_ASSERT("data2" == srv2->get_swapped_object_key());
        CPPUNIT_ASSERT(data2bis == srv2->get_swapped_object());

        // Check that the output of SGenerateData changed as well
        SIGHT_TEST_WAIT(data2bis == gen_data_srv->data::has_data::output("out2").lock().get_shared());
        CPPUNIT_ASSERT(data2bis == gen_data_srv->data::has_data::output("out2").lock().get_shared());

        // Revert that
        gen_data_srv2->set_output("out", data2b);
        SIGHT_TEST_WAIT(
            !srv2->input("data2").expired()
            && srv2->input("data2").lock() == data2b
        );

        CPPUNIT_ASSERT(srv2->input("data2").lock() == data2b);

        SIGHT_TEST_WAIT("data2" == srv2->get_swapped_object_key() && data2b == srv2->get_swapped_object());
        CPPUNIT_ASSERT("data2" == srv2->get_swapped_object_key());
        CPPUNIT_ASSERT(data2b == srv2->get_swapped_object());

        // Check that the output of SGenerateData changed as well
        SIGHT_TEST_WAIT(data2b == gen_data_srv->data::has_data::object("out2", data::access::out));
        CPPUNIT_ASSERT(data2b == gen_data_srv->data::has_data::object("out2", data::access::out));
    }
}

//------------------------------------------------------------------------------

void config_test::key_group_test()
{
    m_app_config_mgr = this->launch_app_config_mgr("keyGroupTest");

    // Service used to generate data
    auto gen_data_srv = std::dynamic_pointer_cast<app::ut::test_out>(core::tools::id::get_object("SGenerateData"));
    CPPUNIT_ASSERT(gen_data_srv != nullptr);

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
        gen_data_srv->set_output("out2", data2b);
        wait_service_started("TestService1Uid");

        gn_srv1 = core::tools::id::get_object("TestService1Uid");
        auto srv1 = std::dynamic_pointer_cast<app::ut::test1_input1_input_group>(gn_srv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv1->status());
        CPPUNIT_ASSERT(!srv1->get_is_updated());

        CPPUNIT_ASSERT(srv1->input("data1").lock() == data1);
        CPPUNIT_ASSERT(srv1->input("dataGroup", 0).lock() == data2b);
        CPPUNIT_ASSERT(srv1->input("dataGroup", 1).expired());
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), srv1->m_input_group.size());

        // Check connection with data 2
        CPPUNIT_ASSERT(!srv1->get_is_updated());
        auto sig2 = data2b->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig2->async_emit();
        SIGHT_TEST_WAIT(srv1->get_is_updated());
        CPPUNIT_ASSERT(srv1->get_is_updated());

        // Create data 3
        data3 = std::make_shared<data::image>();

        srv1->reset_is_updated();

        // Create a slot to wait for the swap to be completed
        bool srv1_swapped = false;
        std::function fn  = [&]()
                            {
                                srv1_swapped = true;
                            };
        auto swapped_slot = core::com::new_slot(fn);
        swapped_slot->set_worker(core::thread::get_default_worker());
        core::com::connection connection = srv1->signal(service::signals::SWAPPED)->connect(swapped_slot);

        gen_data_srv->set_output("out3", data3);

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
        CPPUNIT_ASSERT(2 == srv1->m_input_group.size());

        SIGHT_TEST_WAIT(srv1_swapped);
        CPPUNIT_ASSERT(srv1_swapped);

        // Check connection with data 3
        auto sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig3->async_emit();
        SIGHT_TEST_WAIT(srv1->get_is_updated());
        CPPUNIT_ASSERT(srv1->get_is_updated());

        connection.disconnect();
    }

    // Remove data 2
    {
        gen_data_srv->set_output("out2", nullptr);

        SIGHT_TEST_WAIT(false == core::tools::id::exist("TestService1Uid"));

        core::tools::object::sptr gn_srv5 = core::tools::id::get_object("TestService1Uid");
        CPPUNIT_ASSERT(gn_srv5 == nullptr);
    }

    {
        wait_service_started("TestService2Uid");
        core::tools::object::sptr gn_srv2 = core::tools::id::get_object("TestService2Uid");
        auto srv2                         = std::dynamic_pointer_cast<app::ut::test2_input_groups>(gn_srv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv2->status());
        CPPUNIT_ASSERT(!srv2->get_is_updated());

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
        srv2->reset_is_updated();
        auto sig1 = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig1->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv2->get_is_updated());

        // Check no connection with data 3
        srv2->reset_is_updated();
        auto sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig3->async_emit();
        SIGHT_TEST_WAIT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv2->get_is_updated());

        // Check connection with data 4
        srv2->reset_is_updated();
        auto sig4 = data4->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig4->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated(), 2500);
        CPPUNIT_ASSERT(!srv2->get_is_updated());
        auto sig_im4 = data4->signal<data::image::buffer_modified_signal_t>(data::image::BUFFER_MODIFIED_SIG);
        sig_im4->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated());
        CPPUNIT_ASSERT(srv2->get_is_updated());

        // Check no connection with data 5
        srv2->reset_is_updated();
        auto sig5 = data5->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig5->async_emit();
        SIGHT_TEST_WAIT(!srv2->get_is_updated());
        CPPUNIT_ASSERT(!srv2->get_is_updated());

        auto sig_im5 = data5->signal<data::image::buffer_modified_signal_t>(data::image::BUFFER_MODIFIED_SIG);
        sig_im5->async_emit();
        SIGHT_TEST_WAIT(srv2->get_is_updated(), 2500);
        CPPUNIT_ASSERT(!srv2->get_is_updated());
    }

    // Test output data group
    {
        core::tools::object::sptr gn_srv3 = core::tools::id::get_object("TestService3Uid");
        CPPUNIT_ASSERT(gn_srv3 == nullptr);

        auto data6 = std::make_shared<data::image>();
        gen_data_srv->m_out_group[0] = data6;

        gn_srv3 = core::tools::id::get_object("TestService3Uid");
        CPPUNIT_ASSERT(gn_srv3 == nullptr);

        auto data7 = std::make_shared<data::image>();
        gen_data_srv->m_out_group[1] = data7;

        wait_service_started("TestService3Uid");
        gn_srv3 = core::tools::id::get_object("TestService3Uid");
        auto srv3 = std::dynamic_pointer_cast<app::ut::test1_input1_input_group>(gn_srv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(service::base::global_status::started, srv3->status());

        CPPUNIT_ASSERT(srv3->input("dataGroup", 0).lock() == data6);
        CPPUNIT_ASSERT(srv3->input("dataGroup", 1).lock() == data7);

        gen_data_srv->m_out_group[0] = nullptr;
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
        futures.push_back(std::async(std::launch::async, parameters_config_test));
    }

    for(auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }

    app::extension::config::get_default()->clear_registry();
    std::vector<std::string> all_configs = app::extension::config::get_default()->get_all_configs();
    CPPUNIT_ASSERT(all_configs.empty());
}

//------------------------------------------------------------------------------

void config_test::parameter_replace_test()
{
    m_app_config_mgr = this->launch_app_config_mgr("parameterReplaceTest", true);

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
    auto srv2           = std::dynamic_pointer_cast<service::base>(gn_srv2);
    auto adapted_config = srv2->get_config();

    const auto params = adapted_config.equal_range("parameter");

    std::vector<service::config_t> params_cfg;
    std::for_each(params.first, params.second, [&params_cfg](const auto& _p){params_cfg.push_back(_p.second);});

    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), params_cfg.size());

    std::string replace_by;
    CPPUNIT_ASSERT_EQUAL(std::string("patient"), params_cfg[0].get<std::string>("<xmlattr>.replace"));
    CPPUNIT_ASSERT_EQUAL(std::string("name"), params_cfg[0].get<std::string>("<xmlattr>.by"));

    replace_by = params_cfg[1].get<std::string>("<xmlattr>.by");
    CPPUNIT_ASSERT_EQUAL(std::string("parameterReplaceTest_" + std::to_string(i) + "_Channel No5"), replace_by);

    replace_by = params_cfg[2].get<std::string>("<xmlattr>.by");
    CPPUNIT_ASSERT_EQUAL(std::string("parameterReplaceTest_" + std::to_string(i) + "_disneyChannel"), replace_by);

    replace_by = params_cfg[3].get<std::string>("<xmlattr>.by");
    CPPUNIT_ASSERT_EQUAL(std::string("parameterReplaceTest_" + std::to_string(i) + "_view1"), replace_by);

    core::tools::object::sptr gn_sub_srv;

    // Not really elegant, but we have to "guess" how it is replaced
    while(gn_sub_srv == nullptr && j++ < 200)
    {
        gn_sub_srv = core::tools::id::get_object(
            "parameterReplaceTestSubConfig_" + std::to_string(j)
            + "_TestServiceUid"
        );
    }

    auto srv_in_sub_config = std::dynamic_pointer_cast<app::ut::test_service>(gn_sub_srv);
    CPPUNIT_ASSERT(srv_in_sub_config != nullptr);

    CPPUNIT_ASSERT(srv_in_sub_config->started());

    auto data1 = srv1->input("data1").lock();
    CPPUNIT_ASSERT(data1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(std::string("data1Id"), data1->get_id());

    auto data2 = srv1->input("data2").lock();
    CPPUNIT_ASSERT(data2 != nullptr);

    auto data1_sub_srv = srv_in_sub_config->input("data1").lock();
    CPPUNIT_ASSERT(data1 == data1_sub_srv);

    auto data2_sub_srv = srv_in_sub_config->input("data2").lock();
    CPPUNIT_ASSERT(data2 == data2_sub_srv);

    // check connections through the subconfig channel
    CPPUNIT_ASSERT(!srv1->get_is_updated());

    auto sig1 = data1_sub_srv->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig1->async_emit();

    SIGHT_TEST_WAIT(srv1->get_is_updated());
    CPPUNIT_ASSERT(srv1->get_is_updated());

    CPPUNIT_ASSERT(!srv_in_sub_config->get_is_updated());

    auto sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig2->async_emit();

    SIGHT_TEST_WAIT(srv_in_sub_config->get_is_updated());
    CPPUNIT_ASSERT(srv_in_sub_config->get_is_updated());
}

//------------------------------------------------------------------------------

void config_test::object_config_test()
{
    m_app_config_mgr = this->launch_app_config_mgr("objectConfigTest");

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
    CPPUNIT_ASSERT_EQUAL(service::base::configuration_status::configured, srv1->config_status());

    auto srv_data1 = srv1->input<data::composite>("data1");
    CPPUNIT_ASSERT(!srv_data1.expired());
    CPPUNIT_ASSERT(srv_data1.lock() == compo1);

    auto config = srv1->get_config();
    CPPUNIT_ASSERT_EQUAL(std::string("value1"), config.get<std::string>("param1"));
    CPPUNIT_ASSERT_EQUAL(std::string("value2"), config.get<std::string>("param2"));
}

//------------------------------------------------------------------------------

service::config_t config_test::build_config()
{
    service::config_t cfg;

    // Object
    service::config_t obj_cfg;
    obj_cfg.add("<xmlattr>.uid", "image");
    obj_cfg.add("<xmlattr>.type", "sight::data::image");
    cfg.add_child("object", obj_cfg);

    // Service
    service::config_t srv_cfg;
    srv_cfg.add("<xmlattr>.uid", "myTestService1");
    srv_cfg.add("<xmlattr>.type", "sight::app::ut::test1_image");
    srv_cfg.add("<xmlattr>.auto_connect", "false");
    cfg.add_child("service", srv_cfg);

    // Connections
    service::config_t connect_cfg;
    connect_cfg.add("<xmlattr>.channel", "channel1");
    connect_cfg.add("signal", "image/modified");
    connect_cfg.add("slot", "myTestService1/update");
    cfg.add_child("connect", connect_cfg);

    // Start method from object's services
    service::config_t start_cfg;
    start_cfg.add("<xmlattr>.uid", "myTestService1");
    cfg.add_child("start", start_cfg);

    // Update method from object's services
    service::config_t update_cfg;
    update_cfg.add("<xmlattr>.uid", "myTestService1");
    cfg.add_child("update", update_cfg);

    return cfg;
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
