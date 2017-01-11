/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/Bundle.hpp>

#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/AppConfigParameters.hpp>

#include <fwTest/helper/Thread.hpp>

#include "AppConfigParametersTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::AppConfigParametersTest );

namespace fwServices
{
namespace ut
{

void AppConfigParametersTest::setUp()
{
    // Set up context before running a test.
    //Bundles location
    ::boost::filesystem::path bundlesLocation = ::boost::filesystem::current_path() /
                                                ::boost::filesystem::path(BUNDLE_PREFIX);
    CPPUNIT_ASSERT(::boost::filesystem::exists(bundlesLocation));
    ::fwRuntime::Runtime* runtime = ::fwRuntime::Runtime::getDefault();

    // Read bundles
    runtime->addBundles(bundlesLocation);

    ::boost::filesystem::path location = ::boost::filesystem::current_path() / "share/tu_exec_fwServices_0-0";
    if( !::boost::filesystem::exists( location ) )
    {
        location = ::boost::filesystem::current_path() / "share/tu_xml_fwServices_0-0";
    }
    CPPUNIT_ASSERT(::boost::filesystem::exists(location));

    runtime->addBundles(location);
    CPPUNIT_ASSERT(runtime->bundlesBegin() !=  runtime->bundlesEnd());

    std::shared_ptr< ::fwRuntime::Bundle > bundle = runtime->findBundle("servicesReg");
    CPPUNIT_ASSERT_MESSAGE("'servicesReg bundle not found !'",bundle);
    bundle->setEnable(true);
    CPPUNIT_ASSERT(bundle->isEnable());

    // Test bundle servicesReg
    std::shared_ptr< ::fwRuntime::Bundle > bundle2 = runtime->findBundle("AppConfigParametersTest");
    CPPUNIT_ASSERT_MESSAGE("'AppConfigParametersTest' bundle not found",bundle2);
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
    const unsigned int nbThreads = 20;
    std::vector< SPTR(::fwTest::helper::Thread) > threads;
    for (int i = 0; i<nbThreads; ++i)
    {
        SPTR(::fwTest::helper::Thread) thread;
        thread = std::shared_ptr< ::fwTest::helper::Thread >(
            new ::fwTest::helper::Thread(std::bind(&AppConfigParametersTest::appConfigParametersTest, this)));
        threads.push_back(thread);
    }

    for (int i = 0; i<nbThreads; ++i)
    {
        std::stringstream str;
        str << "thread " << i;
        CPPUNIT_ASSERT_MESSAGE(str.str(), threads[i]->timedJoin(1000));
    }

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
