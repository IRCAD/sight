/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "RuntimeTest.hpp"

#include <core/runtime/detail/ExtensionPoint.hpp>
#include <core/runtime/detail/Module.hpp>
#include <core/runtime/detail/Runtime.hpp>
#include <core/runtime/Extension.hpp>
#include <core/runtime/helper.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/Plugin.hpp>
#include <core/runtime/profile/Profile.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::core::runtime::ut::RuntimeTest );

namespace sight::core::runtime
{
namespace ut
{

//------------------------------------------------------------------------------

RuntimeTest::RuntimeTest()
{
    // Set up context before running all tests.
    core::runtime::init();
}

//------------------------------------------------------------------------------

void RuntimeTest::setUp()
{
}

//------------------------------------------------------------------------------

void RuntimeTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void RuntimeTest::testLibrary()
{
    bool success = core::runtime::loadLibrary(std::string("sight_utest"));
    CPPUNIT_ASSERT_EQUAL(true, success);

    success = core::runtime::loadLibrary(std::string("sight_utest"));
    CPPUNIT_ASSERT_EQUAL(true, success);

    success = core::runtime::loadLibrary(std::string("foo"));
    CPPUNIT_ASSERT_EQUAL(false, success);
}

//------------------------------------------------------------------------------

void RuntimeTest::testModule()
{
    auto module = core::runtime::loadModule(std::string("::sight::module::utest"));

    CPPUNIT_ASSERT_MESSAGE("Module not found", module);
    CPPUNIT_ASSERT_EQUAL(std::string("sight::module::utest"),  module->getIdentifier());
    // No good parameter test for now, but at least test without any parameter
    CPPUNIT_ASSERT_EQUAL(false,  module->hasParameter("test"));
    CPPUNIT_ASSERT_EQUAL(std::string(),  module->getParameterValue("test"));

    auto runtime           = core::runtime::Runtime::getDefault();
    const auto libLocation = runtime->getWorkingPath() / MODULE_LIB_PREFIX;
    CPPUNIT_ASSERT_EQUAL(libLocation,  module->getLibraryLocation());
    const auto rcLocation = runtime->getWorkingPath() / MODULE_RC_PREFIX;
    CPPUNIT_ASSERT_EQUAL(rcLocation / "module_utest",  module->getResourcesLocation());

    CPPUNIT_ASSERT_EQUAL(std::string("sight::module::utest::Plugin"), module->getClass());
    auto plugin = module->getPlugin();
    CPPUNIT_ASSERT_MESSAGE("Plugin is null", nullptr != plugin);

    auto moduleFromPlugin = plugin->getModule();
    CPPUNIT_ASSERT_EQUAL(module, moduleFromPlugin);

    CPPUNIT_ASSERT_EQUAL(true, module->isEnabled());
    CPPUNIT_ASSERT_EQUAL(true, module->isStarted());
}

//------------------------------------------------------------------------------

void RuntimeTest::testOperations()
{
    const auto location = core::runtime::Runtime::getDefault()->getWorkingPath() / MODULE_RC_PREFIX;

    // module location prototype
    auto path = core::runtime::getModuleResourcePath(std::string("::sight::module::utest"));
    CPPUNIT_ASSERT_EQUAL(location / "module_utest", path);

    path = core::runtime::getModuleResourcePath("notExistingBundle");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    // Full path prototype
    path = core::runtime::getModuleResourceFilePath("::sight::module::utest/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/plugin.xml", path);

    path = core::runtime::getModuleResourceFilePath("::sight::module::utest/not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/not_existing.file", path);

    path = core::runtime::getModuleResourceFilePath("notExistingBundle/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    path = core::runtime::getModuleResourceFilePath("wrong::module::format/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    path = core::runtime::getLibraryResourceFilePath("fwLibrary/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "fwLibrary/plugin.xml", path);

    path = core::runtime::getResourceFilePath("fwLibrary/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "fwLibrary/plugin.xml", path);

    path = core::runtime::getResourceFilePath("module_utest/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/plugin.xml", path);

    // (module, path) prototype
    path = core::runtime::getModuleResourceFilePath("::sight::module::utest", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/plugin.xml", path);

    path = core::runtime::getModuleResourceFilePath("::sight::module::utest", "not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "module_utest/not_existing.file", path);

    path = core::runtime::getModuleResourceFilePath("notExistingBundle", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);
}

//------------------------------------------------------------------------------

void RuntimeTest::testPropertyTree()
{
    {
        boost::property_tree::ptree config;
        config.put("test.yes", "foo");
        config.put("test.no", "no");
        config.put("test.true", "true");
        config.put("test.false", "false");

        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value(config, "test.yes", true), core::Exception);
        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value(config, "test.no", true), core::Exception);
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

        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value<int>(config, "test.real", 23), core::Exception);
        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value<int>(config, "test.string", 23), core::Exception);
        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value<float>(config, "test.string", 23.f), core::Exception);
        CPPUNIT_ASSERT_THROW(core::runtime::get_ptree_value<double>(config, "test.string", 23.), core::Exception);

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

} // namespace ut
} // namespace sight::core::runtime
