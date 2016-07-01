/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <set>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/ExtensionPoint.hpp>

#include "RuntimeTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwRuntime::ut::RuntimeTest );

namespace fwRuntime
{
namespace ut
{

void RuntimeTest::setUp()
{
    // Set up context before running a test.
}

void RuntimeTest::tearDown()
{
    // Clean up after the test run.
}

void RuntimeTest::testRuntime()
{
    // Bundles location
    ::boost::filesystem::path location = ::boost::filesystem::current_path() /std::string( std::string(
                                                                                               BUNDLE_PREFIX) + "/");

    CPPUNIT_ASSERT(::boost::filesystem::exists(location));

    ::fwRuntime::Runtime * runtime = ::fwRuntime::Runtime::getDefault();

    // Read bundles
    runtime->addBundles(location);
    CPPUNIT_ASSERT(runtime->bundlesBegin() !=  runtime->bundlesEnd());

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

} // namespace ut
} // namespace fwTools

