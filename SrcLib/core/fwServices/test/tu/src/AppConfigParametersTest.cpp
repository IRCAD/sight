/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "AppConfigParametersTest.hpp"

#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/AppConfigParameters.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Runtime.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::AppConfigParametersTest );

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

void AppConfigParametersTest::setUp()
{
    // Set up context before running a test.
    //Bundles location
    ::fwRuntime::Runtime* runtime = ::fwRuntime::Runtime::getDefault();
    runtime->addDefaultBundles();

    ::boost::filesystem::path location = runtime->getWorkingPath() / "share/tu_exec_fwServices-0.0";
    CPPUNIT_ASSERT(::boost::filesystem::exists(location));

    runtime->addBundles(location);
    CPPUNIT_ASSERT(runtime->bundlesBegin() != runtime->bundlesEnd());

    std::shared_ptr< ::fwRuntime::Bundle > bundle = runtime->findBundle("servicesReg");
    CPPUNIT_ASSERT_MESSAGE("'servicesReg bundle not found !'", bundle);
    bundle->setEnable(true);
    CPPUNIT_ASSERT(bundle->isEnable());

    // Test bundle servicesReg
    std::shared_ptr< ::fwRuntime::Bundle > bundle2 = runtime->findBundle("AppConfigParametersTest");
    CPPUNIT_ASSERT_MESSAGE("'AppConfigParametersTest' bundle not found", bundle2);
    bundle2->setEnable(true);
    CPPUNIT_ASSERT(bundle2->isEnable());

    ::fwServices::registry::AppConfigParameters::sptr appConfigParam;
    appConfigParam = ::fwServices::registry::AppConfigParameters::getDefault();
    appConfigParam->parseBundleInformation();
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::tearDown()
{
    // Clean up after the test run.
    ::fwServices::registry::AppConfigParameters::sptr appConfigParameters;
    appConfigParameters = ::fwServices::registry::AppConfigParameters::getDefault();
    appConfigParameters->clearRegistry();
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::appConfigParametersTest()
{
    ::fwServices::registry::AppConfigParameters::sptr appConfigParam;
    appConfigParam = ::fwServices::registry::AppConfigParameters::getDefault();

    ::fwServices::registry::FieldAdaptorType parameters = appConfigParam->getParameters("id1");
    CPPUNIT_ASSERT(!parameters.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value1"), parameters["param1"]);

    ::fwServices::registry::FieldAdaptorType parameters2 = appConfigParam->getParameters("id2");
    CPPUNIT_ASSERT(!parameters2.empty());
    CPPUNIT_ASSERT_EQUAL(std::string("value3"), parameters2["param3"]);
}

//------------------------------------------------------------------------------

void AppConfigParametersTest::concurentAccessToAppConfigParametersTest()
{
    const auto fn = std::bind(&AppConfigParametersTest::appConfigParametersTest, this);
    std::vector< std::future<void> > futures;
    for (unsigned int i = 0; i < 20; ++i)
    {
        futures.push_back( std::async(std::launch::async, fn) );
    }

    for (auto& future : futures)
    {
        const auto status = future.wait_for(std::chrono::seconds(1));
        CPPUNIT_ASSERT(status == std::future_status::ready);
        future.get(); // Trigger exceptions
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
