/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "parameters_test.hpp"

#include <app/extension/parameters.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <service/extension/config.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::app::ut::parameters_test);

namespace sight::app::ut
{

//------------------------------------------------------------------------------

void parameters_test::setUp()
{
    // Set up context before running a test
    core::runtime::init();

    std::filesystem::path location = core::runtime::get_resource_file_path("tu_exec_app");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    core::runtime::add_modules(location);
    core::runtime::load_module("sight::module::app");
    core::runtime::load_module("parameters_test");

    app::extension::parameters::sptr app_config_param;
    app_config_param = app::extension::parameters::get_default();
    app_config_param->clear_registry();
    app_config_param->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void parameters_test::tearDown()
{
}

//------------------------------------------------------------------------------

void parameters_test::app_config_parameters_test()
{
    app::extension::parameters::sptr app_config_param;
    app_config_param = app::extension::parameters::get_default();

    app::field_adaptor_t parameters = app_config_param->get_parameters("id1");
    CPPUNIT_ASSERT(!parameters.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value1"), parameters["param1"]);

    app::field_adaptor_t parameters2 = app_config_param->get_parameters("id2");
    CPPUNIT_ASSERT(!parameters2.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value3"), parameters2["param3"]);
}

//------------------------------------------------------------------------------

void parameters_test::concurent_access_toparameters_test()
{
    std::vector<std::future<void> > futures;
    for(unsigned int i = 0 ; i < 20 ; ++i)
    {
        futures.push_back(std::async(std::launch::async, app_config_parameters_test));
    }

    for(auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
