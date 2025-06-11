/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "config_launcher_test.hpp"

#include "test_service.hpp"

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <service/extension/config.hpp>
#include <service/op.hpp>

#include <ui/__/parameter.hpp>

#include <utest/wait.hpp>

#include <app/extension/parameters.hpp>

#include <boost/config.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <filesystem>
#include <regex>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::ut::config_launcher_test);

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

void config_launcher_test::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("app_ut");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
    core::runtime::load_module("config_test");

    app::extension::config::sptr app_config = app::extension::config::get();
    app_config->clear_registry();
    app_config->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void config_launcher_test::tearDown()
{
    // Clean up after the test run.
    // unregister the services that have not been unregistered because a test failed.

    auto services = sight::service::get_services("sight::app::config_launcher");
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

void config_launcher_test::multi_config_test()
{
    auto config_id = std::make_shared<sight::data::string>();

    auto srv = service::add("sight::app::config_launcher");
    srv->set_inout(config_id, "config");

    srv->configure();
    srv->start().get();

    // start the test!
    // set config2 =>
    //  srv1 : not existing
    //  srv2 : started

    int current_id = get_current_id();
    config_id->set_value(MULTI_CFG_CTL_2);
    srv->update().get();
    auto srv1_uid = core::id::join(MULTI_CFG_CTL_1, current_id++, "srv");
    auto srv2_uid = core::id::join(MULTI_CFG_CTL_2, current_id++, "srv");

    auto srv1 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 == nullptr);
    auto srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    // set config1 =>
    //  srv1 : started
    //  srv2 : stopped
    config_id->set_value(MULTI_CFG_CTL_1);
    config_id->emit(sight::data::object::MODIFIED_SIG);

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
    config_id->set_value(MULTI_CFG_CTL_2);
    config_id->emit(sight::data::object::MODIFIED_SIG);

    srv2_uid = core::id::join(MULTI_CFG_CTL_2, current_id++, "srv");
    srv1     = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 != nullptr && srv1->stopped());
    srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    // set again config2 => nothing should have changed, the service is not updated
    //  srv1 : stopped
    //  srv2 : started
    config_id->emit(sight::data::object::MODIFIED_SIG);

    srv1 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 != nullptr && srv1->stopped());
    srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

void config_launcher_test::set_config_key_test()
{
    auto config_id = std::make_shared<sight::data::string>(MULTI_CFG_CTL_1);

    // Initialise the testing service
    service::base::sptr srv = service::add("sight::app::config_launcher");
    srv->set_inout(config_id, "config");

    srv->configure();
    srv->start().wait();

    // start the test!
    // set config2 =>
    //  multi_cfg_ctl_1_10_srv : not existing
    //  multi_cfg_ctl_2_8_srv : started

    int current_id = get_current_id();
    config_id->set_value(MULTI_CFG_CTL_2);
    srv->update().get();

    auto srv1_uid = core::id::join(MULTI_CFG_CTL_1, current_id++, "srv");
    auto srv2_uid = core::id::join(MULTI_CFG_CTL_2, current_id++, "srv");
    auto srv1     = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 == nullptr);
    auto srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->started());

    // set config with bad key
    config_id->set_value("Unknown config");
    CPPUNIT_ASSERT_THROW(srv->update().get(), core::exception);

    srv1 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv1_uid));
    CPPUNIT_ASSERT(srv1 == nullptr);
    srv2 = std::dynamic_pointer_cast<app::ut::test_service>(core::id::get_object(srv2_uid));
    CPPUNIT_ASSERT(srv2 != nullptr && srv2->stopped());

    srv->stop().wait();
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
