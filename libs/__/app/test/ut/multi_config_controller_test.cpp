/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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
#include <regex>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::ut::multi_config_controller_test);

namespace sight::app::ut
{

static constexpr auto MULTI_CFG_CTL_1 = "multi_cfg_ctl_1";
static constexpr auto MULTI_CFG_CTL_2 = "multi_cfg_ctl_2";

//------------------------------------------------------------------------------

int get_current_id()
{
    auto current_id_str = sight::app::extension::config::get_unique_identifier("get_id");
    std::smatch match;
    CPPUNIT_ASSERT(std::regex_match(current_id_str, match, std::regex("get_id[^0-9]+([0-9]*)")));
    current_id_str = match[1].str();
    return std::stoi(current_id_str);
}

//------------------------------------------------------------------------------

void multi_config_controller_test::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("app_ut");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
    core::runtime::load_module("config_test");

    app::extension::config::sptr app_config = app::extension::config::get_default();
    app_config->clear_registry();
    app_config->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void multi_config_controller_test::tearDown()
{
    // Clean up after the test run.
    // unregister the services that have not been unregistered because a test failed.

    auto services = sight::service::get_services("sight::app::multi_config_controller");
    for(const auto& srv : services)
    {
        if(srv->started())
        {
            srv->stop().get();
        }

        service::unregister_service(srv);
    }
}

//------------------------------------------------------------------------------

void multi_config_controller_test::multi_config_test()
{
    // Initialise the testing service
    std::stringstream config_string;
    config_string << R"( <appConfig key="myKey" default="config1">)"
    << R"(<config name="config1" id=")" << MULTI_CFG_CTL_1 << R"(" />)"
    << R"(<config name="config2" id=")" << MULTI_CFG_CTL_2 << R"(" />)"
    << "</appConfig>";

    service::base::sptr srv = service::add("sight::app::multi_config_controller");

    service::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);
    srv->configure();
    srv->start().get();

    // create connection to wait the end of the update
    bool service_updated      = false;
    auto slot_service_updated = sight::core::com::new_slot(
        [&service_updated]()
        {
            service_updated = true;
        });
    slot_service_updated->set_worker(sight::core::thread::get_default_worker());
    auto service_updated_connection = srv->signal("updated")->connect(slot_service_updated);

    // start the test!
    // set config2 =>
    //  srv1 : not existing
    //  srv2 : started

    int current_id = get_current_id();
    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("myKey"));
    srv->update().get();
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    auto srv1_uid = core::id::join(MULTI_CFG_CTL_1, current_id++, "srv");
    auto srv2_uid = core::id::join(MULTI_CFG_CTL_2, current_id++, "srv");

    auto srv1 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 == nullptr);
    auto srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    // set config1 =>
    //  srv1 : started
    //  srv2 : stopped
    srv->slot("set_config")->run(ui::parameter_t("config1"), std::string("myKey"));
    srv->update().get();
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    srv1_uid = core::id::join(MULTI_CFG_CTL_1, current_id++, "srv");
    srv1     = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 != nullptr && srv1->started());
    srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->stopped());
    {
        // Ensuring the service is really destroyed by the app config manager by releasing our reference
        srv2.reset();
        auto srv2_test = core::id::get_object(srv2_uid);
        CPPUNIT_ASSERT(srv2_test == nullptr);
    }

    // set config2 =>
    //  srv1 : stopped
    //  srv2 : started
    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("myKey"));
    srv->update().get();
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    srv2_uid = core::id::join(MULTI_CFG_CTL_2, current_id++, "srv");
    srv1     = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 != nullptr && srv1->stopped());
    srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    // set again config2 => nothing should have changed, the service is not updated
    //  srv1 : stopped
    //  srv2 : started
    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("myKey"));
    SIGHT_TEST_FAIL_WAIT(!service_updated);

    srv1 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 != nullptr && srv1->stopped());
    srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

void multi_config_controller_test::set_config_key_test()
{
    // Initialise the testing service

    std::stringstream config_string;
    config_string << R"( <appConfig key="myKey" default="config1">)"
    << R"(<config name="config1" id=")" << MULTI_CFG_CTL_1 << R"(" />)"
    << R"(<config name="config2" id=")" << MULTI_CFG_CTL_2 << R"(" />)"
    << "</appConfig>";

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
    auto service_updated_connection = srv->signal("updated")->connect(slot_service_updated);

    // start the test!
    // set config2 =>
    //  multi_cfg_ctl_1_10_srv : not existing
    //  multi_cfg_ctl_2_8_srv : started

    int current_id = get_current_id();

    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("myKey"));
    srv->update().get();
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    auto srv1_uid = core::id::join(MULTI_CFG_CTL_1, current_id++, "srv");
    auto srv2_uid = core::id::join(MULTI_CFG_CTL_2, current_id++, "srv");
    auto srv1     = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 == nullptr);
    auto srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    // set config with bad key
    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("badKey"));
    srv->update().get();
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;

    srv1 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 == nullptr);
    srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

void multi_config_controller_test::default_loading_test()
{
    std::stringstream config_string;
    config_string << R"( <appConfig key="myKey" default="config1">)"
    << R"(<config name="config1" id=")" << MULTI_CFG_CTL_1 << R"(" />)"
    << R"(<config name="config2" id=")" << MULTI_CFG_CTL_2 << R"(" />)"
    << "</appConfig>";

    service::base::sptr srv = service::add("sight::app::multi_config_controller");

    int current_id = get_current_id();

    service::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);

    srv->configure();
    srv->start().get();
    srv->update().get();

    auto srv1_uid = core::id::join(MULTI_CFG_CTL_1, ++current_id, "srv");
    auto srv1     = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 != nullptr && srv1->started());

    srv->update().get();

    srv1 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 != nullptr && srv1->started());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

void multi_config_controller_test::parameters_test()
{
    // Initialise the testing service
    std::stringstream config_string;
    config_string << R"( <appConfig key="myKey" default="config1">)"
    << R"(<config name="config1" id=")" << MULTI_CFG_CTL_1 << R"(" />)"
    << R"(<config name="config2" id=")" << MULTI_CFG_CTL_2 << R"(" />)"
    << "</appConfig>";

    service::base::sptr srv = service::add("sight::app::multi_config_controller");

    service::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);
    srv->configure();
    srv->start().get();

    // create connection to wait the end of the update
    bool service_updated      = false;
    auto slot_service_updated = sight::core::com::new_slot(
        [&service_updated]()
        {
            service_updated = true;
        });
    slot_service_updated->set_worker(sight::core::thread::get_default_worker());
    auto service_updated_connection = srv->signal("updated")->connect(slot_service_updated);

    std::string started_config;
    auto slot_config_started = sight::core::com::new_slot(
        [&started_config](std::string _config)
        {
            started_config = _config;
        });
    slot_config_started->set_worker(sight::core::thread::get_default_worker());
    auto config_started_connection = srv->signal("config_started")->connect(slot_config_started);

    // start the test!
    // set config2 =>
    //  srv1 : not existing
    //  srv2 : started

    int current_id = get_current_id();
    srv->slot("set_config")->run(ui::parameter_t("config2"), std::string("myKey"));
    srv->update().get();
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;
    SIGHT_TEST_FAIL_WAIT(started_config == "config2");
    started_config.clear();

    auto srv1_uid = core::id::join(MULTI_CFG_CTL_1, current_id++, "srv");
    auto srv2_uid = core::id::join(MULTI_CFG_CTL_2, current_id++, "srv");

    auto srv1 = std::dynamic_pointer_cast<app::ut::test_config>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 == nullptr);
    auto srv2 = std::dynamic_pointer_cast<app::ut::test_config>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    auto exported_config = srv2->export_config();
    CPPUNIT_ASSERT_EQUAL(exported_config.get_optional<std::string>("param1").value_or(""), std::string(""));

    // set config1 =>
    //  srv1 : started
    //  srv2 : stopped
    srv->slot("set_config")->run(ui::parameter_t("config1"), std::string("myKey"));
    srv->slot("set_config")->run(ui::parameter_t("param1"), std::string("param1"));
    srv->update().get();
    SIGHT_TEST_FAIL_WAIT(service_updated);
    service_updated = false;
    SIGHT_TEST_FAIL_WAIT(started_config == "config1");
    started_config.clear();

    srv1_uid = core::id::join(MULTI_CFG_CTL_1, current_id++, "srv");
    srv1     = std::dynamic_pointer_cast<app::ut::test_config>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 != nullptr && srv1->started());
    srv2 = std::dynamic_pointer_cast<app::ut::test_config>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->stopped());
    {
        // Ensuring the service is really destroyed by the app config manager by releasing our reference
        srv2.reset();
        auto srv2_test = core::id::get_object(srv2_uid);
        CPPUNIT_ASSERT(srv2_test == nullptr);
    }

    exported_config = srv1->export_config();
    CPPUNIT_ASSERT_EQUAL(exported_config.get_optional<std::string>("param1").value_or(""), std::string("param1"));

    srv->stop().wait();
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
