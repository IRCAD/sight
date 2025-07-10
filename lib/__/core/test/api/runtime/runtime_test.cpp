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

#include "runtime_test.hpp"

#include <core/ptree.hpp>
#include <core/runtime/extension.hpp>
#include <core/runtime/module.hpp>
#include <core/runtime/path.hpp>
#include <core/runtime/plugin.hpp>
#include <core/runtime/profile/profile.hpp>
#include <core/runtime/runtime.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::runtime::ut::runtime_test);

namespace sight::core::runtime::ut
{

//------------------------------------------------------------------------------

runtime_test::runtime_test()
{
    // Set up context before running all tests.
    core::runtime::init();
}

//------------------------------------------------------------------------------

void runtime_test::setUp()
{
}

//------------------------------------------------------------------------------

void runtime_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void runtime_test::test_library()
{
    bool success = core::runtime::load_library(std::string("sight_utest"));
    CPPUNIT_ASSERT_EQUAL(true, success);

    success = core::runtime::load_library(std::string("sight_utest"));
    CPPUNIT_ASSERT_EQUAL(true, success);

    success = core::runtime::load_library(std::string("foo"));
    CPPUNIT_ASSERT_EQUAL(false, success);
}

//------------------------------------------------------------------------------

void runtime_test::test_module()
{
    auto module = core::runtime::load_module(std::string("sight::module::utest"));

    CPPUNIT_ASSERT_MESSAGE("Module not found", module);
    CPPUNIT_ASSERT_EQUAL(std::string("sight::module::utest"), module->identifier());
    // No good parameter test for now, but at least test without any parameter
    CPPUNIT_ASSERT_EQUAL(false, module->has_parameter("test"));
    CPPUNIT_ASSERT_EQUAL(std::string(), module->get_parameter_value("test"));

    const auto working_path = core::runtime::working_path();
    const auto lib_location = working_path / MODULE_LIB_PREFIX;
    CPPUNIT_ASSERT_EQUAL(lib_location, module->get_library_location());
    const auto rc_location = working_path / MODULE_RC_PREFIX;
    CPPUNIT_ASSERT_EQUAL(rc_location / "module_utest", module->get_resources_location());

    CPPUNIT_ASSERT_EQUAL(std::string("sight::module::utest::plugin"), module->get_class());
    auto plugin = module->get_plugin();
    CPPUNIT_ASSERT_MESSAGE("plugin is null", nullptr != plugin);

    auto module_from_plugin = plugin->get_module();
    CPPUNIT_ASSERT_EQUAL(module, module_from_plugin);

    CPPUNIT_ASSERT_EQUAL(true, module->enabled());
    CPPUNIT_ASSERT_EQUAL(true, module->is_started());
}

//------------------------------------------------------------------------------

void runtime_test::test_operations()
{
    const auto location = core::runtime::working_path() / MODULE_RC_PREFIX;

    // module location prototype
    auto path = core::runtime::get_module_resource_path(std::string("sight::module::utest"));
    CPPUNIT_ASSERT_EQUAL(location / "module_utest", path);

    path = core::runtime::get_module_resource_path("notExistingBundle");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    // Full path prototype
    path = core::runtime::get_module_resource_file_path("sight::module::utest/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/plugin.xml", path);

    path = core::runtime::get_module_resource_file_path("sight::module::utest/not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/not_existing.file", path);

    path = core::runtime::get_module_resource_file_path("notExistingBundle/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    path = core::runtime::get_module_resource_file_path("wrong::module::format/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    path = core::runtime::get_library_resource_file_path("core/plugin.xsd");
    CPPUNIT_ASSERT_EQUAL(location / "core/plugin.xsd", path);

    path = core::runtime::get_library_resource_file_path("sight::core/plugin.xsd");
    CPPUNIT_ASSERT_EQUAL(location / "core/plugin.xsd", path);

    path = core::runtime::get_library_resource_file_path("sight::core/plugin.xsd");
    CPPUNIT_ASSERT_EQUAL(location / "core/plugin.xsd", path);

    path = core::runtime::get_resource_file_path("core/plugin.xsd");
    CPPUNIT_ASSERT_EQUAL(location / "core/plugin.xsd", path);

    path = core::runtime::get_resource_file_path("module_utest/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/plugin.xml", path);

    // (module, path) prototype
    path = core::runtime::get_module_resource_file_path("sight::module::utest", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/plugin.xml", path);

    path = core::runtime::get_module_resource_file_path("sight::module::utest", "not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/not_existing.file", path);

    path = core::runtime::get_module_resource_file_path("notExistingBundle", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::ut
