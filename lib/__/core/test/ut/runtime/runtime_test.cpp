/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/ptree.hpp>
#include <core/runtime/extension.hpp>
#include <core/runtime/module.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/plugin.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/runtime/runtime.hpp>

#include <doctest/doctest.h>

#include <filesystem>

TEST_SUITE("sight::core::runtime")
{
//------------------------------------------------------------------------------

// Set up context before running all tests.
    static bool runtime_initialized = []()
                                      {
                                          sight::core::runtime::init();
                                          return true;
                                      }();

//------------------------------------------------------------------------------

    TEST_CASE("library")
    {
        bool success = sight::core::runtime::load_library(std::string("sight_utest"));
        CHECK_EQ(true, success);

        success = sight::core::runtime::load_library(std::string("sight_utest"));
        CHECK_EQ(true, success);

        success = sight::core::runtime::load_library(std::string("foo"));
        CHECK_EQ(false, success);
    }

//------------------------------------------------------------------------------

    TEST_CASE("module")
    {
        auto module = sight::core::runtime::load_module(std::string("sight::module::utest"));

        CHECK_MESSAGE(module, "Module not found");
        CHECK_EQ(std::string("sight::module::utest"), module->identifier());
        // No good parameter test for now, but at least test without any parameter
        CHECK_EQ(false, module->has_parameter("test"));
        CHECK_EQ(std::string(), module->get_parameter_value("test"));

        const auto working_path = sight::core::runtime::working_path();
        const auto lib_location = working_path / MODULE_LIB_PREFIX;
        CHECK_EQ(lib_location, module->get_library_location());
        const auto rc_location = working_path / MODULE_RC_PREFIX;
        CHECK_EQ(rc_location / "module_utest", module->get_resources_location());

        CHECK_EQ(std::string("sight::module::utest::plugin"), module->get_class());
        auto plugin = module->get_plugin();
        CHECK_MESSAGE(nullptr != plugin, "plugin is null");

        auto module_from_plugin = plugin->get_module();
        CHECK_EQ(module, module_from_plugin);

        CHECK_EQ(true, module->enabled());
        CHECK_EQ(true, module->is_started());
    }

//------------------------------------------------------------------------------

    TEST_CASE("operations")
    {
        const auto location = sight::core::runtime::working_path() / MODULE_RC_PREFIX;

        // module location prototype
        auto path = sight::core::runtime::get_module_resource_path(std::string("sight::module::utest"));
        CHECK_EQ(location / "module_utest", path);

        path = sight::core::runtime::get_module_resource_path("notExistingBundle");
        CHECK_EQ(std::filesystem::path(), path);

        // Full path prototype
        path = sight::core::runtime::get_module_resource_file_path("sight::module::utest/plugin.xml");
        CHECK_EQ(location / "module_utest/plugin.xml", path);

        path = sight::core::runtime::get_module_resource_file_path("sight::module::utest/not_existing.file");
        CHECK_EQ(location / "module_utest/not_existing.file", path);

        path = sight::core::runtime::get_module_resource_file_path("notExistingBundle/plugin.xml");
        CHECK_EQ(std::filesystem::path(), path);

        path = sight::core::runtime::get_module_resource_file_path("wrong::module::format/plugin.xml");
        CHECK_EQ(std::filesystem::path(), path);

        path = sight::core::runtime::get_library_resource_file_path("core/plugin.xsd");
        CHECK_EQ(location / "core/plugin.xsd", path);

        path = sight::core::runtime::get_library_resource_file_path("sight::core/plugin.xsd");
        CHECK_EQ(location / "core/plugin.xsd", path);

        path = sight::core::runtime::get_library_resource_file_path("sight::core/plugin.xsd");
        CHECK_EQ(location / "core/plugin.xsd", path);

        path = sight::core::runtime::get_resource_file_path("core/plugin.xsd");
        CHECK_EQ(location / "core/plugin.xsd", path);

        path = sight::core::runtime::get_resource_file_path("module_utest/plugin.xml");
        CHECK_EQ(location / "module_utest/plugin.xml", path);

        // (module, path) prototype
        path = sight::core::runtime::get_module_resource_file_path("sight::module::utest", "plugin.xml");
        CHECK_EQ(location / "module_utest/plugin.xml", path);

        path = sight::core::runtime::get_module_resource_file_path("sight::module::utest", "not_existing.file");
        CHECK_EQ(location / "module_utest/not_existing.file", path);

        path = sight::core::runtime::get_module_resource_file_path("notExistingBundle", "plugin.xml");
        CHECK_EQ(std::filesystem::path(), path);
    }
} // TEST_SUITE
