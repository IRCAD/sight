/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "helper.hpp"

#include "service/extension/config.hpp"

#include "test_service.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>
#include <core/time_stamp.hpp>

#include <data/boolean.hpp>
#include <data/dvec3.hpp>
#include <data/extension/config.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/ivec3.hpp>
#include <data/matrix4.hpp>
#include <data/string.hpp>
#include <data/transfer_function.hpp>

#include <utest/wait.hpp>

#include <app/config_manager.hpp>

#include <doctest/doctest.h>

#include <filesystem>

//------------------------------------------------------------------------------

static sight::service::config_t build_config()
{
    sight::service::config_t cfg;

    // Object
    sight::service::config_t obj_cfg;
    obj_cfg.add("<xmlattr>.uid", "image");
    obj_cfg.add("<xmlattr>.type", "sight::data::image");
    cfg.add_child("object", obj_cfg);

    // Service
    sight::service::config_t srv_cfg;
    srv_cfg.add("<xmlattr>.uid", "myTestService1");
    srv_cfg.add("<xmlattr>.type", "sight::app::ut::test1_image");
    srv_cfg.add("<xmlattr>.auto_connect", "false");
    cfg.add_child("service", srv_cfg);

    // Connections
    sight::service::config_t connect_cfg;
    connect_cfg.add("<xmlattr>.channel", "channel1");
    connect_cfg.add("signal", "image/modified");
    connect_cfg.add("slot", "myTestService1/update");
    cfg.add_child("connect", connect_cfg);

    // Update method from object's services
    sight::service::config_t update_cfg;
    update_cfg.add("<xmlattr>.uid", "myTestService1");
    cfg.add_child("update", update_cfg);

    return cfg;
}

//------------------------------------------------------------------------------
namespace
{

struct fixture
{
    fixture()
    {
        // Set up context before running a test.
        sight::core::runtime::init();

        std::filesystem::path location = sight::core::runtime::get_resource_file_path("app_ut");
        CHECK(std::filesystem::exists(location));

        sight::core::runtime::add_modules(location);
        sight::core::runtime::load_module("sight::module::app");
        sight::core::runtime::load_module("sight::module::data");
        sight::core::runtime::load_module("config_test");

        auto app_config = sight::app::extension::config::get();
        app_config->clear_registry();
        app_config->parse_plugin_infos();

        auto srv_config = sight::service::extension::config::get_default();
        srv_config->clear_registry();
        srv_config->parse_plugin_infos();

        auto data_config = sight::data::extension::config::get();
        data_config->clear_registry();
        data_config->parse_plugin_infos();
    }

    ~fixture()
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

    sight::app::config_manager::sptr m_app_config_mgr;
};

} // namespace

TEST_SUITE("sight::app::config")
{
//------------------------------------------------------------------------------

    static inline void wait_service_started(const std::string& _srv)
    {
        auto service = sight::core::id::get_object(_srv);
        SIGHT_TEST_WAIT(service != nullptr && std::dynamic_pointer_cast<sight::service::base>(service)->started());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "add_config_test")
    {
        auto current_app_config = sight::app::extension::config::get();

        const std::string config_id(sight::app::extension::config::get_unique_identifier());
        const std::string group("TestGroup");
        const std::string desc("Description");
        const std::string module_id("sight::module::service");
        sight::app::extension::app_info::parameters_t parameters;
        sight::app::extension::app_info::objects_t objects;

        const sight::service::config_t config = build_config();

        current_app_config->add_app_info(config_id, group, desc, parameters, objects, config, module_id);

        std::vector<std::string> all_configs = current_app_config->get_all_configs();
        CHECK_EQ(false, all_configs.empty());

        std::vector<std::string> configs = current_app_config->get_configs_from_group(group);
        CHECK_EQ(std::size_t(1), configs.size());
        CHECK_EQ(config_id, configs.front());

        auto module = current_app_config->get_module(config_id);
        CHECK(module);
        CHECK_EQ(module_id, module->identifier());

        sight::app::field_adaptor_t replace_fields;

        sight::core::runtime::config_t config_adapted = current_app_config->get_adapted_template_config(
            config_id,
            replace_fields,
            ""
        );
        const auto uid = config_adapted.get<std::string>("object.<xmlattr>.uid");
        CHECK_EQ(std::string("image"), uid);

        const auto service_uid1 = config_adapted.get<std::string>("service.<xmlattr>.uid");
        CHECK_EQ(std::string("myTestService1"), service_uid1);
    }

//------------------------------------------------------------------------------

    static void parameters_config_test()
    {
        auto current_app_config = sight::app::extension::config::get();

        const std::string config_id("parametersConfigTest1");

        sight::app::field_adaptor_t replace_fields;
        replace_fields["TEST_IMAGE"] = "objectUUID";

        const auto all_configs = current_app_config->get_all_configs();
        const auto it          = std::ranges::find(all_configs, config_id);
        CHECK(it != all_configs.end());

        sight::core::runtime::config_t config_adapted = current_app_config->get_adapted_template_config(
            config_id,
            replace_fields,
            ""
        );

        const auto uid = config_adapted.get<std::string>("object.<xmlattr>.uid");
        CHECK_EQ(std::string("objectUUID"), uid);

        auto services_cfg = config_adapted.equal_range("service");

        const auto service_uid1 = services_cfg.first->second.get<std::string>("<xmlattr>.uid");
        CHECK_EQ(std::string("myTestService1"), service_uid1);

        services_cfg.first++;
        const auto service_uid2 = services_cfg.first->second.get<std::string>("<xmlattr>.uid");
        CHECK_EQ(std::string("myTestService2"), service_uid2);
    }

    TEST_CASE_FIXTURE(fixture, "parameters_config_test")
    {
        parameters_config_test();
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "start_stop_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("startStopTest");

        // =================================================================================================================
        // Test manual start and stop of services, with or without data
        // =================================================================================================================

        auto data1 = std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("data1Id"));
        CHECK(data1 != nullptr);

        // This service doesn't exist in the config
        CHECK(sight::core::id::get_object("TestService142Uid") == nullptr);

        auto gen_data_srv =
            std::dynamic_pointer_cast<sight::app::ut::test_service>(sight::core::id::get_object("SGenerateData"));
        CHECK(gen_data_srv != nullptr);

        // This service has no data and is started by the config
        {
            sight::core::object::sptr gn_srv1 = sight::core::id::get_object("TestService1Uid");
            auto srv1                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv1);
            CHECK(srv1 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv1->status());
        }

        // This service has no data and is NOT started by the config
        {
            sight::core::object::sptr gn_srv2 = sight::core::id::get_object("TestService2Uid");
            auto srv2                         = std::dynamic_pointer_cast<sight::service::base>(gn_srv2);
            CHECK(srv2 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv2->status());
            srv2->stop().get();
            CHECK_EQ(sight::service::base::global_status::stopped, srv2->status());
            srv2->start().get();
            CHECK_EQ(sight::service::base::global_status::started, srv2->status());
        }

        // This service has a data and is NOT started by the config
        {
            sight::core::object::sptr gn_srv3 = sight::core::id::get_object("TestService3Uid");
            auto srv3                         = std::dynamic_pointer_cast<sight::service::base>(gn_srv3);
            CHECK(srv3 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv3->status());
        }

        // This service has a data that is not present yet (WID), so it is even not created
        {
            sight::core::object::sptr gn_srv4 = sight::core::id::get_object("TestService4Uid");
            CHECK(gn_srv4 == nullptr);
        }

        // =================================================================================================================
        // Test automatic start and stop of the service, with a single data
        // =================================================================================================================

        // Create the data
        sight::data::boolean::sptr data2 = std::make_shared<sight::data::boolean>();
        {
            gen_data_srv->set_output(data2, "out2");
            wait_service_started("TestService4Uid");

            // Now the service should have been started automatically
            {
                auto gn_srv4 = sight::core::id::get_object("TestService4Uid");
                auto srv4    = std::dynamic_pointer_cast<sight::service::base>(gn_srv4);
                CHECK(srv4 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv4->status());
            }

            // Remove the data
            gen_data_srv->set_output(nullptr, "out2");
            SIGHT_TEST_WAIT(sight::core::id::exist("TestService4Uid") == false);

            // Now the service should have been stopped and destroyed automatically
            {
                auto gn_srv4 = sight::core::id::get_object("TestService4Uid");
                CHECK(gn_srv4 == nullptr);
            }

            // Register the data once again
            gen_data_srv->set_output(data2, "out2");
            wait_service_started("TestService4Uid");

            // Check again that the service was started automatically
            {
                auto gn_srv4 = sight::core::id::get_object("TestService4Uid");
                auto srv4    = std::dynamic_pointer_cast<sight::service::base>(gn_srv4);
                CHECK(srv4 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv4->status());
            }
        }

        // =================================================================================================================
        // Test automatic start and stop of the service, with multiple data (3)
        // =================================================================================================================

        {
            // Still one data is not yet available thus the service is not created
            {
                sight::core::object::sptr gn_srv5 = sight::core::id::get_object("TestService5Uid");
                CHECK(gn_srv5 == nullptr);
            }

            // Create the remaining data
            sight::data::boolean::sptr data4 = std::make_shared<sight::data::boolean>();

            gen_data_srv->set_output(data4, "out4");
            wait_service_started("TestService5Uid");

            // Now the service should have been started automatically
            {
                auto gn_srv5 = sight::core::id::get_object("TestService5Uid");
                auto srv5    = std::dynamic_pointer_cast<sight::service::base>(gn_srv5);
                CHECK(srv5 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv5->status());
            }

            // Remove one data
            gen_data_srv->set_output(nullptr, "out2");

            // Now the service should have been stopped and destroyed automatically
            {
                SIGHT_TEST_WAIT(sight::core::id::exist("TestService5Uid") == false);
                auto gn_srv5 = sight::core::id::get_object("TestService5Uid");
                CHECK(gn_srv5 == nullptr);

                // Test as well service 4, just to be sure
                SIGHT_TEST_WAIT(sight::core::id::exist("TestService4Uid") == false);
                auto gn_srv4 = sight::core::id::get_object("TestService4Uid");
                CHECK(gn_srv4 == nullptr);
            }

            // Put everything back
            sight::app::ut::test_service::s_start_counter  = 0;
            sight::app::ut::test_service::s_update_counter = 0;
            gen_data_srv->set_output(data2, "out2");
            wait_service_started("TestService5Uid");

            // Now the service should have been started automatically, check start order as well
            {
                auto gn_srv5 = sight::core::id::get_object("TestService5Uid");
                auto srv5    = std::dynamic_pointer_cast<sight::app::ut::test_srv>(gn_srv5);
                CHECK(srv5 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv5->status());

                // We have started, yet we may not have been updated, so wait for it just in case
                SIGHT_TEST_WAIT(1U == srv5->get_update_order());
                CHECK_EQ(0U, srv5->get_start_order());
                CHECK_EQ(1U, srv5->get_update_order());

                // Test as well service 4, just to be sure
                auto gn_srv4 = sight::core::id::get_object("TestService4Uid");
                auto srv4    = std::dynamic_pointer_cast<sight::app::ut::test_srv>(gn_srv4);
                CHECK(gn_srv4 != nullptr);
                CHECK_EQ(1U, srv4->get_start_order());
                CHECK_EQ(0U, srv4->get_update_order());
            }
        }

        // =================================================================================================================
        // Test data swapping
        // =================================================================================================================

        {
            // Test initial status (started because of the previous test)
            {
                auto gn_srv5 = sight::core::id::get_object("TestService5Uid");
                auto srv5    = std::dynamic_pointer_cast<sight::service::base>(gn_srv5);
                CHECK(srv5 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv5->status());
            }

            // Swap the data
            sight::data::boolean::sptr data5 = std::make_shared<sight::data::boolean>();

            gen_data_srv->set_output(nullptr, "out2");
            SIGHT_TEST_WAIT(sight::core::id::exist("TestService5Uid") == false);
            gen_data_srv->set_output(data5, "out2");
            wait_service_started("TestService5Uid");

            {
                auto gn_srv5 = sight::core::id::get_object("TestService5Uid");
                auto srv5    = std::dynamic_pointer_cast<sight::service::base>(gn_srv5);
                CHECK(srv5 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv5->status());
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "auto_connect_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("autoConnectTest");

        // =================================================================================================================
        // Test autoconnect with available data
        // =================================================================================================================

        auto data1 = std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("data1Id"));
        auto data2 = std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("data2Id"));
        CHECK(data1 != nullptr);

        {
            sight::core::object::sptr gn_srv1 = sight::core::id::get_object("TestService1Uid");
            auto srv1                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv1);
            CHECK(srv1 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv1->status());
            CHECK(!srv1->is_updated());

            sight::core::object::sptr gn_srv2 = sight::core::id::get_object("TestService2Uid");
            auto srv2                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv2);
            CHECK(srv2 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv2->status());
            CHECK(!srv2->is_updated());

            sight::core::object::sptr gn_srv3 = sight::core::id::get_object("TestService3Uid");
            auto srv3                         = std::dynamic_pointer_cast<sight::app::ut::test_srv>(gn_srv3);
            CHECK(srv3 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv3->status());
            CHECK(!srv3->is_updated());
            CHECK(!srv3->get_received());

            data1->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated() && srv3->is_updated());
            CHECK(!srv1->is_updated());
            CHECK(srv2->is_updated());
            CHECK(srv3->is_updated());
            CHECK(!srv3->get_received());

            srv2->reset_is_updated();
            srv3->reset_is_updated();

            CHECK(!srv2->is_updated());
            CHECK(!srv3->is_updated());

            data2->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated() && srv3->get_received());
            CHECK(!srv1->is_updated());
            CHECK(srv2->is_updated());
            CHECK(!srv3->is_updated());
            CHECK(srv3->get_received());
        }

        // =================================================================================================================
        // Test autoconnect with unavailable data at start
        // =================================================================================================================

        // Service used to generate data
        auto gen_data_srv =
            std::dynamic_pointer_cast<sight::app::ut::test_service>(sight::core::id::get_object("SGenerateData"));
        CHECK(gen_data_srv != nullptr);
        {
            // Check that dependent services are not created
            {
                sight::core::object::sptr gn_srv3 = sight::core::id::get_object("TestService4Uid");
                CHECK(gn_srv3 == nullptr);
                sight::core::object::sptr gn_srv5 = sight::core::id::get_object("TestService5Uid");
                CHECK(gn_srv5 == nullptr);
            }

            // Create the data
            sight::data::boolean::sptr data3 = std::make_shared<sight::data::boolean>();
            gen_data_srv->set_output(data3, "out3");

            wait_service_started("TestService4Uid");
            wait_service_started("TestService5Uid");
            {
                sight::core::object::sptr gn_srv4 = sight::core::id::get_object("TestService4Uid");
                auto srv4                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv4);
                CHECK(srv4 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv4->status());
                CHECK(!srv4->is_updated());

                sight::core::object::sptr gn_srv5 = sight::core::id::get_object("TestService5Uid");
                auto srv5                         = std::dynamic_pointer_cast<sight::app::ut::test_srv>(gn_srv5);
                CHECK(srv5 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv5->status());

                CHECK(!srv4->is_updated());
                CHECK(!srv5->is_updated());
                CHECK(!srv5->get_received());

                data3->async_emit(sight::data::signals::MODIFIED);
                SIGHT_TEST_WAIT(srv5->get_received());

                CHECK(!srv4->is_updated());
                CHECK(!srv5->is_updated());
                CHECK(srv5->get_received());
            }

            // Remove one data
            gen_data_srv->set_output(nullptr, "out3");
            SIGHT_TEST_WAIT(
                sight::core::id::exist("TestService4Uid") == false
                && sight::core::id::exist("TestService5Uid") == false
            );
            {
                sight::core::object::sptr gn_srv4 = sight::core::id::get_object("TestService4Uid");
                CHECK(gn_srv4 == nullptr);
                sight::core::object::sptr gn_srv5 = sight::core::id::get_object("TestService5Uid");
                CHECK(gn_srv5 == nullptr);
            }

            // Emit, that should be ok
            gen_data_srv->set_output(data3, "out3");

            wait_service_started("TestService4Uid");
            wait_service_started("TestService5Uid");
            {
                sight::core::object::sptr gn_srv4 = sight::core::id::get_object("TestService4Uid");
                auto srv4                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv4);
                CHECK(srv4 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv4->status());
                CHECK(!srv4->is_updated());

                sight::core::object::sptr gn_srv5 = sight::core::id::get_object("TestService5Uid");
                auto srv5                         = std::dynamic_pointer_cast<sight::app::ut::test_srv>(gn_srv5);
                CHECK(srv5 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv5->status());

                CHECK(!srv4->is_updated());
                CHECK(!srv5->is_updated());
                CHECK(!srv5->get_received());

                data3->async_emit(sight::data::signals::MODIFIED);
                SIGHT_TEST_WAIT(srv5->get_received());
                CHECK(srv5->get_received());

                CHECK(!srv4->is_updated());
                CHECK(!srv5->is_updated());
                CHECK(srv5->get_received());

                data1->async_emit(sight::data::signals::MODIFIED);
                SIGHT_TEST_WAIT(srv5->is_updated());

                CHECK(srv5->is_updated());
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "connection_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("connectionTest");

        // =================================================================================================================
        // Test connection without data
        // =================================================================================================================

        sight::data::map::sptr map;

        auto data1 = std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("data1Id"));
        CHECK(data1 != nullptr);

        // =================================================================================================================
        // Test connection without data or with available data
        // =================================================================================================================

        sight::core::object::sptr gn_srv1 = sight::core::id::get_object("TestService1Uid");
        auto srv1                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv1);
        CHECK(srv1 != nullptr);
        CHECK_EQ(sight::service::base::global_status::started, srv1->status());
        CHECK(!srv1->is_updated());

        sight::core::object::sptr gn_srv2 = sight::core::id::get_object("TestService2Uid");
        auto srv2                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv2);
        CHECK(srv2 != nullptr);
        CHECK_EQ(sight::service::base::global_status::started, srv2->status());
        CHECK(!srv2->is_updated());

        sight::core::object::sptr gn_srv4 = sight::core::id::get_object("TestService4Uid");
        auto srv4                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv4);
        CHECK(srv4 != nullptr);
        SIGHT_TEST_WAIT(sight::service::base::global_status::started == srv4->status());
        CHECK_EQ(sight::service::base::global_status::started, srv4->status());

        // Check connection
        data1->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(srv1->is_updated() && srv2->is_updated());

        CHECK(srv1->is_updated());
        CHECK(srv2->is_updated());

        // Service used to generate data
        auto gen_data_srv =
            std::dynamic_pointer_cast<sight::app::ut::test_service>(sight::core::id::get_object("SGenerateData"));
        CHECK(gen_data_srv != nullptr);

        // =================================================================================================================
        // Test connection with unavailable data at start
        // =================================================================================================================

        {
            sight::core::object::sptr gn_srv3 = sight::core::id::get_object("TestService3Uid");
            CHECK(gn_srv3 == nullptr);
        }

        // Emit a signal just for fun, anyway the service doesn't exist
        data1->emit(sight::data::signals::MODIFIED);

        srv2->reset_is_updated();
        CHECK(!srv2->is_updated());

        // Check connection data4 -> srv2
        auto data4 = std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("data4Id"));
        CHECK(data4 != nullptr);
        data4->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(srv2->is_updated());

        CHECK(srv2->is_updated());

        SIGHT_TEST_WAIT(!srv4->is_updated());
        CHECK(!srv4->is_updated());
        CHECK(!srv4->is_updated2());

        // Create the missing data
        sight::data::boolean::sptr data2 = std::make_shared<sight::data::boolean>();
        sight::data::boolean::sptr data3 = std::make_shared<sight::data::boolean>();
        gen_data_srv->set_output(data2, "out2");
        gen_data_srv->set_output(data3, "out3");
        wait_service_started("TestService3Uid");
        {
            sight::core::object::sptr gn_srv3 = sight::core::id::get_object("TestService3Uid");
            auto srv3                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv3);
            CHECK(srv3 != nullptr);
            SIGHT_TEST_WAIT(sight::service::base::global_status::started == srv3->status());
            CHECK_EQ(sight::service::base::global_status::started, srv3->status());
            srv2->reset_is_updated();
            CHECK(!srv2->is_updated());
            CHECK(!srv3->is_updated());

            // Check "started" signal
            SIGHT_TEST_WAIT(srv4->is_updated())
            CHECK(srv4->is_updated());
            CHECK(!srv4->is_updated2());

            data1->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated() && srv3->is_updated());

            CHECK(srv2->is_updated());
            CHECK(srv3->is_updated());

            srv1->reset_is_updated();
            srv3->reset_is_updated();
            CHECK(!srv1->is_updated());
            CHECK(!srv3->is_updated());

            data2->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv1->is_updated() && srv3->is_updated());

            CHECK(srv1->is_updated());
            CHECK(srv3->is_updated());

            // Check connection data4 -> srv2,srv3
            srv2->reset_is_updated();
            srv3->reset_is_updated();
            CHECK(!srv2->is_updated());
            CHECK(!srv3->is_updated());

            data4->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated() && srv3->is_updated());

            CHECK(srv2->is_updated());
            CHECK(srv3->is_updated());

            // Check connection data3 -> srv2,srv3
            srv2->reset_is_updated();
            srv3->reset_is_updated();
            CHECK(!srv2->is_updated());
            CHECK(!srv3->is_updated());

            data3->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated() && srv3->is_updated());

            CHECK(srv2->is_updated());
            CHECK(srv3->is_updated());
        }

        // Remove one data
        gen_data_srv->set_output(nullptr, "out3");

        SIGHT_TEST_WAIT(sight::core::id::exist("TestService3Uid") == false);

        // Service 3 should be removed
        {
            sight::core::object::sptr gn_srv3 = sight::core::id::get_object("TestService3Uid");
            CHECK(gn_srv3 == nullptr);
        }

        // Check that connection is still there for the working services
        srv2->reset_is_updated();
        CHECK(!srv2->is_updated());
        data1->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(srv2->is_updated());
        CHECK(srv2->is_updated());

        srv1->reset_is_updated();
        CHECK(!srv1->is_updated());
        data2->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(srv1->is_updated());
        CHECK(srv1->is_updated());

        // Check connection data4 -> srv2
        srv2->reset_is_updated();
        CHECK(!srv2->is_updated());

        data4->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(srv2->is_updated());

        CHECK(srv2->is_updated());

        // Check "stopped" signal
        CHECK(srv4->is_updated());
        CHECK(srv4->is_updated2());
        srv4->reset_is_updated();
        srv4->reset_is_updated2();
        CHECK(!srv4->is_updated2());

        // Add back data 3 and check connection again
        gen_data_srv->set_output(data3, "out3");
        wait_service_started("TestService3Uid");

        {
            sight::core::object::sptr gn_srv3 = sight::core::id::get_object("TestService3Uid");
            auto srv3                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv3);
            CHECK(srv3 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv3->status());
            srv2->reset_is_updated();
            CHECK(!srv2->is_updated());
            CHECK(!srv3->is_updated());

            SIGHT_TEST_WAIT(srv4->is_updated())
            CHECK(srv4->is_updated());
            CHECK(!srv4->is_updated2());

            data1->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated() && srv3->is_updated());

            CHECK(srv2->is_updated());
            CHECK(srv3->is_updated());

            srv1->reset_is_updated();
            srv3->reset_is_updated();
            CHECK(!srv1->is_updated());
            CHECK(!srv3->is_updated());

            data2->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv1->is_updated() && srv3->is_updated());

            CHECK(srv1->is_updated());
            CHECK(srv3->is_updated());

            // Check connection data4 -> srv2,srv3
            srv2->reset_is_updated();
            srv3->reset_is_updated();
            CHECK(!srv2->is_updated());
            CHECK(!srv3->is_updated());

            data4->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated() && srv3->is_updated());

            CHECK(srv2->is_updated());
            CHECK(srv3->is_updated());

            // Check connection data3 -> srv2,srv3
            srv2->reset_is_updated();
            srv3->reset_is_updated();
            CHECK(!srv2->is_updated());
            CHECK(!srv3->is_updated());

            data3->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated() && srv3->is_updated());

            CHECK(srv2->is_updated());
            CHECK(srv3->is_updated());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "start_stop_connection_test")
    {
        {
            m_app_config_mgr = sight::app::ut::launch_app_config_mgr("startStopConnectionTest");

            // Check TestService5 starts TestService6
            // Check TestService5 stops TestService6
            {
                sight::core::object::sptr gn_srv5 = sight::core::id::get_object("TestService5Uid");
                auto srv5                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv5);
                CHECK(srv5 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv5->status());

                {
                    sight::core::object::sptr gn_srv6 = sight::core::id::get_object("TestService6Uid");
                    auto srv6                         =
                        std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv6);
                    CHECK(srv6 != nullptr);
                    CHECK_EQ(sight::service::base::global_status::stopped, srv6->status());

                    srv5->update().get();
                    SIGHT_TEST_WAIT(srv6->started());
                    CHECK_EQ(true, srv6->started());

                    srv5->async_emit(sight::app::ut::test_srv::signals::SIG_1, 0);

                    SIGHT_TEST_WAIT(srv6->stopped());
                    CHECK_EQ(true, srv6->stopped());
                }
            }
            m_app_config_mgr->stop_and_destroy();
            m_app_config_mgr.reset();
        }

        {
            m_app_config_mgr = sight::app::ut::launch_app_config_mgr("startStopConnectionTest");

            // Check TestService5 starts TestService6
            // Check TestService5 stops TestService6
            // Check TestService6 is well stopped by the config_manager
            // Check TestService6 stops TestService7
            {
                sight::core::object::sptr gn_srv5 = sight::core::id::get_object("TestService5Uid");
                auto srv5                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv5);
                CHECK(srv5 != nullptr);
                CHECK_EQ(sight::service::base::global_status::started, srv5->status());

                {
                    sight::core::object::sptr gn_srv6 = sight::core::id::get_object("TestService6Uid");
                    auto srv6                         =
                        std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv6);
                    CHECK(srv6 != nullptr);
                    CHECK_EQ(sight::service::base::global_status::stopped, srv6->status());

                    srv5->update().get();
                    SIGHT_TEST_WAIT(srv6->started());
                    CHECK_EQ(true, srv6->started());

                    srv5->async_emit(sight::app::ut::test_srv::signals::SIG_1, 0);

                    SIGHT_TEST_WAIT(srv6->stopped());
                    CHECK_EQ(true, srv6->stopped());

                    srv5->update().get();
                    SIGHT_TEST_WAIT(srv6->started());
                    CHECK_EQ(true, srv6->started());

                    sight::core::object::sptr gn_srv7 = sight::core::id::get_object("TestService7Uid");
                    auto srv7                         =
                        std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv7);
                    srv6->update().get();
                    SIGHT_TEST_WAIT(srv7->stopped());
                    CHECK_EQ(true, srv7->stopped());
                }
            }
            m_app_config_mgr->stop_and_destroy();
            m_app_config_mgr.reset();
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "optional_key_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("optionalKeyTest");

        // Service used to generate data
        auto gen_data_srv =
            std::dynamic_pointer_cast<sight::app::ut::test_service>(sight::core::id::get_object("SGenerateData"));
        CHECK(gen_data_srv != nullptr);

        auto data1 = std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("data1Id"));
        CHECK(data1 != nullptr);

        // =================================================================================================================
        // Test service with two optional attributes and available data at start
        // =================================================================================================================

        sight::core::object::sptr gn_srv1 = sight::core::id::get_object("TestService1Uid");
        auto srv1                         = std::dynamic_pointer_cast<sight::app::ut::test_srv>(gn_srv1);
        CHECK(srv1 != nullptr);
        CHECK_EQ(sight::service::base::global_status::started, srv1->status());
        CHECK(!srv1->is_updated());

        // Check connection
        data1->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(srv1->is_updated());

        CHECK(srv1->is_updated());
        srv1->reset_is_updated();

        CHECK(srv1->input("data1").lock() == data1);
        CHECK(srv1->input("data2").expired());
        CHECK(srv1->input("data3").expired());

        // Create data 2
        sight::data::boolean::sptr data2 = std::make_shared<sight::data::boolean>();

        gen_data_srv->set_output(data2, "out2");
        SIGHT_TEST_WAIT(
            !srv1->input("data2").expired()
            && srv1->input("data2").lock() == data2
        );

        CHECK(srv1->input("data2").lock() == data2);
        CHECK(srv1->input("data3").expired());

        SIGHT_TEST_WAIT("data2" == srv1->get_swapped_object_key());
        CHECK_EQ(std::string("data2"), srv1->get_swapped_object_key());
        CHECK(data2 == srv1->get_swapped_object());

        // Check no connection with data 2
        CHECK(!srv1->is_updated());
        data2->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(!srv1->is_updated());
        CHECK(!srv1->is_updated());

        // Create data 3 and 4
        sight::data::boolean::sptr data3 = std::make_shared<sight::data::boolean>();
        sight::data::boolean::sptr data4 = std::make_shared<sight::data::boolean>();

        gen_data_srv->set_output(data3, "out3");
        gen_data_srv->set_output(data4, "out4");

        SIGHT_TEST_WAIT(
            !srv1->input("data3").expired()
            && !srv1->input("data4").expired()
            && srv1->input("data3").lock() == data3
            && srv1->input("data4").lock() == data4
        );

        CHECK(srv1->input("data3").lock() == data3);
        CHECK(srv1->input("data4").lock() == data4);
        CHECK_EQ(sight::service::base::global_status::started, srv1->status());

        // Check connection with data 3
        srv1->reset_is_updated();
        data3->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(srv1->is_updated());
        CHECK(srv1->is_updated());

        // Check connection with data 4
        srv1->reset_is_updated();
        data4->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(srv1->is_updated());
        CHECK(srv1->is_updated());

        // Remove data 2 and 3
        gen_data_srv->set_output(nullptr, "out2");
        SIGHT_TEST_WAIT(srv1->input("data2").expired());

        SIGHT_TEST_WAIT("data2" == srv1->get_swapped_object_key());
        CHECK_EQ(std::string("data2"), srv1->get_swapped_object_key());
        CHECK(nullptr == srv1->get_swapped_object());
        CHECK(srv1->input("data2").expired());

        gen_data_srv->set_output(nullptr, "out3");
        SIGHT_TEST_WAIT(
            !srv1->input(
                "data3"
            ).expired() && srv1->input("data3").lock() == nullptr
        );

        SIGHT_TEST_WAIT("data3" == srv1->get_swapped_object_key());
        CHECK_EQ(std::string("data3"), srv1->get_swapped_object_key());
        CHECK(nullptr == srv1->get_swapped_object());
        CHECK(srv1->input("data3").expired());

        CHECK_EQ(sight::service::base::global_status::started, srv1->status());

        // Create data 3
        gen_data_srv->set_output(data3, "out3");
        SIGHT_TEST_WAIT(
            !srv1->input(
                "data3"
            ).expired() && srv1->input("data3").lock() == data3
        );

        CHECK(srv1->input("data2").expired());
        CHECK(srv1->input("data3").lock() == data3);
        CHECK_EQ(sight::service::base::global_status::started, srv1->status());

        // =================================================================================================================
        // Test service with two optional attributes and unavailable data at start
        // =================================================================================================================

        // Create data 5
        sight::data::boolean::sptr data5 = std::make_shared<sight::data::boolean>();
        {
            sight::core::object::sptr gn_srv2 = sight::core::id::get_object("TestService2Uid");
            CHECK(gn_srv2 == nullptr);

            gen_data_srv->set_output(data5, "out5");
            wait_service_started("TestService2Uid");

            gn_srv2 = sight::core::id::get_object("TestService2Uid");
            CHECK(gn_srv2 != nullptr);
            auto srv2 = std::dynamic_pointer_cast<sight::app::ut::test_srv>(gn_srv2);
            CHECK(srv2 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv2->status());
            CHECK(!srv2->is_updated());

            CHECK(srv2->input("data1").lock() == data5);
            CHECK(srv2->input("data2").expired());
            CHECK(srv2->input("data3").lock() == data3);
            CHECK(srv2->input("data4").lock() == data4);

            // Check connection with data 4
            srv2->reset_is_updated();
            data4->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated());

            // Remove data 3 and 4
            gen_data_srv->set_output(nullptr, "out3");
            gen_data_srv->set_output(nullptr, "out4");

            SIGHT_TEST_WAIT(
                !srv2->input("data3").expired()
                && srv2->input("data4").expired()
            );

            CHECK(srv2->input("data1").lock() == data5);
            CHECK(srv2->input("data2").expired());
            CHECK(srv2->input("data3").expired());
            CHECK(srv2->input("data4").expired());

            // Create data 3
            gen_data_srv->set_output(data3, "out3");
            SIGHT_TEST_WAIT(!srv2->input("data3").expired());

            CHECK(srv2->input("data1").lock() == data5);
            CHECK(srv2->input("data2").expired());
            CHECK(srv2->input("data3").lock() == data3);
            CHECK(srv2->input("data4").expired());
        }

        // Remove data 5
        {
            gen_data_srv->set_output(nullptr, "out5");
            SIGHT_TEST_WAIT(false == sight::core::id::exist("TestService2Uid"));

            sight::core::object::sptr gn_srv5 = sight::core::id::get_object("TestService2Uid");
            CHECK(gn_srv5 == nullptr);
        }

        {
            // Create data 5
            gen_data_srv->set_output(data5, "out5");
            wait_service_started("TestService2Uid");

            auto gn_srv2 = sight::core::id::get_object("TestService2Uid");
            auto srv2    = std::dynamic_pointer_cast<sight::app::ut::test_srv>(gn_srv2);
            CHECK(srv2 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv2->status());
            CHECK(!srv2->is_updated());

            CHECK(srv2->input("data1").lock() == data5);
            CHECK(srv2->input("data2").expired());
            CHECK(srv2->input("data3").lock() == data3);
            CHECK(srv2->input("data4").expired());

            // Check connection with data 3
            srv2->reset_is_updated();
            data3->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated());
            CHECK(srv2->is_updated());

            // Create data 2
            sight::data::boolean::sptr data2b = std::make_shared<sight::data::boolean>();

            gen_data_srv->set_output(data2b, "out2");
            SIGHT_TEST_WAIT(
                !srv2->input(
                    "data2"
                ).expired()
                && srv2->input("data2").lock() == data2b
            );

            CHECK(srv2->input("data1").lock() == data5);
            CHECK(srv2->input("data2").lock() == data2b);
            CHECK(srv2->input("data3").lock() == data3);
            CHECK(srv2->input("data4").expired());

            // Check no connection with data 2
            srv2->reset_is_updated();
            CHECK(!srv2->is_updated());
            data2b->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(!srv2->is_updated());
            CHECK(!srv2->is_updated());

            // Overwrite data 2 with a new data generated by an another service
            sight::data::boolean::sptr data2bis = std::make_shared<sight::data::boolean>();

            auto gen_data_srv2 =
                std::dynamic_pointer_cast<sight::app::ut::test_service>(sight::core::id::get_object("SGenerateData2"));
            CHECK(gen_data_srv2 != nullptr);

            gen_data_srv2->set_output(data2bis, "out");
            SIGHT_TEST_WAIT(
                !srv2->input(
                    "data2"
                ).expired() && srv2->input(
                    "data2"
                ).lock() == data2bis
            );

            CHECK(srv2->input("data2").lock() == data2bis);

            SIGHT_TEST_WAIT("data2" == srv2->get_swapped_object_key() && data2bis == srv2->get_swapped_object());
            CHECK("data2" == srv2->get_swapped_object_key());
            CHECK(data2bis == srv2->get_swapped_object());

            // Check that the output of SGenerateData changed as well
            SIGHT_TEST_WAIT(data2bis == gen_data_srv->sight::data::has_data::output("out2").lock().get_shared());
            CHECK(data2bis == gen_data_srv->sight::data::has_data::output("out2").lock().get_shared());

            // Revert that
            gen_data_srv2->set_output(data2b, "out");
            SIGHT_TEST_WAIT(
                !srv2->input("data2").expired()
                && srv2->input("data2").lock() == data2b
            );

            CHECK(srv2->input("data2").lock() == data2b);

            SIGHT_TEST_WAIT("data2" == srv2->get_swapped_object_key() && data2b == srv2->get_swapped_object());
            CHECK("data2" == srv2->get_swapped_object_key());
            CHECK(data2b == srv2->get_swapped_object());

            // Check that the output of SGenerateData changed as well
            SIGHT_TEST_WAIT(data2b == gen_data_srv->sight::data::has_data::output("out2").lock().get_shared());
            CHECK(data2b == gen_data_srv->sight::data::has_data::output("out2").lock().get_shared());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "key_group_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("keyGroupTest");

        // Service used to generate data
        auto gen_data_srv =
            std::dynamic_pointer_cast<sight::app::ut::test_out>(sight::core::id::get_object("SGenerateData"));
        CHECK(gen_data_srv != nullptr);

        auto data1 = std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("data1Id"));
        CHECK(data1 != nullptr);

        auto data4 = std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("data4Id"));
        CHECK(data4 != nullptr);

        auto data5 = std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("data5Id"));
        CHECK(data5 != nullptr);

        // =================================================================================================================
        // Test service with one key group of two data
        // =================================================================================================================

        sight::data::image::sptr data3;
        {
            sight::core::object::sptr gn_srv1 = sight::core::id::get_object("TestService1Uid");
            CHECK(gn_srv1 == nullptr);

            // Create data 2b
            sight::data::boolean::sptr data2b = std::make_shared<sight::data::boolean>();
            gen_data_srv->set_output(data2b, "out2");
            wait_service_started("TestService1Uid");

            gn_srv1 = sight::core::id::get_object("TestService1Uid");
            auto srv1 = std::dynamic_pointer_cast<sight::app::ut::test1_input1_input_group>(gn_srv1);
            CHECK(srv1 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv1->status());
            CHECK(!srv1->is_updated());

            CHECK(srv1->input("data1").lock() == data1);
            CHECK(srv1->input("dataGroup", 0).lock() == data2b);
            CHECK(srv1->input("dataGroup", 1).expired());
            CHECK_EQ(std::size_t(2), srv1->m_input_group.size());

            // Check connection with data 2
            CHECK(!srv1->is_updated());
            data2b->async_emit(sight::data::signals::MODIFIED);

            SIGHT_TEST_WAIT(srv1->is_updated());
            CHECK(srv1->is_updated());

            // Create data 3
            data3 = std::make_shared<sight::data::image>();

            srv1->reset_is_updated();

            // Create a slot to wait for the swap to be completed
            bool srv1_swapped = false;
            auto swapped_slot = sight::core::com::new_slot([&](){srv1_swapped = true;});
            swapped_slot->set_worker(sight::core::thread::get_default_worker());
            sight::core::com::connection connection =
                srv1->signal(sight::service::signals::SWAPPED)->connect(swapped_slot);

            gen_data_srv->set_output(data3, "out3");

            SIGHT_TEST_WAIT(
                !srv1->input(
                    "dataGroup",
                    1
                ).expired() && srv1->input(
                    "dataGroup",
                    1
                ).lock() == data3
            );

            CHECK(srv1->input("dataGroup", 0).lock() == data2b);
            CHECK(srv1->input("dataGroup", 1).lock() == data3);
            CHECK(2 == srv1->m_input_group.size());

            SIGHT_TEST_WAIT(srv1_swapped);
            CHECK(srv1_swapped);

            // Check connection with data 3
            data3->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv1->is_updated());
            CHECK(srv1->is_updated());

            connection.disconnect();
        }

        // Remove data 2
        {
            gen_data_srv->set_output(nullptr, "out2");

            SIGHT_TEST_WAIT(false == sight::core::id::exist("TestService1Uid"));

            sight::core::object::sptr gn_srv5 = sight::core::id::get_object("TestService1Uid");
            CHECK(gn_srv5 == nullptr);
        }

        {
            wait_service_started("TestService2Uid");
            sight::core::object::sptr gn_srv2 = sight::core::id::get_object("TestService2Uid");
            auto srv2                         = std::dynamic_pointer_cast<sight::app::ut::test2_input_groups>(gn_srv2);
            CHECK(srv2 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv2->status());
            CHECK(!srv2->is_updated());

            SIGHT_TEST_WAIT(
                !srv2->input(
                    "dataGroup0",
                    1
                ).expired() && srv2->input(
                    "dataGroup0",
                    1
                ).lock() == data3
            );

            CHECK_EQ(std::size_t(1), srv2->m_input1.size());
            CHECK(srv2->input("dataGroup0", 0).lock() == data1);
            CHECK(srv2->input("dataGroup0", 1).expired());

            CHECK_EQ(std::size_t(3), srv2->m_input2.size());
            CHECK(srv2->input("dataGroup1", 0).lock() == data3);
            CHECK(srv2->input("dataGroup1", 1).lock() == data4);
            CHECK(srv2->input("dataGroup1", 2).lock() == data5);

            // Check connection with data 1
            srv2->reset_is_updated();
            data1->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated());
            CHECK(srv2->is_updated());

            // Check no connection with data 3
            srv2->reset_is_updated();
            data3->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(!srv2->is_updated());
            CHECK(!srv2->is_updated());

            // Check connection with data 4
            srv2->reset_is_updated();
            data4->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated(), 2500);
            CHECK(!srv2->is_updated());
            data4->async_emit(sight::data::image::signals::BUFFER_MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated());
            CHECK(srv2->is_updated());

            // Check no connection with data 5
            srv2->reset_is_updated();
            data5->async_emit(sight::data::signals::MODIFIED);
            SIGHT_TEST_WAIT(!srv2->is_updated());
            CHECK(!srv2->is_updated());

            data5->async_emit(sight::data::image::signals::BUFFER_MODIFIED);
            SIGHT_TEST_WAIT(srv2->is_updated(), 2500);
            CHECK(!srv2->is_updated());
        }

        // Test output data group
        {
            sight::core::object::sptr gn_srv3 = sight::core::id::get_object("TestService3Uid");
            CHECK(gn_srv3 == nullptr);

            auto data6 = std::make_shared<sight::data::image>();
            gen_data_srv->m_out_group[0] = data6;

            gn_srv3 = sight::core::id::get_object("TestService3Uid");
            CHECK(gn_srv3 == nullptr);

            auto data7 = std::make_shared<sight::data::image>();
            gen_data_srv->m_out_group[1] = data7;

            wait_service_started("TestService3Uid");
            gn_srv3 = sight::core::id::get_object("TestService3Uid");
            auto srv3 = std::dynamic_pointer_cast<sight::app::ut::test1_input1_input_group>(gn_srv3);
            CHECK(srv3 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv3->status());

            CHECK(srv3->input("dataGroup", 0).lock() == data6);
            CHECK(srv3->input("dataGroup", 1).lock() == data7);

            gen_data_srv->m_out_group[0] = nullptr;
        }
        {
            SIGHT_TEST_WAIT(false == sight::core::id::exist("TestService3Uid"));

            auto gn_srv3 = sight::core::id::get_object("TestService3Uid");
            CHECK(gn_srv3 == nullptr);
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "concurrent_access_to_config_test")
    {
        std::vector<std::future<void> > futures;
        futures.reserve(20);

        for(unsigned int i = 0 ; i < 20 ; ++i)
        {
            futures.emplace_back(std::async(std::launch::async, parameters_config_test));
        }

        for(auto& future : futures)
        {
            const auto status = future.wait_for(std::chrono::seconds(1));
            CHECK(status == std::future_status::ready);
            future.get(); // Trigger exceptions
        }

        sight::app::extension::config::get()->clear_registry();
        std::vector<std::string> all_configs = sight::app::extension::config::get()->get_all_configs();
        CHECK(all_configs.empty());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "parameter_replace_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("parameterReplaceTest", true);

        unsigned int i = 0;
        sight::core::object::sptr gn_srv1;
        sight::core::object::sptr gn_srv2;

        // Not really elegant, but we have to "guess" how it is replaced
        while(gn_srv1 == nullptr && i++ < 200)
        {
            gn_srv1 = sight::core::id::get_object("parameterReplaceTest", i, "TestService1Uid");
        }

        auto srv1 = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_srv1);
        CHECK(srv1 != nullptr);

        gn_srv2 = sight::core::id::get_object("parameterReplaceTest", i, "TestService2Uid");
        auto srv2           = std::dynamic_pointer_cast<sight::service::base>(gn_srv2);
        auto adapted_config = srv2->get_config();

        const auto params = adapted_config.equal_range("parameter");

        std::vector<sight::service::config_t> params_cfg;
        std::for_each(params.first, params.second, [&params_cfg](const auto& _p){params_cfg.push_back(_p.second);});

        CHECK_EQ(static_cast<std::size_t>(4), params_cfg.size());

        std::string replace_by;
        CHECK_EQ(std::string("patient"), params_cfg[0].get<std::string>("<xmlattr>.replace"));
        CHECK_EQ(std::string("name"), params_cfg[0].get<std::string>("<xmlattr>.by"));

        replace_by = params_cfg[1].get<std::string>("<xmlattr>.by");
        CHECK_EQ(sight::core::id::join("parameterReplaceTest", i, "Channel No5"), replace_by);

        replace_by = params_cfg[2].get<std::string>("<xmlattr>.by");
        CHECK_EQ(sight::core::id::join("parameterReplaceTest", i, "disneyChannel"), replace_by);

        replace_by = params_cfg[3].get<std::string>("<xmlattr>.by");
        CHECK_EQ(sight::core::id::join("parameterReplaceTest", i, "view1"), replace_by);

        // Not really elegant, but we have to "guess" how it is replaced
        sight::core::object::sptr gn_sub_srv;
        unsigned int j = 0;
        while(gn_sub_srv == nullptr && j++ < 200)
        {
            gn_sub_srv = sight::core::id::get_object("parameter_replace_test_sub_config", j, "test_service1_uid");
        }

        auto srv_in_sub_config = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_sub_srv);
        CHECK(srv_in_sub_config != nullptr);

        CHECK(srv_in_sub_config->started());

        auto data1 = srv1->input("data1").lock();
        CHECK(data1 != nullptr);
        CHECK_EQ(std::string("data1Id"), data1->get_id());

        auto data2 = srv1->input("data2").lock();
        CHECK(data2 != nullptr);

        auto data1_sub_srv = srv_in_sub_config->input("data1").lock();
        CHECK(data1 == data1_sub_srv);

        auto data2_sub_srv = srv_in_sub_config->input("data2").lock();
        CHECK(data2 == data2_sub_srv);

        // Not really elegant, but we have to "guess" how it is replaced
        sight::core::object::sptr gn_sub_srv2;
        j = 0;
        while(gn_sub_srv2 == nullptr && j++ < 200)
        {
            gn_sub_srv2 = sight::core::id::get_object("parameter_replace_test_sub_config", j, "test_service2_uid");
        }

        auto srv2_in_sub_config = std::dynamic_pointer_cast<sight::app::ut::test_service>(gn_sub_srv2);

        // Optional object, passed as parameter
        auto data3 = srv2_in_sub_config->input("data1").lock();
        CHECK(data3 != nullptr);
        auto str_data3 = std::dynamic_pointer_cast<const sight::data::string>(data3.get_shared());
        CHECK(str_data3->value() == "value");

        // Optional object, not passed as parameter
        auto data4 = srv2_in_sub_config->input("data2").lock();
        CHECK(data4 != nullptr);
        auto str_data4 = std::dynamic_pointer_cast<const sight::data::string>(data4.get_shared());
        CHECK(str_data4->value() == "default_value");

        // check connections through the subconfig channel
        CHECK(!srv1->is_updated());

        data1_sub_srv->async_emit(sight::data::signals::MODIFIED);

        SIGHT_TEST_WAIT(srv1->is_updated());
        CHECK(srv1->is_updated());

        CHECK(!srv_in_sub_config->is_updated());

        data2->async_emit(sight::data::signals::MODIFIED);

        SIGHT_TEST_WAIT(srv_in_sub_config->is_updated());
        CHECK(srv_in_sub_config->is_updated());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "optional_parameter_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("optional_parameter_test", true);

        // Verify that we manage to instantiate the same configuration twice with default optional parameters
        // Unique identifiers must be generated
        sight::core::object::sptr data1_in_config1;
        int j = 0;
        while(data1_in_config1 == nullptr && j++ < 200)
        {
            data1_in_config1 = sight::core::id::get_object("default_object_id_test_sub_config", j, "data1");
        }

        CHECK(data1_in_config1 != nullptr);
        {
            auto str = std::dynamic_pointer_cast<sight::data::string>(data1_in_config1);
            CHECK_EQ(std::string("default_value"), str->to_string());
        }

        sight::core::object::sptr data1_in_config2;
        while(data1_in_config2 == nullptr && j++ < 200)
        {
            data1_in_config2 = sight::core::id::get_object("default_object_id_test_sub_config", j, "data1");
        }

        CHECK(data1_in_config2 != nullptr);
        {
            auto str = std::dynamic_pointer_cast<sight::data::string>(data1_in_config2);
            CHECK_EQ(std::string("default_value"), str->to_string());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "value_inout_parameter_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("value_inout_parameter_test", true);

        sight::core::object::sptr receiver_obj;
        int j = 0;
        while(receiver_obj == nullptr && j++ < 200)
        {
            receiver_obj = sight::core::id::get_object("value_inout_parameter_sub_config", j, "receiver");
        }

        CHECK(receiver_obj != nullptr);

        auto receiver = std::dynamic_pointer_cast<sight::app::ut::test_service>(receiver_obj);
        CHECK(receiver != nullptr);
        CHECK(receiver->started());

        auto show_image =
            std::dynamic_pointer_cast<const sight::data::boolean>(receiver->input("data1").lock().get_shared());
        CHECK(show_image != nullptr);
        CHECK_EQ(true, show_image->value());

        auto position =
            std::dynamic_pointer_cast<const sight::data::dvec3>(receiver->input("data2").lock().get_shared());
        CHECK(position != nullptr);
        CHECK_EQ(sight::vec3d_t({100., 20., 12.}), position->value());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "value_inout_dvec3_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("value_inout_dvec3_test", true);

        sight::core::object::sptr receiver_obj;
        int j = 0;
        while(receiver_obj == nullptr && j++ < 200)
        {
            receiver_obj = sight::core::id::get_object("value_inout_dvec3_sub_config", j, "receiver");
        }

        CHECK(receiver_obj != nullptr);

        auto receiver = std::dynamic_pointer_cast<sight::app::ut::test_service>(receiver_obj);
        CHECK(receiver != nullptr);
        CHECK(receiver->started());

        auto position =
            std::dynamic_pointer_cast<const sight::data::dvec3>(receiver->input("data1").lock().get_shared());
        CHECK(position != nullptr);
        CHECK_EQ(sight::vec3d_t({11.5, -42.0, 7.25}), position->value());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "value_inout_optional_override_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("value_inout_optional_override_test", true);

        sight::core::object::sptr receiver_obj;
        int j = 0;
        while(receiver_obj == nullptr && j++ < 200)
        {
            receiver_obj = sight::core::id::get_object("value_inout_optional_override_sub_config", j, "receiver");
        }

        CHECK(receiver_obj != nullptr);

        auto receiver = std::dynamic_pointer_cast<sight::app::ut::test_service>(receiver_obj);
        CHECK(receiver != nullptr);
        CHECK(receiver->started());

        auto position =
            std::dynamic_pointer_cast<const sight::data::dvec3>(receiver->input("data1").lock().get_shared());
        CHECK(position != nullptr);

        // Optional object has default "1;2;3" in target config, but launcher-provided value must take precedence.
        CHECK_EQ(sight::vec3d_t({9., 8., 7.}), position->value());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "value_inout_optional_mixed_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("value_inout_optional_mixed_test", true);

        sight::core::object::sptr receiver_obj;
        int j = 0;
        while(receiver_obj == nullptr && j++ < 200)
        {
            receiver_obj = sight::core::id::get_object("value_inout_optional_mixed_sub_config", j, "receiver");
        }

        CHECK(receiver_obj != nullptr);

        auto receiver = std::dynamic_pointer_cast<sight::app::ut::test_service>(receiver_obj);
        CHECK(receiver != nullptr);
        CHECK(receiver->started());

        auto position_opt =
            std::dynamic_pointer_cast<const sight::data::dvec3>(receiver->input("data1").lock().get_shared());
        CHECK(position_opt != nullptr);
        CHECK_EQ(sight::vec3d_t({9., 8., 7.}), position_opt->value());

        auto position_uid =
            std::dynamic_pointer_cast<const sight::data::dvec3>(receiver->input("data2").lock().get_shared());
        CHECK(position_uid != nullptr);
        CHECK_EQ(sight::vec3d_t({-5., 0., 4.}), position_uid->value());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "nested_config_launcher_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("nested_config_launcher_test", true);

        sight::core::object::sptr receiver_obj;
        int j = 0;
        while(receiver_obj == nullptr && j++ < 200)
        {
            receiver_obj = sight::core::id::get_object("value_inout_optional_mixed_sub_config", j, "receiver");
        }

        auto receiver = std::dynamic_pointer_cast<sight::app::ut::test_service>(receiver_obj);
        REQUIRE(receiver != nullptr);
        CHECK(receiver->started());

        auto position_opt =
            std::dynamic_pointer_cast<const sight::data::dvec3>(receiver->input("data1").lock().get_shared());
        REQUIRE(position_opt != nullptr);
        CHECK_EQ(sight::vec3d_t({9., 8., 7.}), position_opt->value());

        auto position_uid =
            std::dynamic_pointer_cast<const sight::data::dvec3>(receiver->input("data2").lock().get_shared());
        REQUIRE(position_uid != nullptr);
        CHECK_EQ(sight::vec3d_t({-5., 0., 4.}), position_uid->value());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "value_simple_key_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("value_simple_key_test");

        // A simple input and a simple inout, both declared with a literal value instead of an object uid. The type is
        // deduced from the type the data::ptr is templated with.
        {
            auto srv = std::dynamic_pointer_cast<sight::app::ut::test_service_with_typed_data>(
                sight::core::id::get_object("typed_srv")
            );
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());
            CHECK(srv->started());

            const auto flag = srv->m_flag.const_lock();
            CHECK(flag != nullptr);
            CHECK_EQ(true, flag->value());

            const auto position = srv->m_position.const_lock();
            CHECK(position != nullptr);
            CHECK(sight::vec3d_t({4.5, -1., 0.25}) == position->value());

            // Not declared at all in the configuration, built from the default value declared with the data::ptr.
            const auto threshold = srv->m_threshold.const_lock();
            CHECK(threshold != nullptr);
            CHECK_EQ(std::int64_t(50), threshold->value());

            // Declared without 'uid' nor 'value', built from the default value as well.
            const auto offset = srv->m_offset.const_lock();
            CHECK(offset != nullptr);
            CHECK_EQ(std::int64_t(-3), offset->value());
        }

        // A group templated with a concrete type, mixing a literal value and an object uid. The value must land at the
        // right index, the uid keeping its own position.
        {
            auto srv = std::dynamic_pointer_cast<sight::app::ut::test_service_with_data>(
                sight::core::id::get_object("typed_group_srv")
            );
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());
            CHECK(srv->started());

            const auto from_value = srv->m_inout_group[0].const_lock();
            CHECK(from_value != nullptr);
            CHECK_EQ(std::int64_t(11), from_value->value());

            const auto from_uid = srv->m_inout_group[1].const_lock();
            CHECK(from_uid != nullptr);
            CHECK_EQ(std::int64_t(7), from_uid->value());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "nested_keys_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("nested_keys_test");

        {
            auto srv = std::dynamic_pointer_cast<sight::app::ut::test_service_with_nested_keys>(
                sight::core::id::get_object("nested_srv")
            );
            REQUIRE(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());
            CHECK(srv->started());

            // Objects bound from their uid, declared in two separate <image> tags
            const auto source = std::dynamic_pointer_cast<const sight::data::string>(
                srv->m_source.const_lock().get_shared()
            );
            REQUIRE(source != nullptr);
            CHECK_EQ(std::string("source"), source->value());

            const auto target = std::dynamic_pointer_cast<const sight::data::string>(
                srv->m_target.const_lock().get_shared()
            );
            REQUIRE(target != nullptr);
            CHECK_EQ(std::string("target"), target->value());

            // Properties given a literal value
            CHECK_EQ(std::int64_t(10), *srv->m_threshold);
            CHECK_EQ(std::string("hello"), *srv->m_label);

            // Groups, indexed by the rank of the repeated <tracker> tag
            REQUIRE_EQ(std::size_t(2), srv->m_tracker_ip.size());
            CHECK_EQ(std::string("127.0.0.1"), srv->m_tracker_ip[0].const_lock()->value());
            CHECK_EQ(std::string("192.168.0.1"), srv->m_tracker_ip[1].const_lock()->value());

            REQUIRE_EQ(std::size_t(2), srv->m_tracker_port.size());
            CHECK_EQ(std::int64_t(3000), srv->m_tracker_port[0].const_lock()->value());
            // The second port is an object uid, not a literal value
            CHECK_EQ(std::int64_t(4242), srv->m_tracker_port[1].const_lock()->value());
        }

        // Keys omitted in the configuration fall back on the default value declared with the pointer
        {
            auto srv = std::dynamic_pointer_cast<sight::app::ut::test_service_with_nested_keys>(
                sight::core::id::get_object("nested_default_srv")
            );
            REQUIRE(srv != nullptr);
            CHECK(srv->started());

            CHECK_EQ(std::int64_t(5), *srv->m_threshold);
            CHECK_EQ(std::string("default_label"), *srv->m_label);
            CHECK_EQ(std::size_t(0), srv->m_tracker_ip.size());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "nested_keys_deferred_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("nested_keys_deferred_test");

        // The deferred object is never produced, so the service waits for it
        CHECK(sight::core::id::get_object("nested_deferred_srv") == nullptr);

        // <optional> lets the service start without the deferred object
        auto srv = std::dynamic_pointer_cast<sight::app::ut::test_service_with_nested_keys>(
            sight::core::id::get_object("nested_optional_srv")
        );
        REQUIRE(srv != nullptr);
        CHECK(srv->started());
        CHECK(srv->m_source.const_lock() == nullptr);

        // The reserved 'optional' attribute only applies to the element of the group that carries it
        auto group_srv = std::dynamic_pointer_cast<sight::app::ut::test_service_with_nested_keys>(
            sight::core::id::get_object("nested_group_optional_srv")
        );
        REQUIRE(group_srv != nullptr);
        CHECK(group_srv->started());
        CHECK_EQ(std::string("127.0.0.1"), group_srv->m_tracker_ip[1].const_lock()->value());

        // Without it, the service waits for the deferred object
        CHECK(sight::core::id::get_object("nested_group_deferred_srv") == nullptr);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "object_config_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("objectConfigTest");

        // =================================================================================================================
        // Test a service with an external configuration and test Map with sub-object parsing
        // =================================================================================================================

        auto compo1 = std::dynamic_pointer_cast<sight::data::map>(sight::core::id::get_object("compo1Id"));
        CHECK(compo1 != nullptr);
        CHECK_EQ(static_cast<std::size_t>(1), compo1->count("data_in_map"));
        auto data2 = compo1->get<sight::data::string>("data_in_map");
        CHECK(data2);
        CHECK_EQ(std::string("data2Id"), data2->get_id());
        CHECK_EQ(std::string("Hello"), data2->value());

        // This service should have a map data and contain an external configuration with 2 parameters
        sight::core::object::sptr service = sight::core::id::get_object("TestService1Uid");
        auto srv1                         = std::dynamic_pointer_cast<sight::app::ut::test_service>(service);
        CHECK(srv1 != nullptr);
        CHECK_EQ(sight::service::base::configuration_status::configured, srv1->config_status());

        auto srv_data1 = srv1->input<sight::data::map>("data1");
        CHECK(!srv_data1.expired());
        CHECK(srv_data1.lock() == compo1);

        auto config = srv1->get_config();
        CHECK_EQ(std::string("value1"), config.get<std::string>("param1"));
        CHECK_EQ(std::string("value2"), config.get<std::string>("param2"));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "properties_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("properties_cfg_test");

        // =================================================================================================================
        // Test all possible initializations of properties in services
        // =================================================================================================================

        {
            sight::core::object::sptr service = sight::core::id::get_object("test_service_default_props");
            auto srv                          =
                std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());

            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());

            CHECK_EQ(std::int64_t(42), *srv->m_int_prop);
            CHECK_EQ(std::string("default_value"), *srv->m_string_prop);
            CHECK(sight::vec3d_t({12.123, 56.0, 78.56}) == *srv->m_vec_prop);
        }
        {
            sight::core::object::sptr service = sight::core::id::get_object("test_service_parse_props_1");
            auto srv                          =
                std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());

            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());

            CHECK_EQ(std::int64_t(12), *srv->m_int_prop);
            CHECK_EQ(std::string("foo"), *srv->m_string_prop);
            CHECK(sight::vec3d_t({78.2, 54, 14.3}) == *srv->m_vec_prop);
        }
        {
            sight::core::object::sptr service = sight::core::id::get_object("test_service_parse_props_2");
            auto srv                          =
                std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());

            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());

            CHECK_EQ(std::int64_t(-45), *srv->m_int_prop);
            CHECK_EQ(std::string("bar"), *srv->m_string_prop);
            CHECK(sight::vec3d_t({-8.7, 79, 7.48}) == *srv->m_vec_prop);
        }
        {
            sight::core::object::sptr service = sight::core::id::get_object("test_service_object_props_1");
            auto srv                          =
                std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());

            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());

            CHECK_EQ(std::int64_t(23), *srv->m_int_prop);
            CHECK_EQ(std::string("hello"), *srv->m_string_prop);
            CHECK(sight::vec3d_t({64, -94, -21.467}) == *srv->m_vec_prop);
            {
                auto int_object =
                    std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("integer_object"));
                CHECK(int_object != nullptr);
                int_object->async_emit(sight::data::signals::MODIFIED);

                SIGHT_TEST_WAIT(srv->m_slot_called);
                CHECK_EQ(false, srv->m_slot_called);
                CHECK_EQ(std::string_view("integer"), srv->m_callback_called_parameter);
            }
            srv->m_callback_called_parameter = "";
            {
                auto vec_object =
                    std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("vec_object"));
                CHECK(vec_object != nullptr);
                vec_object->async_emit(sight::data::signals::MODIFIED);

                SIGHT_TEST_WAIT(srv->m_slot_called);
                CHECK_EQ(true, srv->m_slot_called);
                CHECK_EQ(std::string_view(), srv->m_callback_called_parameter);
            }
        }
        {
            sight::core::object::sptr service = sight::core::id::get_object("test_service_mix_props_1");
            auto srv                          =
                std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());

            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());

            CHECK_EQ(std::int64_t(42), *srv->m_int_prop);
            CHECK_EQ(std::string("bye"), *srv->m_string_prop);
            CHECK(sight::vec3d_t({64, -94, -21.467}) == *srv->m_vec_prop);

            srv->m_slot_called = false;
            CHECK_EQ(false, srv->m_slot_called);

            srv->m_callback_called_parameter = "";
            {
                auto int_object =
                    std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("integer_object"));
                CHECK(int_object != nullptr);
                int_object->async_emit(sight::data::signals::MODIFIED);

                SIGHT_TEST_WAIT(srv->m_slot_called);
                CHECK_EQ(false, srv->m_slot_called);
                CHECK_EQ(std::string_view(), srv->m_callback_called_parameter);
            }
            {
                auto vec_object =
                    std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("vec_object"));
                CHECK(vec_object != nullptr);
                vec_object->async_emit(sight::data::signals::MODIFIED);

                SIGHT_TEST_WAIT(srv->m_slot_called);
                CHECK_EQ(true, srv->m_slot_called);
                CHECK_EQ(std::string_view(), srv->m_callback_called_parameter);
            }
        }
        {
            sight::core::object::sptr service = sight::core::id::get_object("test_service_map_props_1");
            auto srv                          =
                std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());

            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());

            CHECK_EQ(std::int64_t(45), *srv->m_int_prop);
            CHECK_EQ(std::string("default_value"), *srv->m_string_prop);
            CHECK(sight::vec3d_t({-0.6, 2.41, 78}) == *srv->m_vec_prop);

            CHECK_EQ(false, srv->m_slot_called);

            auto map_object =
                std::dynamic_pointer_cast<sight::data::map>(sight::core::id::get_object("properties_map"));
            CHECK(map_object != nullptr);

            // "vec" is connected thus only the slot is called
            {
                (*map_object)["vec"]->async_emit(sight::data::signals::MODIFIED);

                SIGHT_TEST_WAIT(srv->m_slot_called);
                CHECK_EQ(true, srv->m_slot_called);
                CHECK_EQ(std::string_view(), srv->m_callback_called_parameter);
            }

            srv->m_slot_called = false;

            // "integer" is not connected thus only the callback is called
            {
                (*map_object)["integer"]->async_emit(sight::data::signals::MODIFIED);

                SIGHT_TEST_WAIT(srv->m_slot_called);
                CHECK_EQ(false, srv->m_slot_called);
                CHECK_EQ(std::string_view("integer"), srv->m_callback_called_parameter);
            }
        }
        {
            sight::core::object::sptr service = sight::core::id::get_object("test_service_map_object_props_1");
            auto srv                          =
                std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());
            srv->m_slot_called = false;

            CHECK_EQ(std::int64_t(45), *srv->m_int_prop);
            {
                auto map_object =
                    std::dynamic_pointer_cast<sight::data::map>(sight::core::id::get_object("properties_map"));
                CHECK(map_object != nullptr);
                (*map_object)["integer"]->async_emit(sight::data::signals::MODIFIED);

                SIGHT_TEST_WAIT(srv->m_slot_called);
                CHECK_EQ(false, srv->m_slot_called);

                CHECK_EQ(std::string_view("integer"), srv->m_callback_called_parameter);
            }
            srv->m_callback_called_parameter = "";
        }
        {
            sight::core::object::sptr service = sight::core::id::get_object("test_service_map_object_props_2");
            auto srv                          =
                std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());
            srv->m_slot_called = false;

            CHECK_EQ(std::int64_t(45), *srv->m_int_prop);
            {
                auto map_object =
                    std::dynamic_pointer_cast<sight::data::map>(sight::core::id::get_object("properties_map"));
                CHECK(map_object != nullptr);
                (*map_object)["integer"]->async_emit(sight::data::signals::MODIFIED);

                SIGHT_TEST_WAIT(srv->m_slot_called);
                CHECK_EQ(false, srv->m_slot_called);

                CHECK_EQ(std::string_view("integer"), srv->m_callback_called_parameter);
            }
            srv->m_callback_called_parameter = "";
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "properties_signals_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("properties_cfg_test");

        {
            sight::core::object::sptr service = sight::core::id::get_object("test_service_object_props_1");
            auto srv                          =
                std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());
            srv->m_slot_called = false;
            srv->m_emit_signal = true;

            CHECK_EQ(std::int64_t(23), *srv->m_int_prop);
            {
                auto int_object =
                    std::dynamic_pointer_cast<sight::data::object>(sight::core::id::get_object("integer_object"));
                CHECK(int_object != nullptr);
                int_object->async_emit(sight::data::signals::MODIFIED);

                SIGHT_TEST_WAIT(srv->m_slot_called);
                CHECK_EQ(false, srv->m_slot_called);

                CHECK_EQ(std::string_view("integer"), srv->m_callback_called_parameter);
                // Verify we did not enter an infinite loop
                CHECK_EQ(std::size_t(1), srv->m_signal_count);
            }
            srv->m_callback_called_parameter = "";
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "properties_map_element_parameter_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("properties_map_element_parameter_test", true);

        sight::core::object::sptr data1_in_config1;
        {
            int j = 0;
            while(data1_in_config1 == nullptr && j++ < 200)
            {
                data1_in_config1 = sight::core::id::get_object(
                    "properties_map_element_parameter_test",
                    j,
                    "data1_in_map"
                );
            }
        }
        CHECK(data1_in_config1 != nullptr);
        {
            auto str = std::dynamic_pointer_cast<sight::data::string>(data1_in_config1);
            CHECK_EQ(std::string("yeah"), str->to_string());
        }

        {
            sight::core::object::sptr service;
            {
                int j = 0;
                while(service == nullptr && j++ < 200)
                {
                    service = sight::core::id::get_object(
                        "default_object_id_test_sub_config",
                        j,
                        "test_service_parse_props_1"
                    );
                }
            }
            CHECK(service != nullptr);
            auto srv = std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());

            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());

            CHECK_EQ(std::string("yeah"), *srv->m_string_prop);
        }
        {
            sight::core::object::sptr service;
            {
                int j = 0;
                while(service == nullptr && j++ < 200)
                {
                    service = sight::core::id::get_object(
                        "properties_map_element_parameter_subconfig",
                        j,
                        "test_service_parse_props_1"
                    );
                }
            }
            CHECK(service != nullptr);
            auto srv = std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());

            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());

            CHECK_EQ(std::string("yeah2"), *srv->m_string_prop);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "properties_map_element_deferred_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("properties_map_element_deferred_test");

        {
            sight::core::object::sptr service;
            {
                int j = 0;
                while(service == nullptr && j++ < 200)
                {
                    service = sight::core::id::get_object(
                        "default_object_id_test_sub_config",
                        j,
                        "test_service_parse_props_1"
                    );
                }
            }
            CHECK(service == nullptr);

            // Create the data that will launch the service
            auto data1 = std::make_shared<sight::data::string>();
            data1->set_value("yeah2");

            auto gen_data_srv =
                std::dynamic_pointer_cast<sight::app::ut::test_service>(sight::core::id::get_object("generate_data"));
            CHECK(gen_data_srv != nullptr);
            gen_data_srv->set_output(data1, "out");

            wait_service_started("test3_srv");
            auto gn_srv3 = sight::core::id::get_object("test3_srv");
            auto srv3    = std::dynamic_pointer_cast<sight::service::base>(gn_srv3);
            CHECK(srv3 != nullptr);
            CHECK_EQ(sight::service::base::global_status::started, srv3->status());

            {
                int j = 0;
                while(service == nullptr && j++ < 200)
                {
                    service = sight::core::id::get_object(
                        "default_object_id_test_sub_config",
                        j,
                        "test_service_parse_props_1"
                    );
                }
            }
            CHECK(service != nullptr);
            auto srv = std::dynamic_pointer_cast<sight::app::ut::test_service_with_properties>(service);
            CHECK(srv != nullptr);
            CHECK_EQ(sight::service::base::configuration_status::configured, srv->config_status());

            CHECK(srv != nullptr);
            CHECK_EQ(true, srv->started());

            CHECK_EQ(std::string("yeah"), *srv->m_string_prop);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "map_config_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("map_config_test", true);

        sight::core::object::sptr object;

        for(std::size_t i = 0 ; object == nullptr && i < 200 ; ++i)
        {
            object = sight::core::id::get_object("map_config_test", i, "map");
        }

        CHECK(object != nullptr);

        auto map = std::dynamic_pointer_cast<sight::data::map>(object);
        CHECK(map != nullptr);

        for(const auto& item : *map)
        {
            if(item.first == "integer")
            {
                auto integer = std::dynamic_pointer_cast<sight::data::integer>(item.second);
                CHECK(integer != nullptr);
                CHECK_EQ(std::int64_t(1), integer->value());
            }
            else if(item.first == "string")
            {
                auto string = std::dynamic_pointer_cast<sight::data::string>(item.second);
                CHECK(string != nullptr);
                CHECK_EQ(std::string("abc"), string->value());
            }
            else if(item.first == "boolean")
            {
                auto boolean = std::dynamic_pointer_cast<sight::data::boolean>(item.second);
                CHECK(boolean != nullptr);
                CHECK_EQ(true, boolean->value());
            }
            else if(item.first == "ivec3")
            {
                auto ivec3 = std::dynamic_pointer_cast<sight::data::ivec3>(item.second);
                CHECK(ivec3 != nullptr);

                const auto& value = ivec3->value();
                CHECK_EQ(std::int64_t(0), value[0]);
                CHECK_EQ(std::int64_t(0), value[1]);
                CHECK_EQ(std::int64_t(0), value[2]);
            }
            else if(item.first == "dvec3")
            {
                auto dvec3 = std::dynamic_pointer_cast<sight::data::dvec3>(item.second);
                CHECK(dvec3 != nullptr);

                const auto& value = dvec3->value();
                CHECK_EQ(1.0, value[0]);
                CHECK_EQ(1.0, value[1]);
                CHECK_EQ(1.0, value[2]);
            }
            else if(item.first == "transfer_function")
            {
                auto transfer_function = std::dynamic_pointer_cast<sight::data::transfer_function>(item.second);
                CHECK(transfer_function != nullptr);
                CHECK_EQ(std::string("CT-GreyLevel"), transfer_function->name());
            }
            else
            {
                FAIL("Unknown item in map: ", item.first);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "matrix_config_test")
    {
        m_app_config_mgr = sight::app::ut::launch_app_config_mgr("matrix_config_test", true);

        sight::core::object::sptr object;

        for(std::size_t i = 0 ; object == nullptr && i < 200 ; ++i)
        {
            object = sight::core::id::get_object("matrix_config_test", i, "matrix");
        }

        CHECK(object != nullptr);

        auto matrix = std::dynamic_pointer_cast<sight::data::matrix4>(object);
        CHECK(matrix != nullptr);

        const auto& values = matrix->values();
        CHECK(doctest::Approx(1.) == values[0]);
        CHECK(doctest::Approx(0.) == values[1]);
        CHECK(doctest::Approx(0.) == values[2]);
        CHECK(doctest::Approx(5.) == values[3]);
        CHECK(doctest::Approx(0.) == values[4]);
        CHECK(doctest::Approx(1.) == values[5]);
        CHECK(doctest::Approx(0.) == values[6]);
        CHECK(doctest::Approx(0.) == values[7]);
        CHECK(doctest::Approx(0.) == values[8]);
        CHECK(doctest::Approx(0.) == values[9]);
        CHECK(doctest::Approx(1.) == values[10]);
        CHECK(doctest::Approx(0.) == values[11]);
        CHECK(doctest::Approx(0.) == values[12]);
        CHECK(doctest::Approx(0.) == values[13]);
        CHECK(doctest::Approx(0.) == values[14]);
        CHECK(doctest::Approx(1.) == values[15]);
    }

//------------------------------------------------------------------------------
} // namespace sight::app::ut
