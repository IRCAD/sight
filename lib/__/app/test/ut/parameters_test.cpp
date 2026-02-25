/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <service/extension/config.hpp>

#include <app/extension/parameters.hpp>

#include <doctest/doctest.h>

#include <filesystem>

namespace
{

struct fixture
{
    fixture()
    {
        // Set up context before running a test
        sight::core::runtime::init();

        std::filesystem::path location = sight::core::runtime::get_resource_file_path("app_ut");
        CHECK(std::filesystem::exists(location));

        sight::core::runtime::add_modules(location);
        sight::core::runtime::load_module("sight::module::app");
        sight::core::runtime::load_module("parameters_test");

        sight::app::extension::parameters::sptr app_config_param;
        app_config_param = sight::app::extension::parameters::get_default();
        app_config_param->clear_registry();
        app_config_param->parse_plugin_infos();
    }
};

} // namespace

TEST_SUITE("sight::app::parameters")
{
//------------------------------------------------------------------------------

    static void app_config_parameters_test()
    {
        sight::app::extension::parameters::sptr app_config_param;
        app_config_param = sight::app::extension::parameters::get_default();

        sight::app::field_adaptor_t parameters = app_config_param->get_parameters("id1");
        CHECK(!parameters.empty());
        CHECK_EQ(std::string("value1"), parameters["param1"]);

        sight::app::field_adaptor_t parameters2 = app_config_param->get_parameters("id2");
        CHECK(!parameters2.empty());
        CHECK_EQ(std::string("value3"), parameters2["param3"]);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "app_config_parameters")
    {
        app_config_parameters_test();
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "concurrent_access_to_parameters")
    {
        std::vector<std::future<void> > futures;
        futures.reserve(20);
        for(unsigned int i = 0 ; i < 20 ; ++i)
        {
            futures.push_back(std::async(std::launch::async, app_config_parameters_test));
        }

        for(auto& future : futures)
        {
            const auto status = future.wait_for(std::chrono::seconds(1));
            CHECK(status == std::future_status::ready);
            future.get(); // Trigger exceptions
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
