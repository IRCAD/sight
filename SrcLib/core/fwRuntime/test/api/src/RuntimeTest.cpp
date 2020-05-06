/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include <fwRuntime/detail/ExtensionPoint.hpp>
#include <fwRuntime/detail/Module.hpp>
#include <fwRuntime/detail/Runtime.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/Plugin.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRuntime::ut::RuntimeTest );

namespace fwRuntime
{
namespace ut
{

//------------------------------------------------------------------------------

RuntimeTest::RuntimeTest()
{
    // Set up context before running all tests.
    ::fwRuntime::init();
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

void RuntimeTest::testModule()
{
    auto module = ::fwRuntime::loadModule(std::string("dataReg"));

    CPPUNIT_ASSERT_EQUAL(std::string("dataReg"),  module->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(Version("0.1"),  module->getVersion());
    // No good parameter test for now, but at least test without any parameter
    CPPUNIT_ASSERT_EQUAL(false,  module->hasParameter("test"));
    CPPUNIT_ASSERT_EQUAL(std::string(),  module->getParameterValue("test"));

    auto runtime           = ::fwRuntime::Runtime::getDefault();
    const auto libLocation = runtime->getWorkingPath() / MODULE_LIB_PREFIX;
    CPPUNIT_ASSERT_EQUAL(libLocation / "dataReg-0.1",  module->getLibraryLocation());
    const auto rcLocation = runtime->getWorkingPath() / MODULE_RC_PREFIX;
    CPPUNIT_ASSERT_EQUAL(rcLocation / "dataReg-0.1",  module->getResourcesLocation());

    const auto extensions = module->getExtensions();
    CPPUNIT_ASSERT_EQUAL(true, extensions.empty());

    CPPUNIT_ASSERT_EQUAL(std::string("::dataReg::Plugin"), module->getClass());
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
    const auto location = ::fwRuntime::Runtime::getDefault()->getWorkingPath() / MODULE_RC_PREFIX;

    // module location prototype
    auto path = ::fwRuntime::getModuleResourcePath(std::string("servicesReg"));
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1", path);

    path = ::fwRuntime::getModuleResourcePath("notExistingBundle");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    // Full path prototype
    path = ::fwRuntime::getModuleResourceFilePath("servicesReg-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/plugin.xml", path);

    path = ::fwRuntime::getModuleResourceFilePath("servicesReg-0.1/not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/not_existing.file", path);

    path = ::fwRuntime::getModuleResourceFilePath("notExistingBundle-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    path = ::fwRuntime::getModuleResourceFilePath("wrong_version_format-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    path = ::fwRuntime::getLibraryResourceFilePath("fwLibrary-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "fwLibrary-0.1/plugin.xml", path);

    path = ::fwRuntime::getResourceFilePath("fwLibrary-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "fwLibrary-0.1/plugin.xml", path);

    path = ::fwRuntime::getResourceFilePath("servicesReg-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/plugin.xml", path);

    // (module, path) prototype
    path = ::fwRuntime::getModuleResourceFilePath("servicesReg", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/plugin.xml", path);

    path = ::fwRuntime::getModuleResourceFilePath("servicesReg", "not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/not_existing.file", path);

    path = ::fwRuntime::getModuleResourceFilePath("notExistingBundle", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwTools
