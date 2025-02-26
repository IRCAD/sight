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

#include "config_update_test.hpp"

#include "helper.hpp"
#include "test_service.hpp"
#include "test_services.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/runtime/helper.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>
#include <core/time_stamp.hpp>

#include <data/boolean.hpp>
#include <data/image.hpp>
#include <data/string.hpp>

#include <service/extension/config.hpp>
#include <service/registry.hpp>

#include <utest/wait.hpp>

#include <filesystem>
#include <ranges>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::ut::config_update_test);

namespace sight::app::ut
{

//------------------------------------------------------------------------------

void config_update_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("app_ut");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
    core::runtime::load_module("config_update_test");

    auto app_config = app::extension::config::get();
    app_config->clear_registry();
    app_config->parse_plugin_infos();

    auto srv_config = sight::service::extension::config::get_default();
    srv_config->clear_registry();
    srv_config->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void config_update_test::tearDown()
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

const auto TEST_SERVICE =
    [](unsigned int _i)
    {
        core::object::sptr service = core::id::get_object("test" + std::to_string(_i) + "_srv");
        auto srv                   = std::dynamic_pointer_cast<sight::app::ut::test_order_srv>(service);
        CPPUNIT_ASSERT_EQUAL(true, srv->started());
        SIGHT_TEST_WAIT(srv->update_order() == _i);
        CPPUNIT_ASSERT_EQUAL(_i, srv->update_order());
    };

//------------------------------------------------------------------------------

void config_update_test::simple_sequence_test()
{
    test_order_srv::s_ORDER = 1;
    m_app_config_mgr        = app::ut::launch_app_config_mgr("sequence_cfg_test");

    for(const auto i : std::views::iota(1U, 4U))
    {
        TEST_SERVICE(i);
    }
}

//------------------------------------------------------------------------------

void config_update_test::imbricated_sequence_test()
{
    test_order_srv::s_ORDER = 1;
    m_app_config_mgr        = app::ut::launch_app_config_mgr("imbricated_sequence_cfg_test");

    for(const auto i : std::views::iota(1U, 7U))
    {
        TEST_SERVICE(i);
    }
}

//------------------------------------------------------------------------------

void config_update_test::mutiple_config_sequence_test()
{
    test_order_srv::s_ORDER = 1;
    m_app_config_mgr        = app::ut::launch_app_config_mgr("mutiple_config_sequence_cfg_test");

    TEST_SERVICE(1);
    TEST_SERVICE(4);
    TEST_SERVICE(7);
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
