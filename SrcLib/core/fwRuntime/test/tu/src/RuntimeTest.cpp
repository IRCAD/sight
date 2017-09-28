/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "RuntimeTest.hpp"

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/ExtensionPoint.hpp>
#include <fwRuntime/operations.hpp>
#include <fwRuntime/Runtime.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

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
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg_0-1", path);

    path = ::fwRuntime::getBundleResourcePath("notExistingBundle");
    CPPUNIT_ASSERT_EQUAL(::boost::filesystem::path(), path);

    // Full path prototype
    path = ::fwRuntime::getBundleResourceFilePath("servicesReg_0-1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg_0-1/plugin.xml", path);

    path = ::fwRuntime::getBundleResourceFilePath("servicesReg_0-1/not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg_0-1/not_existing.file", path);

    path = ::fwRuntime::getBundleResourceFilePath("notExistingBundle_0-1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(::boost::filesystem::path(), path);

    path = ::fwRuntime::getBundleResourceFilePath("wrong_version_format_0-1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(::boost::filesystem::path(), path);

    path = ::fwRuntime::getLibraryResourceFilePath("fwLibrary_0-1/plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "fwLibrary_0-1/plugin.xml", path);

    // (Bundle, path) prototype
    path = ::fwRuntime::getBundleResourceFilePath("servicesReg", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg_0-1/plugin.xml", path);

    path = ::fwRuntime::getBundleResourceFilePath("servicesReg", "not_existing.file");
    CPPUNIT_ASSERT_EQUAL(location / "servicesReg_0-1/not_existing.file", path);

    path = ::fwRuntime::getBundleResourceFilePath("notExistingBundle", "plugin.xml");
    CPPUNIT_ASSERT_EQUAL(::boost::filesystem::path(), path);
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwTools

