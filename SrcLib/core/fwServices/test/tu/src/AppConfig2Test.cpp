/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "AppConfig2Test.hpp"

#include "fwServices/registry/ActiveWorkers.hpp"
#include "fwServices/registry/AppConfig2.hpp"
#include "fwServices/registry/ObjectService.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Runtime.hpp>

#include <fwTest/helper/Thread.hpp>

#include <TestService.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::AppConfig2Test );

namespace fwServices
{
namespace ut
{

// Wait at worst 1s for a given condition
#define WAIT(cond) \
    ::fwCore::TimeStamp BOOST_PP_CAT(timeStamp, __LINE__); \
    BOOST_PP_CAT(timeStamp, __LINE__).setLifePeriod(1000); \
    BOOST_PP_CAT(timeStamp, __LINE__).modified(); \
    while(!(cond) && !BOOST_PP_CAT(timeStamp, __LINE__).periodExpired()) \
    { \
        std::this_thread::sleep_for( std::chrono::milliseconds(10)); \
    }

#define WAIT_SERVICE_STARTED(srv)  \
    WAIT(::fwTools::fwID::getObject(srv) != nullptr && \
         ::fwServices::IService::dynamicCast(::fwTools::fwID::getObject(srv))->getStatus() \
         == ::fwServices::IService::STARTED)

void AppConfig2Test::setUp()
{
    // Set up context before running a test.

    ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();
}

//------------------------------------------------------------------------------

void AppConfig2Test::tearDown()
{
    // Clean up after the test run.
    if(m_appConfigMgr)
    {
        // If everything went well, the manager should have been destroyed
        // This means a test failed, thus we need to clean everything properly, otherwise
        // We will get an assert from the destructor and we will not get the cppunit report in the console
        m_appConfigMgr->stopAndDestroy();
        m_appConfigMgr = nullptr;
    }

    ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

void AppConfig2Test::parametersConfigTest()
{
    ::fwServices::registry::AppConfig2::sptr currentAppConfig = ::fwServices::registry::AppConfig2::getDefault();

    const std::string configId(::fwServices::registry::AppConfig2::getUniqueIdentifier());
    const std::string group("parametersGroup");
    const std::string desc("Description");
    const std::string bundleId("dataReg");
    const std::string bundleVersion("0-1");
    ::fwServices::registry::AppInfo::ParametersType parameters;

    parameters["TEST_IMAGE"]   = "";
    parameters["UID_SERVICE2"] = "myTestService2";

    ::fwRuntime::ConfigurationElement::csptr config = this->buildParametersConfig();

    currentAppConfig->addAppInfo(configId, group, desc, parameters, config, bundleId, bundleVersion);

    std::vector< std::string > allCconfigs = currentAppConfig->getAllConfigs();
    CPPUNIT_ASSERT( !allCconfigs.empty());

    std::vector< std::string > configs = currentAppConfig->getConfigsFromGroup(group);
    CPPUNIT_ASSERT(!configs.empty());

    ::fwServices::registry::FieldAdaptorType replaceFields;
    replaceFields["TEST_IMAGE"] = "objectUUID";

    ::fwRuntime::ConfigurationElement::csptr configEltAdaptedConst;
    configEltAdaptedConst = currentAppConfig->getAdaptedTemplateConfig(configId, replaceFields, false);

    ::fwRuntime::ConfigurationElement::sptr configEltAdapted;
    configEltAdapted = ::fwRuntime::ConfigurationElement::constCast(configEltAdaptedConst);

    std::string uid = configEltAdapted->getAttributeValue("uid");
    CPPUNIT_ASSERT_EQUAL(std::string("objectUUID"), uid );

    std::vector< ::fwRuntime::ConfigurationElement::sptr >  servicesCfg = configEltAdapted->find("service");

    std::string serviceUid1 = servicesCfg.at(0)->getAttributeValue("uid");
    CPPUNIT_ASSERT_EQUAL( std::string("myTestService1"), serviceUid1);

    std::string serviceUid2 = servicesCfg.at(1)->getAttributeValue("uid");
    CPPUNIT_ASSERT_EQUAL( std::string("myTestService2"), serviceUid2);
}

//------------------------------------------------------------------------------

::fwServices::AppConfigManager2::sptr AppConfig2Test::launchAppConfigMgr(
    const std::string& name,
    const ::fwRuntime::ConfigurationElement::csptr& config)
{
    const std::string configId(name);
    const std::string group("parametersGroup");
    const std::string desc("Description");
    const std::string bundleId("dummy");
    const std::string bundleVersion("0-1");
    ::fwServices::registry::AppInfo::ParametersType parameters;

    ::fwServices::registry::AppConfig2::sptr currentAppConfig = ::fwServices::registry::AppConfig2::getDefault();
    currentAppConfig->addAppInfo(configId, group, desc, parameters, config, bundleId, bundleVersion);

    auto appConfigMgr = ::fwServices::AppConfigManager2::New();
    appConfigMgr->setIsUnitTest(true);

    const ::fwServices::registry::FieldAdaptorType fields;
    appConfigMgr->setConfig( configId, fields );
    appConfigMgr->launch();

    return appConfigMgr;
}

//------------------------------------------------------------------------------

void AppConfig2Test::startStopTest()
{
    ::fwRuntime::ConfigurationElement::csptr config = this->buildStartStopConfig();
    m_appConfigMgr                                  = this->launchAppConfigMgr("startStopTest", config);

    // =================================================================================================================
    // Test manual start and stop of services, with or without data
    // =================================================================================================================

    auto data1 = std::dynamic_pointer_cast< ::fwData::Object >(::fwTools::fwID::getObject("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // This service doesn't exist in the config
    CPPUNIT_ASSERT(::fwTools::fwID::getObject("TestService142Uid") == nullptr);

    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(::fwTools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    // This service has no data and is started by the config
    {
        fwTools::Object::sptr gnsrv1 = ::fwTools::fwID::getObject("TestService1Uid");
        auto srv1                    = ::fwServices::ut::TestService::dynamicCast(gnsrv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
    }

    // This service has no data and is NOT started by the config
    {
        fwTools::Object::sptr gnsrv2 = ::fwTools::fwID::getObject("TestService2Uid");
        auto srv2                    = ::fwServices::IService::dynamicCast(gnsrv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STOPPED, srv2->getStatus());
        srv2->start().wait();
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
        srv2->stop().wait();
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STOPPED, srv2->getStatus());
    }

    // This service has a data and is NOT started by the config
    {
        fwTools::Object::sptr gnsrv3 = ::fwTools::fwID::getObject("TestService3Uid");
        auto srv3                    = ::fwServices::IService::dynamicCast(gnsrv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STOPPED, srv3->getStatus());
    }

    // This service has a data that is not present yet (WID), so it is even not created
    {
        fwTools::Object::sptr gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
        CPPUNIT_ASSERT(gnsrv4 == nullptr);
    }

    // =================================================================================================================
    // Test automatic start and stop of the service, with a single data
    // =================================================================================================================

    // Create the data
    ::fwData::Boolean::sptr data2 = ::fwData::Boolean::New();
    {
        ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
        WAIT_SERVICE_STARTED("TestService4Uid");

        // Now the service should have been started automatically
        {
            auto gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
            auto srv4   = ::fwServices::IService::dynamicCast(gnsrv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv4->getStatus());
        }

        // Remove the data
        ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);
        WAIT(::fwTools::fwID::getObject("TestService4Uid") == nullptr);

        // Now the service should have been stopped and destroyed automatically
        {
            auto gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
            CPPUNIT_ASSERT(gnsrv4 == nullptr);
        }

        // Register the data once again
        ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
        WAIT_SERVICE_STARTED("TestService4Uid");

        // Check again that the service was started automatically
        {
            auto gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
            auto srv4   = ::fwServices::IService::dynamicCast(gnsrv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv4->getStatus());
        }
    }

    // =================================================================================================================
    // Test automatic start and stop of the service, with multiple data (3)
    // =================================================================================================================

    {
        // Still one data is not yet available thus the service is not created
        {
            fwTools::Object::sptr gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            CPPUNIT_ASSERT(gnsrv5 == nullptr);
        }

        // Create the remaining data
        ::fwData::Boolean::sptr data4 = ::fwData::Boolean::New();

        ::fwServices::OSR::registerServiceOutput(data4, "out4", genDataSrv);
        WAIT_SERVICE_STARTED("TestService5Uid");

        // Now the service should have been started automatically
        {
            auto gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            auto srv5   = ::fwServices::IService::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());
        }

        // Remove one data
        ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);

        // Now the service should have been stopped and destroyed automatically
        {
            WAIT(::fwTools::fwID::getObject("TestService5Uid") == nullptr);
            auto gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            CPPUNIT_ASSERT(gnsrv5 == nullptr);
            // Test as well service 4, just to be sure
            WAIT(::fwTools::fwID::getObject("TestService4Uid") == nullptr);
            auto gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
            CPPUNIT_ASSERT(gnsrv4 == nullptr);
        }

        // Put everything back
        ::fwServices::ut::TestService::s_START_COUNTER  = 0;
        ::fwServices::ut::TestService::s_UPDATE_COUNTER = 0;
        ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
        WAIT_SERVICE_STARTED("TestService5Uid");

        // Now the service should have been started automatically, check start order as well
        {
            auto gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            auto srv5   = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());
            CPPUNIT_ASSERT_EQUAL(0u, srv5->getStartOrder());
            CPPUNIT_ASSERT_EQUAL(1u, srv5->getUpdateOrder());

            // Test as well service 4, just to be sure
            auto gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
            auto srv4   = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv4);
            CPPUNIT_ASSERT(gnsrv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(1u, srv4->getStartOrder());
            CPPUNIT_ASSERT_EQUAL(0u, srv4->getUpdateOrder());
        }
    }

    // =================================================================================================================
    // Test data swapping
    // =================================================================================================================

    {
        // Test initial status (started because of the previous test)
        {
            auto gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            auto srv5   = ::fwServices::IService::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());
        }

        // Swap the data
        ::fwData::Boolean::sptr data5 = ::fwData::Boolean::New();

        ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);
        WAIT(::fwTools::fwID::getObject("TestService5Uid") == nullptr);
        ::fwServices::OSR::registerServiceOutput(data5, "out2", genDataSrv);
        WAIT_SERVICE_STARTED("TestService5Uid");

        {
            auto gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            auto srv5   = ::fwServices::IService::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());
        }
    }

    m_appConfigMgr->stopAndDestroy();

    m_appConfigMgr = nullptr;
}

//------------------------------------------------------------------------------

void AppConfig2Test::autoConnectTest()
{
    ::fwRuntime::ConfigurationElement::csptr config = this->buildAutoConnectTestConfig();
    m_appConfigMgr                                  = this->launchAppConfigMgr("autoConnectTest", config);

    // =================================================================================================================
    // Test autoconnect with available data
    // =================================================================================================================

    auto data1 = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject("data1Id"));
    auto data2 = ::fwData::Object::dynamicCast(::fwTools::fwID::getObject("data2Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    {
        fwTools::Object::sptr gnsrv1 = ::fwTools::fwID::getObject("TestService1Uid");
        auto srv1                    = ::fwServices::ut::TestService::dynamicCast(gnsrv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());

        fwTools::Object::sptr gnsrv2 = ::fwTools::fwID::getObject("TestService2Uid");
        auto srv2                    = ::fwServices::ut::TestService::dynamicCast(gnsrv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        fwTools::Object::sptr gnsrv3 = ::fwTools::fwID::getObject("TestService3Uid");
        auto srv3                    = ::fwServices::ut::TestSrvAutoconnect::dynamicCast(gnsrv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv3->getStatus());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getReceived());

        auto sig1 = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig1->asyncEmit();

        WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getReceived());

        srv2->resetIsUpdated();
        srv3->resetIsUpdated();

        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig2->asyncEmit();
        WAIT(srv2->getIsUpdated() && srv3->getReceived());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getReceived());
    }

    // =================================================================================================================
    // Test autoconnect with unavailable data at start
    // =================================================================================================================

    // Service used to generate data
    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(::fwTools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);
    {
        // Check that dependent services are not created
        {
            fwTools::Object::sptr gnsrv3 = ::fwTools::fwID::getObject("TestService4Uid");
            CPPUNIT_ASSERT(gnsrv3 == nullptr);
            fwTools::Object::sptr gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            CPPUNIT_ASSERT(gnsrv5 == nullptr);
        }

        // Create the data
        ::fwData::Boolean::sptr data3 = ::fwData::Boolean::New();

        ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);

        WAIT_SERVICE_STARTED("TestService4Uid");
        WAIT_SERVICE_STARTED("TestService5Uid");
        {
            fwTools::Object::sptr gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
            auto srv4                    = ::fwServices::ut::TestService::dynamicCast(gnsrv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv4->getStatus());
            CPPUNIT_ASSERT(!srv4->getIsUpdated());

            fwTools::Object::sptr gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            auto srv5                    = ::fwServices::ut::TestSrvAutoconnect::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getReceived());

            auto sig = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
            WAIT(srv5->getReceived());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(srv5->getReceived());
        }

        // Remove one data
        ::fwServices::OSR::unregisterServiceOutput("out3", genDataSrv);
        WAIT(::fwTools::fwID::getObject("TestService4Uid") == nullptr &&
             ::fwTools::fwID::getObject("TestService5Uid") == nullptr);
        {
            fwTools::Object::sptr gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
            CPPUNIT_ASSERT(gnsrv4 == nullptr);
            fwTools::Object::sptr gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            CPPUNIT_ASSERT(gnsrv5 == nullptr);
        }

        // Emit, that should be ok
        ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);

        WAIT_SERVICE_STARTED("TestService4Uid");
        WAIT_SERVICE_STARTED("TestService5Uid");
        {
            fwTools::Object::sptr gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
            auto srv4                    = ::fwServices::ut::TestService::dynamicCast(gnsrv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv4->getStatus());
            CPPUNIT_ASSERT(!srv4->getIsUpdated());

            fwTools::Object::sptr gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
            auto srv5                    = ::fwServices::ut::TestSrvAutoconnect::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getReceived());

            auto sig = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
            WAIT(srv5->getReceived());
            CPPUNIT_ASSERT(srv5->getReceived());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(srv5->getReceived());

            auto sig1 = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
            sig1->asyncEmit();
            WAIT(srv5->getIsUpdated());

            CPPUNIT_ASSERT(srv5->getIsUpdated());
        }
    }
}

//------------------------------------------------------------------------------

void AppConfig2Test::connectionTest()
{
    ::fwRuntime::ConfigurationElement::csptr config = this->buildConnectionConfig();
    m_appConfigMgr                                  = this->launchAppConfigMgr("connectionTest", config);

    // =================================================================================================================
    // Test connection without data
    // =================================================================================================================

    ::fwData::Composite::sptr composite;

    auto data1 = std::dynamic_pointer_cast< ::fwData::Object >(::fwTools::fwID::getObject("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test connection without data or with available data
    // =================================================================================================================

    fwTools::Object::sptr gnsrv1 = ::fwTools::fwID::getObject("TestService1Uid");
    auto srv1                    = ::fwServices::ut::TestService::dynamicCast(gnsrv1);
    CPPUNIT_ASSERT(srv1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    fwTools::Object::sptr gnsrv2 = ::fwTools::fwID::getObject("TestService2Uid");
    auto srv2                    = ::fwServices::ut::TestService::dynamicCast(gnsrv2);
    CPPUNIT_ASSERT(srv2 != nullptr);
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
    CPPUNIT_ASSERT(!srv2->getIsUpdated());

    fwTools::Object::sptr gnsrv4 = ::fwTools::fwID::getObject("TestService4Uid");
    auto srv4                    = ::fwServices::ut::TestService::dynamicCast(gnsrv4);
    CPPUNIT_ASSERT(srv4 != nullptr);
    WAIT(::fwServices::IService::STARTED == srv4->getStatus());
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv4->getStatus());

    // Check connection
    auto sig = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
    WAIT(srv1->getIsUpdated() && srv2->getIsUpdated());

    CPPUNIT_ASSERT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv2->getIsUpdated());

    // Service used to generate data
    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(::fwTools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    // =================================================================================================================
    // Test connection with unavailable data at start
    // =================================================================================================================

    {
        fwTools::Object::sptr gnsrv3 = ::fwTools::fwID::getObject("TestService3Uid");
        CPPUNIT_ASSERT(gnsrv3 == nullptr);
    }

    // Emit a signal just for fun, anyway the service doesn't exist
    sig = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig->emit();

    srv2->resetIsUpdated();
    CPPUNIT_ASSERT(!srv2->getIsUpdated());

    // Check connection data4 -> srv2
    auto data4 = std::dynamic_pointer_cast< ::fwData::Object >(::fwTools::fwID::getObject("data4Id"));
    CPPUNIT_ASSERT(data4 != nullptr);
    auto sig4 = data4->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig4->asyncEmit();
    WAIT(srv2->getIsUpdated());

    CPPUNIT_ASSERT(srv2->getIsUpdated());

    WAIT(!srv4->getIsUpdated());
    CPPUNIT_ASSERT(!srv4->getIsUpdated());
    CPPUNIT_ASSERT(!srv4->getIsUpdated2());

    // Create the missing data
    ::fwData::Boolean::sptr data2 = ::fwData::Boolean::New();
    ::fwData::Boolean::sptr data3 = ::fwData::Boolean::New();

    ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
    ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);
    WAIT_SERVICE_STARTED("TestService3Uid");
    {
        fwTools::Object::sptr gnsrv3 = ::fwTools::fwID::getObject("TestService3Uid");
        auto srv3                    = ::fwServices::ut::TestService::dynamicCast(gnsrv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        WAIT(::fwServices::IService::STARTED == srv3->getStatus());
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv3->getStatus());
        srv2->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        // Check "started" signal
        CPPUNIT_ASSERT(srv4->getIsUpdated());
        CPPUNIT_ASSERT(!srv4->getIsUpdated2());

        sig->asyncEmit();
        WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        srv1->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig2->asyncEmit();
        WAIT(srv1->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data4 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        sig4->asyncEmit();
        WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data3 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig3->asyncEmit();
        WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());
    }

    // Remove one data
    ::fwServices::OSR::unregisterServiceOutput("out3", genDataSrv);
    WAIT(::fwTools::fwID::getObject("TestService3Uid") == nullptr);

    // Service 3 should be removed
    {
        fwTools::Object::sptr gnsrv3 = ::fwTools::fwID::getObject("TestService3Uid");
        CPPUNIT_ASSERT(gnsrv3 == nullptr);
    }

    // Check that connection is still there for the working services
    srv2->resetIsUpdated();
    CPPUNIT_ASSERT(!srv2->getIsUpdated());
    sig->asyncEmit();
    WAIT(srv2->getIsUpdated());
    CPPUNIT_ASSERT(srv2->getIsUpdated());

    srv1->resetIsUpdated();
    CPPUNIT_ASSERT(!srv1->getIsUpdated());
    auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig2->asyncEmit();
    WAIT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    // Check connection data4 -> srv2
    srv2->resetIsUpdated();
    CPPUNIT_ASSERT(!srv2->getIsUpdated());

    sig4->asyncEmit();
    WAIT(srv2->getIsUpdated());

    CPPUNIT_ASSERT(srv2->getIsUpdated());

    // Check "stopped" signal
    CPPUNIT_ASSERT(srv4->getIsUpdated());
    CPPUNIT_ASSERT(srv4->getIsUpdated2());
    srv4->resetIsUpdated();
    srv4->resetIsUpdated2();
    CPPUNIT_ASSERT(!srv4->getIsUpdated2());

    // Add back data 3 and check connection again
    ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);
    WAIT_SERVICE_STARTED("TestService3Uid");

    {
        fwTools::Object::sptr gnsrv3 = ::fwTools::fwID::getObject("TestService3Uid");
        auto srv3                    = ::fwServices::ut::TestService::dynamicCast(gnsrv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv3->getStatus());
        srv2->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());
        CPPUNIT_ASSERT(srv4->getIsUpdated());
        CPPUNIT_ASSERT(!srv4->getIsUpdated2());

        sig->asyncEmit();
        WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        srv1->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig2->asyncEmit();
        WAIT(srv1->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data4 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        sig4->asyncEmit();
        WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data3 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig3->asyncEmit();
        WAIT(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());
    }

}

//------------------------------------------------------------------------------

void AppConfig2Test::optionalKeyTest()
{
    ::fwRuntime::ConfigurationElement::csptr config = this->buildOptionalKeyConfig();
    m_appConfigMgr                                  = this->launchAppConfigMgr("optionalKeyTest", config);

    // Service used to generate data
    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(::fwTools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    auto data1 = std::dynamic_pointer_cast< ::fwData::Object >(::fwTools::fwID::getObject("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test service with two optional attributes and available data at start
    // =================================================================================================================

    fwTools::Object::sptr gnsrv1 = ::fwTools::fwID::getObject("TestService1Uid");
    auto srv1                    = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv1);
    CPPUNIT_ASSERT(srv1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    // Check connection
    auto sig = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
    WAIT(srv1->getIsUpdated());

    CPPUNIT_ASSERT(srv1->getIsUpdated());
    srv1->resetIsUpdated();

    CPPUNIT_ASSERT(data1   == srv1->getInput< ::fwData::Object>("data1") );
    CPPUNIT_ASSERT(nullptr == srv1->getInput< ::fwData::Object>("data2") );
    CPPUNIT_ASSERT(nullptr == srv1->getInput< ::fwData::Object>("data3") );

    // Create data 2
    ::fwData::Boolean::sptr data2 = ::fwData::Boolean::New();

    ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
    WAIT(data2 == srv1->getInput< ::fwData::Object>("data2"));

    CPPUNIT_ASSERT(data2 == srv1->getInput< ::fwData::Object>("data2") );
    CPPUNIT_ASSERT(nullptr == srv1->getInput< ::fwData::Object>("data3") );

    CPPUNIT_ASSERT("data2" == srv1->getSwappedObjectKey() );
    CPPUNIT_ASSERT(data2 == srv1->getSwappedObject() );

    // Check no connection with data 2
    CPPUNIT_ASSERT(!srv1->getIsUpdated());
    auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig2->asyncEmit();
    WAIT(!srv1->getIsUpdated());
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    // Create data 3 and 4
    ::fwData::Boolean::sptr data3 = ::fwData::Boolean::New();
    ::fwData::Boolean::sptr data4 = ::fwData::Boolean::New();

    ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);
    ::fwServices::OSR::registerServiceOutput(data4, "out4", genDataSrv);

    WAIT(data3 == srv1->getInput< ::fwData::Object>("data3") &&
         data4 == srv1->getInput< ::fwData::Object>("data4"));

    CPPUNIT_ASSERT(data3 == srv1->getInput< ::fwData::Object>("data3") );
    CPPUNIT_ASSERT(data4 == srv1->getInput< ::fwData::Object>("data4") );
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());

    // Check connection with data 3
    srv1->resetIsUpdated();
    auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig3->asyncEmit();
    WAIT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    // Check connection with data 4
    srv1->resetIsUpdated();
    auto sig4 = data4->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig4->asyncEmit();
    WAIT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    // Remove data 2 and 3
    ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);
    WAIT(nullptr == srv1->getInput< ::fwData::Object>("data2"));

    CPPUNIT_ASSERT("data2" == srv1->getSwappedObjectKey() );
    CPPUNIT_ASSERT(nullptr == srv1->getSwappedObject() );
    CPPUNIT_ASSERT(nullptr == srv1->getInput< ::fwData::Object>("data2") );

    ::fwServices::OSR::unregisterServiceOutput("out3", genDataSrv);
    WAIT(nullptr == srv1->getInput< ::fwData::Object>("data3"));

    CPPUNIT_ASSERT("data3" == srv1->getSwappedObjectKey() );
    CPPUNIT_ASSERT(nullptr == srv1->getSwappedObject() );
    CPPUNIT_ASSERT(nullptr == srv1->getInput< ::fwData::Object>("data3") );

    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());

    // Create data 3
    ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);
    WAIT(data3 == srv1->getInput< ::fwData::Object>("data3"));

    CPPUNIT_ASSERT(nullptr == srv1->getInput< ::fwData::Object>("data2") );
    CPPUNIT_ASSERT(data3 == srv1->getInput< ::fwData::Object>("data3") );
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());

    // =================================================================================================================
    // Test service with two optional attributes and unavailable data at start
    // =================================================================================================================

    // Create data 5
    ::fwData::Boolean::sptr data5 = ::fwData::Boolean::New();
    {
        fwTools::Object::sptr gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
        CPPUNIT_ASSERT(gnsrv5 == nullptr);

        ::fwServices::OSR::registerServiceOutput(data5, "out5", genDataSrv);
        WAIT_SERVICE_STARTED("TestService5Uid");

        gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
        auto srv5 = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv5);
        CPPUNIT_ASSERT(srv5 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());
        CPPUNIT_ASSERT(!srv5->getIsUpdated());

        CPPUNIT_ASSERT(data5 == srv5->getInput< ::fwData::Object>("data5") );
        CPPUNIT_ASSERT(nullptr == srv5->getInput< ::fwData::Object>("data2") );
        CPPUNIT_ASSERT(data3 == srv5->getInput< ::fwData::Object>("data3") );
        CPPUNIT_ASSERT(data4 == srv5->getInput< ::fwData::Object>("data4") );

        // Check connection with data 4
        srv5->resetIsUpdated();
        auto sig4 = data4->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig4->asyncEmit();
        WAIT(srv5->getIsUpdated());

        // Remove data 3 and 4
        ::fwServices::OSR::unregisterServiceOutput("out3", genDataSrv);
        ::fwServices::OSR::unregisterServiceOutput("out4", genDataSrv);

        WAIT(nullptr == srv5->getInput< ::fwData::Object>("data3") &&
             nullptr == srv5->getInput< ::fwData::Object>("data4"));

        CPPUNIT_ASSERT(data5 == srv5->getInput< ::fwData::Object>("data5") );
        CPPUNIT_ASSERT(nullptr == srv5->getInput< ::fwData::Object>("data2") );
        CPPUNIT_ASSERT(nullptr == srv5->getInput< ::fwData::Object>("data3") );
        CPPUNIT_ASSERT(nullptr == srv5->getInput< ::fwData::Object>("data4") );

        // Create data 3
        ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);
        WAIT(nullptr != srv5->getInput< ::fwData::Object>("data3"));

        CPPUNIT_ASSERT(data5 == srv5->getInput< ::fwData::Object>("data5") );
        CPPUNIT_ASSERT(nullptr == srv5->getInput< ::fwData::Object>("data2") );
        CPPUNIT_ASSERT(data3 == srv5->getInput< ::fwData::Object>("data3") );
        CPPUNIT_ASSERT(nullptr == srv5->getInput< ::fwData::Object>("data4") );
    }

    // Remove data 5
    {
        ::fwServices::OSR::unregisterServiceOutput("out5", genDataSrv);

        WAIT(nullptr == ::fwTools::fwID::getObject("TestService5Uid"));

        fwTools::Object::sptr gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
        CPPUNIT_ASSERT(gnsrv5 == nullptr);
    }

    // Create data 5
    {
        ::fwServices::OSR::registerServiceOutput(data5, "out5", genDataSrv);
        WAIT_SERVICE_STARTED("TestService5Uid");

        auto gnsrv5 = ::fwTools::fwID::getObject("TestService5Uid");
        auto srv5   = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv5);
        CPPUNIT_ASSERT(srv5 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());
        CPPUNIT_ASSERT(!srv5->getIsUpdated());

        CPPUNIT_ASSERT(data5 == srv5->getInput< ::fwData::Object>("data5") );
        CPPUNIT_ASSERT(nullptr == srv5->getInput< ::fwData::Object>("data2") );
        CPPUNIT_ASSERT(data3 == srv5->getInput< ::fwData::Object>("data3") );
        CPPUNIT_ASSERT(nullptr == srv5->getInput< ::fwData::Object>("data4") );

        // Check connection with data 3
        srv5->resetIsUpdated();
        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig3->asyncEmit();
        WAIT(srv5->getIsUpdated());
        CPPUNIT_ASSERT(srv5->getIsUpdated());

        // Create data 2
        ::fwData::Boolean::sptr data2 = ::fwData::Boolean::New();

        ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
        WAIT(data2 == srv5->getInput< ::fwData::Object>("data2"));

        CPPUNIT_ASSERT(data5 == srv5->getInput< ::fwData::Object>("data5") );
        CPPUNIT_ASSERT(data2 == srv5->getInput< ::fwData::Object>("data2") );
        CPPUNIT_ASSERT(data3 == srv5->getInput< ::fwData::Object>("data3") );
        CPPUNIT_ASSERT(nullptr == srv5->getInput< ::fwData::Object>("data4") );

        // Check no connection with data 2
        srv5->resetIsUpdated();
        CPPUNIT_ASSERT(!srv5->getIsUpdated());
        auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig2->asyncEmit();
        WAIT(!srv5->getIsUpdated());
        CPPUNIT_ASSERT(!srv5->getIsUpdated());
    }
}

//------------------------------------------------------------------------------

void AppConfig2Test::keyGroupTest()
{
    ::fwRuntime::ConfigurationElement::csptr config = this->buildKeyGroupConfig();
    m_appConfigMgr                                  = this->launchAppConfigMgr("keyGroupTest", config);

    // Service used to generate data
    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(::fwTools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    auto data1 = std::dynamic_pointer_cast< ::fwData::Object >(::fwTools::fwID::getObject("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    auto data4 = std::dynamic_pointer_cast< ::fwData::Object >(::fwTools::fwID::getObject("data4Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    auto data5 = std::dynamic_pointer_cast< ::fwData::Object >(::fwTools::fwID::getObject("data5Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test service with one key group of two data
    // =================================================================================================================

    ::fwData::Image::sptr data3;
    {
        fwTools::Object::sptr gnsrv1 = ::fwTools::fwID::getObject("TestService1Uid");
        CPPUNIT_ASSERT(gnsrv1 == nullptr);

        // Create data 2
        ::fwData::Boolean::sptr data2 = ::fwData::Boolean::New();
        ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
        WAIT_SERVICE_STARTED("TestService1Uid");

        gnsrv1 = ::fwTools::fwID::getObject("TestService1Uid");
        auto srv1 = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());

        CPPUNIT_ASSERT(data1 == srv1->getInput< ::fwData::Object>("data1") );
        CPPUNIT_ASSERT(data2 == srv1->getInput< ::fwData::Object>("dataGroup#0") );
        CPPUNIT_ASSERT(nullptr == srv1->getInput< ::fwData::Object>("dataGroup#1") );

        CPPUNIT_ASSERT(data2 == srv1->getInput< ::fwData::Object>("dataGroup", 0 ) );
        CPPUNIT_ASSERT(nullptr == srv1->getInput< ::fwData::Object>("dataGroup", 1 ) );

        CPPUNIT_ASSERT(2 == srv1->getKeyGroupSize("dataGroup") );

        // Check connection with data 2
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig2->asyncEmit();
        WAIT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv1->getIsUpdated());

        // Create data 3
        data3 = ::fwData::Image::New();

        ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);

        WAIT(data3 == srv1->getInput< ::fwData::Object>("dataGroup2"));

        CPPUNIT_ASSERT(data2 == srv1->getInput< ::fwData::Object>("dataGroup#0") );
        CPPUNIT_ASSERT(data3 == srv1->getInput< ::fwData::Object>("dataGroup#1") );

        CPPUNIT_ASSERT(data2 == srv1->getInput< ::fwData::Object>("dataGroup", 0 ) );
        CPPUNIT_ASSERT(data3 == srv1->getInput< ::fwData::Object>("dataGroup", 1 ) );

        // Check connection with data 3
        srv1->resetIsUpdated();
        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig3->asyncEmit();
        WAIT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv1->getIsUpdated());
    }

    // Remove data 2
    {
        ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);

        WAIT(nullptr == ::fwTools::fwID::getObject("TestService1Uid"));

        fwTools::Object::sptr gnsrv5 = ::fwTools::fwID::getObject("TestService1Uid");
        CPPUNIT_ASSERT(gnsrv5 == nullptr);
    }

    {
        WAIT_SERVICE_STARTED("TestService2Uid");
        fwTools::Object::sptr gnsrv2 = ::fwTools::fwID::getObject("TestService2Uid");
        auto srv2                    = ::fwServices::ut::TestSrvAutoconnect::dynamicCast(gnsrv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        WAIT(data3 == srv2->getInput< ::fwData::Object>("dataGroup1#0"));

        CPPUNIT_ASSERT(2 == srv2->getKeyGroupSize("dataGroup0") );
        CPPUNIT_ASSERT(data1   == srv2->getInput< ::fwData::Object>("dataGroup0#0") );
        CPPUNIT_ASSERT(nullptr == srv2->getInput< ::fwData::Object>("dataGroup0#1") );

        CPPUNIT_ASSERT(data1   == srv2->getInput< ::fwData::Object>("dataGroup0", 0 ) );
        CPPUNIT_ASSERT(nullptr == srv2->getInput< ::fwData::Object>("dataGroup0", 1 ) );

        CPPUNIT_ASSERT(3 == srv2->getKeyGroupSize("dataGroup1") );
        CPPUNIT_ASSERT(data3 == srv2->getInput< ::fwData::Object>("dataGroup1#0") );
        CPPUNIT_ASSERT(data4 == srv2->getInput< ::fwData::Object>("dataGroup1#1") );
        CPPUNIT_ASSERT(data5 == srv2->getInput< ::fwData::Object>("dataGroup1#2") );

        CPPUNIT_ASSERT(data3 == srv2->getInput< ::fwData::Object>("dataGroup1", 0) );
        CPPUNIT_ASSERT(data4 == srv2->getInput< ::fwData::Object>("dataGroup1", 1) );
        CPPUNIT_ASSERT(data5 == srv2->getInput< ::fwData::Object>("dataGroup1", 2) );

        // Check connection with data 1
        srv2->resetIsUpdated();
        auto sig1 = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig1->asyncEmit();
        WAIT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());

        // Check no connection with data 3
        srv2->resetIsUpdated();
        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig3->asyncEmit();
        WAIT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        // Check connection with data 4
        srv2->resetIsUpdated();
        auto sig4 = data4->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig4->asyncEmit();
        WAIT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        auto sigIm4 = data4->signal< ::fwData::Image::BufferModifiedSignalType>(::fwData::Image::s_BUFFER_MODIFIED_SIG);
        sigIm4->asyncEmit();
        WAIT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());

        // Check no connection with data 5
        srv2->resetIsUpdated();
        auto sig5 = data5->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig5->asyncEmit();
        WAIT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        auto sigIm5 = data5->signal< ::fwData::Image::BufferModifiedSignalType>(::fwData::Image::s_BUFFER_MODIFIED_SIG);
        sigIm5->asyncEmit();
        WAIT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
    }

    return;
}

//-----------------------------------------------------------------------------

void AppConfig2Test::concurentAccessToAppConfig2Test()
{
    const unsigned int nbThreads = 20;
    std::vector< SPTR(::fwTest::helper::Thread) > threads;

    for (unsigned int i = 0; i<nbThreads; ++i)
    {
        SPTR(::fwTest::helper::Thread) thread;
        thread = std::shared_ptr< ::fwTest::helper::Thread >(
            new ::fwTest::helper::Thread(std::bind(&AppConfig2Test::parametersConfigTest, this)));
        threads.push_back(thread);
    }

    for (unsigned int i = 0; i<nbThreads; ++i)
    {
        std::stringstream str;
        str << "thread " << i;
        CPPUNIT_ASSERT_MESSAGE(str.str(), threads[i]->timedJoin(1000));
    }

    ::fwServices::registry::AppConfig2::getDefault()->clearRegistry();
    std::vector< std::string > allCconfigs = ::fwServices::registry::AppConfig2::getDefault()->getAllConfigs();
    CPPUNIT_ASSERT(allCconfigs.empty());

}

//------------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr AppConfig2Test::buildParametersConfig()
{
    // Configuration on fwTools::Object which uid is objectUUID
    std::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("object"));
    cfg->setAttributeValue( "uid", "${TEST_IMAGE}");
    cfg->setAttributeValue( "type", "::fwData::Image");

    // Service A
    std::shared_ptr< ::fwRuntime::EConfigurationElement > serviceA = cfg->addConfigurationElement("service");
    serviceA->setAttributeValue( "uid", "myTestService1" );
    serviceA->setAttributeValue("type", "::fwServices::ut::TestServiceImplementationImage" );
    serviceA->setAttributeValue( "autoConnect", "no" );

    // Service B
    std::shared_ptr< ::fwRuntime::EConfigurationElement > serviceB = cfg->addConfigurationElement("service");
    serviceB->setAttributeValue( "uid", "${UID_SERVICE2}" );
    serviceB->setAttributeValue("type", "::fwServices::ut::TestServiceImplementationImage" );
    serviceB->setAttributeValue( "autoConnect", "no" );

    // Connections
    std::shared_ptr< ::fwRuntime::EConfigurationElement > connect1 = cfg->addConfigurationElement("connect");
    connect1->setAttributeValue( "channel", "channel1" );
    connect1->addConfigurationElement("signal")->setValue( "${TEST_IMAGE}/modified" );
    connect1->addConfigurationElement("slot")->setValue( "myTestService1/update" );

    // Start method from object's services
    std::shared_ptr< ::fwRuntime::EConfigurationElement > startA = cfg->addConfigurationElement("start");
    startA->setAttributeValue( "uid", "myTestService1" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > startB = cfg->addConfigurationElement("start");
    startB->setAttributeValue( "uid", "myTestService2" );

    // Update method from object's services
    std::shared_ptr< ::fwRuntime::EConfigurationElement > updateA = cfg->addConfigurationElement("update");
    updateA->setAttributeValue( "uid", "myTestService1" );

    return cfg;
}

//------------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr AppConfig2Test::buildStartStopConfig()
{
    std::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("config"));

    std::shared_ptr< ::fwRuntime::EConfigurationElement > objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data1Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data2Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");
    objCfg->setAttributeValue( "src", "deferred");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data3Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data4Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");
    objCfg->setAttributeValue( "src", "deferred");

    // Service used to generate data
    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataSrv = cfg->addConfigurationElement("service");
    genDataSrv->setAttributeValue( "uid", "SGenerateData" );
    genDataSrv->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    genDataSrv->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService2 = genDataSrv->addConfigurationElement("out");
    genDataService2->setAttributeValue( "key", "out2" );
    genDataService2->setAttributeValue( "uid", "data2Id" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService4 = genDataSrv->addConfigurationElement("out");
    genDataService4->setAttributeValue( "key", "out4" );
    genDataService4->setAttributeValue( "uid", "data4Id" );

    // Service #1
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service1 = cfg->addConfigurationElement("service");
    service1->setAttributeValue( "uid", "TestService1Uid" );
    service1->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    service1->setAttributeValue( "autoConnect", "no" );

    // Service #2
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service2 = cfg->addConfigurationElement("service");
    service2->setAttributeValue( "uid", "TestService2Uid" );
    service2->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    service2->setAttributeValue( "autoConnect", "no" );

    // Service #3 with one data
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service3 = cfg->addConfigurationElement("service");
    service3->setAttributeValue( "uid", "TestService3Uid" );
    service3->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    service3->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService3 = service3->addConfigurationElement("in");
    dataService3->setAttributeValue( "key", "data" );
    dataService3->setAttributeValue( "uid", "data1Id" );

    // Service #4 with one deferred data
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service4 = cfg->addConfigurationElement("service");
    service4->setAttributeValue( "uid", "TestService4Uid" );
    service4->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    service4->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService4 = service4->addConfigurationElement("in");
    dataService4->setAttributeValue( "key", "data" );
    dataService4->setAttributeValue( "uid", "data2Id" );

    // Service #5 with two deferred data
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service5 = cfg->addConfigurationElement("service");
    service5->setAttributeValue( "uid", "TestService5Uid" );
    service5->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    service5->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService5_1 = service5->addConfigurationElement("in");
    dataService5_1->setAttributeValue( "key", "data1" );
    dataService5_1->setAttributeValue( "uid", "data1Id" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService5_2 = service5->addConfigurationElement("in");
    dataService5_2->setAttributeValue( "key", "data2" );
    dataService5_2->setAttributeValue( "uid", "data2Id" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService5_3 = service5->addConfigurationElement("in");
    dataService5_3->setAttributeValue( "key", "data3" );
    dataService5_3->setAttributeValue( "uid", "data3Id" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService5_4 = service5->addConfigurationElement("inout");
    dataService5_4->setAttributeValue( "key", "data4" );
    dataService5_4->setAttributeValue( "uid", "data4Id" );

    // Start method from object's services
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "SGenerateData" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService1Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService5Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService4Uid" );

    // Update method from object's services
    cfg->addConfigurationElement("update")->setAttributeValue( "uid", "TestService1Uid" );
    cfg->addConfigurationElement("update")->setAttributeValue( "uid", "TestService4Uid" );
    cfg->addConfigurationElement("update")->setAttributeValue( "uid", "TestService5Uid" );

    return cfg;
}

//------------------------------------------------------------------------------

fwRuntime::ConfigurationElement::sptr AppConfig2Test::buildAutoConnectTestConfig()
{
    // Configuration on fwTools::Object which uid is objectUUID
    std::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("config"));

    std::shared_ptr< ::fwRuntime::EConfigurationElement > objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data1Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data2Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data3Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");
    objCfg->setAttributeValue( "src", "deferred");

    // Service used to generate data
    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataSrv = cfg->addConfigurationElement("service");
    genDataSrv->setAttributeValue( "uid", "SGenerateData" );
    genDataSrv->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    genDataSrv->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService3 = genDataSrv->addConfigurationElement("out");
    genDataService3->setAttributeValue( "key", "out3" );
    genDataService3->setAttributeValue( "uid", "data3Id" );

    // Service #1 - "autoconnect" = "no"
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service1 = cfg->addConfigurationElement("service");
    service1->setAttributeValue( "uid", "TestService1Uid" );
    service1->setAttributeValue("type", "::fwServices::ut::TestSrvAutoconnect" );
    service1->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService1_1 = service1->addConfigurationElement("in");
    dataService1_1->setAttributeValue( "key", "data1" );
    dataService1_1->setAttributeValue( "uid", "data1Id" );

    // Service #2 - global "autoconnect" = "yes"
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service2 = cfg->addConfigurationElement("service");
    service2->setAttributeValue( "uid", "TestService2Uid" );
    service2->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    service2->setAttributeValue( "autoConnect", "yes" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService2_1 = service2->addConfigurationElement("in");
    dataService2_1->setAttributeValue( "key", "data1" );
    dataService2_1->setAttributeValue( "uid", "data1Id" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService2_2 = service2->addConfigurationElement("inout");
    dataService2_2->setAttributeValue( "key", "data2" );
    dataService2_2->setAttributeValue( "uid", "data2Id" );

    // Service #3 - local "autoconnect" = "yes"
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service3 = cfg->addConfigurationElement("service");
    service3->setAttributeValue( "uid", "TestService3Uid" );
    service3->setAttributeValue("type", "::fwServices::ut::TestSrvAutoconnect" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService3_1 = service3->addConfigurationElement("in");
    dataService3_1->setAttributeValue( "key", "data1" );
    dataService3_1->setAttributeValue( "uid", "data1Id" );
    dataService3_1->setAttributeValue( "autoConnect", "yes" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService3_2 = service3->addConfigurationElement("inout");
    dataService3_2->setAttributeValue( "key", "data2" );
    dataService3_2->setAttributeValue( "uid", "data2Id" );
    dataService3_2->setAttributeValue( "autoConnect", "yes" );

    // Service #4 - "autoconnect" = "no" with unavailable data at start
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service4 = cfg->addConfigurationElement("service");
    service4->setAttributeValue( "uid", "TestService4Uid" );
    service4->setAttributeValue("type", "::fwServices::ut::TestSrvAutoconnect" );
    service4->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService4_1 = service4->addConfigurationElement("in");
    dataService4_1->setAttributeValue( "key", "data3" );
    dataService4_1->setAttributeValue( "uid", "data3Id" );

    // Service #5 - "autoconnect" = "yes" with unavailable data at start
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service5 = cfg->addConfigurationElement("service");
    service5->setAttributeValue( "uid", "TestService5Uid" );
    service5->setAttributeValue("type", "::fwServices::ut::TestSrvAutoconnect" );
    service5->setAttributeValue( "autoConnect", "yes" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > itemService5_1 = service5->addConfigurationElement("in");
    itemService5_1->setAttributeValue( "key", "data1" );
    itemService5_1->setAttributeValue( "uid", "data1Id" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > itemService5_2 = service5->addConfigurationElement("in");
    itemService5_2->setAttributeValue( "key", "data3" );
    itemService5_2->setAttributeValue( "uid", "data3Id" );

    // Start method from object's services
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "SGenerateData" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService1Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService2Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService3Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService4Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService5Uid" );

    return cfg;
}

//------------------------------------------------------------------------------

fwRuntime::ConfigurationElement::sptr AppConfig2Test::buildConnectionConfig()
{
    std::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("config"));

    std::shared_ptr< ::fwRuntime::EConfigurationElement > objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data1Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data2Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");
    objCfg->setAttributeValue( "src", "deferred");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data3Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");
    objCfg->setAttributeValue( "src", "deferred");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data4Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    // Service used to generate data
    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataSrv = cfg->addConfigurationElement("service");
    genDataSrv->setAttributeValue( "uid", "SGenerateData" );
    genDataSrv->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    genDataSrv->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService2 = genDataSrv->addConfigurationElement("out");
    genDataService2->setAttributeValue( "key", "out2" );
    genDataService2->setAttributeValue( "uid", "data2Id" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService3 = genDataSrv->addConfigurationElement("out");
    genDataService3->setAttributeValue( "key", "out3" );
    genDataService3->setAttributeValue( "uid", "data3Id" );

    // Service #1 - we use don't use autoconnect and we connect manually at the end of the configuration
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service1 = cfg->addConfigurationElement("service");
    service1->setAttributeValue( "uid", "TestService1Uid" );
    service1->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    service1->setAttributeValue( "autoConnect", "no" );

    // Service #2 - we use don't use autoconnect and we connect manually at the end of the configuration
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service2 = cfg->addConfigurationElement("service");
    service2->setAttributeValue( "uid", "TestService2Uid" );
    service2->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    service2->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService2_1 = service2->addConfigurationElement("in");
    dataService2_1->setAttributeValue( "key", "data1" );
    dataService2_1->setAttributeValue( "uid", "data1Id" );

    // Service #3 - we use don't use autoconnect and we connect manually at the end of the configuration
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service3 = cfg->addConfigurationElement("service");
    service3->setAttributeValue( "uid", "TestService3Uid" );
    service3->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );
    service3->setAttributeValue( "autoConnect", "no" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService3_1 = service3->addConfigurationElement("in");
    dataService3_1->setAttributeValue( "key", "data1" );
    dataService3_1->setAttributeValue( "uid", "data1Id" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService3_2 = service3->addConfigurationElement("out");
    dataService3_2->setAttributeValue( "key", "data2" );
    dataService3_2->setAttributeValue( "uid", "data2Id" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService3_3 = service3->addConfigurationElement("inout");
    dataService3_3->setAttributeValue( "key", "data3" );
    dataService3_3->setAttributeValue( "uid", "data3Id" );

    // Service #4 - used to test "started" and "stopped" signals
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service4 = cfg->addConfigurationElement("service");
    service4->setAttributeValue( "uid", "TestService4Uid" );
    service4->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );

    // Connections
    std::shared_ptr< ::fwRuntime::EConfigurationElement > connect1 = cfg->addConfigurationElement("connect");
    connect1->addConfigurationElement("signal")->setValue( "data1Id/modified" );
    connect1->addConfigurationElement("slot")->setValue( "TestService1Uid/update" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > connect2 = cfg->addConfigurationElement("connect");
    connect2->setAttributeValue( "channel", "2" );
    connect2->addConfigurationElement("signal")->setValue( "data1Id/modified" );
    connect2->addConfigurationElement("slot")->setValue( "TestService2Uid/update" );
    connect2->addConfigurationElement("slot")->setValue( "TestService3Uid/update" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > connect3 = cfg->addConfigurationElement("connect");
    connect3->addConfigurationElement("signal")->setValue( "data2Id/modified" );
    connect3->addConfigurationElement("slot")->setValue( "TestService1Uid/update" );
    connect3->addConfigurationElement("slot")->setValue( "TestService3Uid/update" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > connect4 = cfg->addConfigurationElement("connect");
    connect4->setAttributeValue( "channel", "4" );
    connect4->addConfigurationElement("signal")->setValue( "data3Id/modified" );
    connect4->addConfigurationElement("signal")->setValue( "data4Id/modified" );
    connect4->addConfigurationElement("slot")->setValue( "TestService2Uid/update" );
    connect4->addConfigurationElement("slot")->setValue( "TestService3Uid/update" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > connect5 = cfg->addConfigurationElement("connect");
    connect5->addConfigurationElement("signal")->setValue( "TestService3Uid/started" );
    connect5->addConfigurationElement("slot")->setValue( "TestService4Uid/update" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > connect6 = cfg->addConfigurationElement("connect");
    connect6->addConfigurationElement("signal")->setValue( "TestService3Uid/stopped" );
    connect6->addConfigurationElement("slot")->setValue( "TestService4Uid/update2" );

    // Start method from object's services
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "SGenerateData" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService1Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService2Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService3Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService4Uid" );

    return cfg;
}

//------------------------------------------------------------------------------

fwRuntime::ConfigurationElement::sptr AppConfig2Test::buildOptionalKeyConfig()
{
    std::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("config"));

    std::shared_ptr< ::fwRuntime::EConfigurationElement > objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data1Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data2Id");
    objCfg->setAttributeValue( "type", "::fwData::Boolean");
    objCfg->setAttributeValue( "src", "deferred");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data3Id");
    objCfg->setAttributeValue( "type", "::fwData::Boolean");
    objCfg->setAttributeValue( "src", "deferred");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data4Id");
    objCfg->setAttributeValue( "type", "::fwData::Boolean");
    objCfg->setAttributeValue( "src", "deferred");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data5Id");
    objCfg->setAttributeValue( "type", "::fwData::Boolean");
    objCfg->setAttributeValue( "src", "deferred");

    // Service used to generate data
    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataSrv = cfg->addConfigurationElement("service");
    genDataSrv->setAttributeValue( "uid", "SGenerateData" );
    genDataSrv->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService2 = genDataSrv->addConfigurationElement("out");
    genDataService2->setAttributeValue( "key", "out2" );
    genDataService2->setAttributeValue( "uid", "data2Id" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService3 = genDataSrv->addConfigurationElement("out");
    genDataService3->setAttributeValue( "key", "out3" );
    genDataService3->setAttributeValue( "uid", "data3Id" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService4 = genDataSrv->addConfigurationElement("out");
    genDataService4->setAttributeValue( "key", "out4" );
    genDataService4->setAttributeValue( "uid", "data4Id" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService5 = genDataSrv->addConfigurationElement("out");
    genDataService5->setAttributeValue( "key", "out5" );
    genDataService5->setAttributeValue( "uid", "data5Id" );

    // Service #1
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service1 = cfg->addConfigurationElement("service");
    service1->setAttributeValue( "uid", "TestService1Uid" );
    service1->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService1_1 = service1->addConfigurationElement("in");
    dataService1_1->setAttributeValue( "key", "data1" );
    dataService1_1->setAttributeValue( "uid", "data1Id" );
    dataService1_1->setAttributeValue( "autoConnect", "yes" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService1_2 = service1->addConfigurationElement("in");
    dataService1_2->setAttributeValue( "key", "data2" );
    dataService1_2->setAttributeValue( "uid", "data2Id" );
    dataService1_2->setAttributeValue( "autoConnect", "no" );
    dataService1_2->setAttributeValue( "optional", "yes");
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService1_3 = service1->addConfigurationElement("in");
    dataService1_3->setAttributeValue( "key", "data3" );
    dataService1_3->setAttributeValue( "uid", "data3Id" );
    dataService1_3->setAttributeValue( "autoConnect", "no" );
    dataService1_3->setAttributeValue( "optional", "yes");
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService1_4 = service1->addConfigurationElement("in");
    dataService1_4->setAttributeValue( "key", "data4" );
    dataService1_4->setAttributeValue( "uid", "data4Id" );
    dataService1_4->setAttributeValue( "autoConnect", "yes" );
    dataService1_4->setAttributeValue( "optional", "yes");

    // Service #2
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service5 = cfg->addConfigurationElement("service");
    service5->setAttributeValue( "uid", "TestService5Uid" );
    service5->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService5_1 = service5->addConfigurationElement("in");
    dataService5_1->setAttributeValue( "key", "data5" );
    dataService5_1->setAttributeValue( "uid", "data5Id" );
    dataService5_1->setAttributeValue( "autoConnect", "yes" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService5_2 = service5->addConfigurationElement("in");
    dataService5_2->setAttributeValue( "key", "data2" );
    dataService5_2->setAttributeValue( "uid", "data2Id" );
    dataService5_2->setAttributeValue( "autoConnect", "no" );
    dataService5_2->setAttributeValue( "optional", "yes");
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService5_3 = service5->addConfigurationElement("in");
    dataService5_3->setAttributeValue( "key", "data3" );
    dataService5_3->setAttributeValue( "uid", "data3Id" );
    dataService5_3->setAttributeValue( "autoConnect", "no" );
    dataService5_3->setAttributeValue( "optional", "yes");
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService5_4 = service5->addConfigurationElement("in");
    dataService5_4->setAttributeValue( "key", "data4" );
    dataService5_4->setAttributeValue( "uid", "data4Id" );
    dataService5_4->setAttributeValue( "autoConnect", "yes" );
    dataService5_4->setAttributeValue( "optional", "yes");

    // Connections
    std::shared_ptr< ::fwRuntime::EConfigurationElement > connect1 = cfg->addConfigurationElement("connect");
    connect1->addConfigurationElement("signal")->setValue( "data3Id/modified" );
    connect1->addConfigurationElement("slot")->setValue( "TestService1Uid/update" );
    connect1->addConfigurationElement("slot")->setValue( "TestService5Uid/update" );

    // Start method from object's services
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "SGenerateData" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService1Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService5Uid" );

    return cfg;
}

//------------------------------------------------------------------------------

fwRuntime::ConfigurationElement::sptr AppConfig2Test::buildKeyGroupConfig()
{
    std::shared_ptr< ::fwRuntime::EConfigurationElement > cfg ( new ::fwRuntime::EConfigurationElement("config"));

    std::shared_ptr< ::fwRuntime::EConfigurationElement > objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data1Id");
    objCfg->setAttributeValue( "type", "::fwData::String");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data2Id");
    objCfg->setAttributeValue( "type", "::fwData::Boolean");
    objCfg->setAttributeValue( "src", "deferred");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data3Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");
    objCfg->setAttributeValue( "src", "deferred");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data4Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    objCfg = cfg->addConfigurationElement("object");
    objCfg->setAttributeValue( "uid", "data5Id");
    objCfg->setAttributeValue( "type", "::fwData::Image");

    // Service used to generate data
    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataSrv = cfg->addConfigurationElement("service");
    genDataSrv->setAttributeValue( "uid", "SGenerateData" );
    genDataSrv->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService2 = genDataSrv->addConfigurationElement("out");
    genDataService2->setAttributeValue( "key", "out2" );
    genDataService2->setAttributeValue( "uid", "data2Id" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > genDataService3 = genDataSrv->addConfigurationElement("out");
    genDataService3->setAttributeValue( "key", "out3" );
    genDataService3->setAttributeValue( "uid", "data3Id" );

    // Service #1
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service1 = cfg->addConfigurationElement("service");
    service1->setAttributeValue( "uid", "TestService1Uid" );
    service1->setAttributeValue("type", "::fwServices::ut::TestServiceImplementation" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService1_1 = service1->addConfigurationElement("in");
    dataService1_1->setAttributeValue( "key", "data1" );
    dataService1_1->setAttributeValue( "uid", "data1Id" );
    dataService1_1->setAttributeValue( "autoConnect", "yes" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataGroup1 = service1->addConfigurationElement("in");
    dataGroup1->setAttributeValue( "group", "dataGroup" );
    dataGroup1->setAttributeValue( "autoConnect", "yes" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService1_2 = dataGroup1->addConfigurationElement("key");
    dataService1_2->setAttributeValue( "uid", "data2Id" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService1_3 = dataGroup1->addConfigurationElement("key");
    dataService1_3->setAttributeValue( "uid", "data3Id" );
    dataService1_3->setAttributeValue( "optional", "yes" );

    // Service #2
    std::shared_ptr< ::fwRuntime::EConfigurationElement > service2 = cfg->addConfigurationElement("service");
    service2->setAttributeValue( "uid", "TestService2Uid" );
    service2->setAttributeValue("type", "::fwServices::ut::TestSrvAutoconnect" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataGroup2 = service2->addConfigurationElement("in");
    dataGroup2->setAttributeValue( "group", "dataGroup0" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService2_1 = dataGroup2->addConfigurationElement("key");
    dataService2_1->setAttributeValue( "uid", "data1Id" );
    dataService2_1->setAttributeValue( "autoConnect", "no" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService2_2 = dataGroup2->addConfigurationElement("key");
    dataService2_2->setAttributeValue( "uid", "data2Id" );
    dataService2_2->setAttributeValue( "autoConnect", "yes" );
    dataService2_2->setAttributeValue( "optional", "yes" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataGroup3 = service2->addConfigurationElement("in");
    dataGroup3->setAttributeValue( "group", "dataGroup1" );

    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService2_3 = dataGroup3->addConfigurationElement("key");
    dataService2_3->setAttributeValue( "uid", "data3Id" );
    dataService2_3->setAttributeValue( "autoConnect", "no" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService2_4 = dataGroup3->addConfigurationElement("key");
    dataService2_4->setAttributeValue( "uid", "data4Id" );
    dataService2_4->setAttributeValue( "autoConnect", "yes" );
    std::shared_ptr< ::fwRuntime::EConfigurationElement > dataService2_5 = dataGroup3->addConfigurationElement("key");
    dataService2_5->setAttributeValue( "uid", "data5Id" );
    dataService2_5->setAttributeValue( "autoConnect", "no" );

    // Connections
    std::shared_ptr< ::fwRuntime::EConfigurationElement > connect1 = cfg->addConfigurationElement("connect");
    connect1->addConfigurationElement("signal")->setValue( "data1Id/modified" );
    connect1->addConfigurationElement("slot")->setValue( "TestService2Uid/update" );

    // Start method from object's services
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "SGenerateData" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService1Uid" );
    cfg->addConfigurationElement("start")->setAttributeValue( "uid", "TestService2Uid" );

    return cfg;
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace fwServices
