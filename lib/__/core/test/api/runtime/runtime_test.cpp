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

#include "runtime_test.hpp"

#include <core/runtime/extension.hpp>
#include <core/runtime/helper.hpp>
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

void runtime_test::test_property_tree()
{
    {
        boost::property_tree::ptree config;
        config.put("test.true", "true");
        config.put("test.false", "false");
        bool value = core::runtime::get_ptree_value(config, "test.true", true);
        CPPUNIT_ASSERT_EQUAL(true, value);
        value = core::runtime::get_ptree_value(config, "test.false", true);
        CPPUNIT_ASSERT_EQUAL(false, value);
        value = core::runtime::get_ptree_value(config, "not_set!!!", true);
        CPPUNIT_ASSERT_EQUAL(true, value);
        value = core::runtime::get_ptree_value(config, "not_set!!!", false);
        CPPUNIT_ASSERT_EQUAL(false, value);
    }

    {
        boost::property_tree::ptree config;
        config.put("test.int", "42");
        config.put("test.real", "2.34");
        config.put("test.string", "digits");

        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value<int>(config, "test.real", 23), core::exception);
        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value<int>(config, "test.string", 23), core::exception);
        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value<float>(config, "test.string", 23.F), core::exception);
        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value<double>(config, "test.string", 23.), core::exception);

        int value = core::runtime::get_ptree_value(config, "test.int", 23);
        CPPUNIT_ASSERT_EQUAL(42, value);
        value = core::runtime::get_ptree_value(config, "not_set!!!", 23);
        CPPUNIT_ASSERT_EQUAL(23, value);

        double real = core::runtime::get_ptree_value(config, "test.int", 23);
        CPPUNIT_ASSERT_EQUAL(42., real);
        real = core::runtime::get_ptree_value(config, "test.real", 42.3);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(2.34, real, 0.0001);

        std::string str = core::runtime::get_ptree_value(config, "test.string", std::string("foo"));
        CPPUNIT_ASSERT_EQUAL(std::string("digits"), str);
        str = core::runtime::get_ptree_value(config, "test.missing_string", std::string("foo"));
        CPPUNIT_ASSERT_EQUAL(std::string("foo"), str);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::core::runtime::ut
