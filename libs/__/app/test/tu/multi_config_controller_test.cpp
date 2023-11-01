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

#include "multi_config_controller_test.hpp"

#include "test_service.hpp"

#include <app/extension/parameters.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <service/extension/config.hpp>
#include <service/op.hpp>

#include <ui/__/parameter.hpp>

#include <utest/wait.hpp>

#include <boost/config.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::ut::multi_config_controller_test);

namespace sight::app::ut
{

//------------------------------------------------------------------------------

void multi_config_controller_test::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("tu_exec_app");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
}

//------------------------------------------------------------------------------

void multi_config_controller_test::tearDown()
{
    // Clean up after the test run.
    // unregister the services that have not been unregistered because a test failed.

    auto services = sight::service::get_services<service::base>();
    for(const auto& srv : services)
    {
        if(srv->started())
        {
            srv->stop().wait();
        }

        service::unregister_service(srv);
    }
}

//------------------------------------------------------------------------------

void multi_config_controller_test::multi_config_test()
{
    // Initialise the testing service

    std::stringstream config_string;
    config_string << " <appConfig key=\"myKey\" default=\"config1\">"
                     "<config name=\"config1\" id=\"connectionTest\" />"
                     "<config name=\"config2\" id=\"startStopConnectionTest\" />"
                     "</appConfig>";
    service::base::sptr srv = service::add("sight::app::multi_config_controller");

    service::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);
    srv->configure();
    srv->start().wait();

    // create connection to wait the end of the update
    bool service_updated      = false;
    auto slot_service_updated = sight::core::com::new_slot(
        [&service_updated]()
        {
            service_updated = true;
        });
    slot_service_updated->set_worker(sight::core::thread::get_default_worker());
    auto service_updated_connection = srv->signal("updated")->connect(
        slot_service_updated
    );

    // start the test!
    // set config2 =>
    //  connectionTest_2_SGenerateData : not existing
    //  startStopConnectionTest_2_TestService5Uid : started

    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("myKey"));
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    auto gen_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "connectionTest_6_SGenerateData"
            )
        );
    CPPUNIT_ASSERT(gen_data_srv == nullptr);
    auto no_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "startStopConnectionTest_6_TestService5Uid"
            )
        );
    CPPUNIT_ASSERT(no_data_srv != nullptr && no_data_srv->started());

    // set config1 =>
    //  connectionTest_2_SGenerateData : started
    //  startStopConnectionTest_2_TestService5Uid : stopped
    srv->slot("set_config")->run(ui::parameter_t("config1"), std::string("myKey"));
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    gen_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "connectionTest_8_SGenerateData"
            )
        );
    CPPUNIT_ASSERT(gen_data_srv != nullptr && gen_data_srv->started());
    no_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "startStopConnectionTest_6_TestService5Uid"
            )
        );
    CPPUNIT_ASSERT(no_data_srv != nullptr && no_data_srv->stopped());

    // set config2 =>
    //  connectionTest_2_SGenerateData : stopped
    //  startStopConnectionTest_2_TestService5Uid : started
    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("myKey"));
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    gen_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "connectionTest_8_SGenerateData"
            )
        );
    CPPUNIT_ASSERT(gen_data_srv != nullptr && gen_data_srv->stopped());
    no_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "startStopConnectionTest_10_TestService5Uid"
            )
        );
    CPPUNIT_ASSERT(no_data_srv != nullptr && no_data_srv->started());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

void multi_config_controller_test::set_config_key_test()
{
    // Initialise the testing service

    std::stringstream config_string;
    config_string << " <appConfig key=\"myKey\" default=\"config1\">"
                     "<config name=\"config1\" id=\"connectionTest\" />"
                     "<config name=\"config2\" id=\"startStopConnectionTest\" />"
                     "</appConfig>";
    service::base::sptr srv = service::add("sight::app::multi_config_controller");

    service::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);
    srv->configure();
    srv->start().wait();

    // create connection to wait the end of the update
    bool service_updated      = false;
    auto slot_service_updated = sight::core::com::new_slot(
        [&service_updated]()
        {
            service_updated = true;
        });
    slot_service_updated->set_worker(sight::core::thread::get_default_worker());
    auto service_updated_connection = srv->signal("updated")->connect(
        slot_service_updated
    );

    // start the test!
    // set config2 =>
    //  connectionTest_2_SGenerateData : not existing
    //  startStopConnectionTest_2_TestService5Uid : started

    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("myKey"));
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    auto gen_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "connectionTest_12_SGenerateData"
            )
        );
    CPPUNIT_ASSERT(gen_data_srv == nullptr);
    auto no_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "startStopConnectionTest_12_TestService5Uid"
            )
        );
    CPPUNIT_ASSERT(no_data_srv != nullptr && no_data_srv->started());

    // set config with bad key
    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("badKey"));
    srv->slot("update")->run();
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    gen_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "connectionTest_14_SGenerateData"
            )
        );
    CPPUNIT_ASSERT(gen_data_srv == nullptr);
    no_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "startStopConnectionTest_14_TestService5Uid"
            )
        );
    CPPUNIT_ASSERT(no_data_srv != nullptr && no_data_srv->started());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

void multi_config_controller_test::default_loading_test()
{
    std::stringstream config_string;
    config_string << " <appConfig key=\"myKey\" default=\"config1\">"
                     "<config name=\"config1\" id=\"connectionTest\" />"
                     "<config name=\"config2\" id=\"startStopConnectionTest\" />"
                     "</appConfig>";
    service::base::sptr srv = service::add("sight::app::multi_config_controller");

    service::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);

    srv->configure();
    srv->start().wait();
    srv->update().wait();

    auto gen_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "connectionTest_16_SGenerateData"
            )
        );
    CPPUNIT_ASSERT(gen_data_srv != nullptr && gen_data_srv->started());

    srv->update().wait();
    gen_data_srv =
        std::dynamic_pointer_cast<app::ut::test_service>(
            core::tools::id::get_object(
                "connectionTest_18_SGenerateData"
            )
        );
    CPPUNIT_ASSERT(gen_data_srv != nullptr && gen_data_srv->started());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
