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

#include <fwRuntime/Extension.hpp>
#include <fwRuntime/impl/ExtensionPoint.hpp>
#include <fwRuntime/impl/Module.hpp>
#include <fwRuntime/impl/Runtime.hpp>
#include <fwRuntime/operations.hpp>

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
    // Set up context before running a test.
    ::fwRuntime::Runtime* runtime = ::fwRuntime::Runtime::getDefault();
    runtime->addDefaultModules();
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
