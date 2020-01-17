/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/ExtensionPoint.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/Runtime.hpp>

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
    runtime->addDefaultBundles();
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

void RuntimeTest::testRuntime()
{
    ::fwRuntime::Runtime* runtime = ::fwRuntime::Runtime::getDefault();

    // Test bundle dataReg
    CPPUNIT_ASSERT(runtime->findBundle("dataReg"));
    std::shared_ptr< ::fwRuntime::Bundle > bundle = runtime->findBundle("dataReg");
    bundle->setEnable(true);
    CPPUNIT_ASSERT(bundle->isEnable());

    // Test bundle servicesReg
    CPPUNIT_ASSERT(runtime->findBundle("servicesReg"));
    std::shared_ptr< ::fwRuntime::Bundle > bundle2 = runtime->findBundle("servicesReg");
    bundle2->setEnable(true);
    CPPUNIT_ASSERT(bundle2->isEnable());

    // Test runtime extensions
    CPPUNIT_ASSERT(runtime->findExtensionPoint("::fwServices::registry::ServiceFactory"));
    CPPUNIT_ASSERT(runtime->findExtensionPoint("::fwServices::registry::ServiceConfig"));
    CPPUNIT_ASSERT(runtime->findExtensionPoint("::fwServices::registry::AppConfig"));
}

//------------------------------------------------------------------------------

void RuntimeTest::testOperations()
{
    const auto location = ::fwRuntime::Runtime::getDefault()->getWorkingPath() / BUNDLE_RC_PREFIX;

    // Bundle location prototype
    auto path = ::fwRuntime::getBundleResourcePath(std::string("servicesReg"));
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1", path);

    path = ::fwRuntime::getBundleResourcePath("notExistingBundle");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    // Full path prototype
    path = ::fwRuntime::getBundleResourceFilePath("servicesReg-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/plugin.xml", path);

    path = ::fwRuntime::getBundleResourceFilePath("servicesReg-0.1/not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/not_existing.file", path);

    path = ::fwRuntime::getBundleResourceFilePath("notExistingBundle-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    path = ::fwRuntime::getBundleResourceFilePath("wrong_version_format-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);

    path = ::fwRuntime::getLibraryResourceFilePath("fwLibrary-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "fwLibrary-0.1/plugin.xml", path);

    path = ::fwRuntime::getResourceFilePath("fwLibrary-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "fwLibrary-0.1/plugin.xml", path);

    path = ::fwRuntime::getResourceFilePath("servicesReg-0.1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/plugin.xml", path);

    // (Bundle, path) prototype
    path = ::fwRuntime::getBundleResourceFilePath("servicesReg", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/plugin.xml", path);

    path = ::fwRuntime::getBundleResourceFilePath("servicesReg", "not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg-0.1/not_existing.file", path);

    path = ::fwRuntime::getBundleResourceFilePath("notExistingBundle", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(std::filesystem::path(), path);
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwTools
