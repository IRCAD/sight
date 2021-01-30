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

#include "AppConfigTest.hpp"

#include "fwServices/registry/ActiveWorkers.hpp"
#include "fwServices/registry/AppConfig.hpp"
#include "fwServices/registry/ObjectService.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/runtime/Convert.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>
#include <core/runtime/Runtime.hpp>
#include <core/TimeStamp.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>

#include <utest/wait.hpp>

#include <TestService.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::AppConfigTest );

namespace fwServices
{
namespace ut
{

#define WAIT_SERVICE_STARTED(srv)  \
    fwTestWaitMacro(core::tools::fwID::getObject(srv) != nullptr && \
                    ::fwServices::IService::dynamicCast(core::tools::fwID::getObject(srv))->getStatus() \
                    == ::fwServices::IService::STARTED)

//------------------------------------------------------------------------------

void AppConfigTest::setUp()
{
    // Set up context before running a test.

    ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    // Set up context before running a test.
    //modules location
    core::runtime::init();
    core::runtime::Runtime* runtime = core::runtime::Runtime::getDefault();

    std::filesystem::path location = core::runtime::getResourceFilePath("tu_exec_fwServices-0.0");
    CPPUNIT_ASSERT(std::filesystem::exists(location));

    runtime->addModules(location);
    core::runtime::loadModule("servicesReg");
    core::runtime::loadModule("AppConfigTest");

    ::fwServices::registry::AppConfig::sptr appConfig = ::fwServices::registry::AppConfig::getDefault();
    appConfig->parseBundleInformation();
}

//------------------------------------------------------------------------------

void AppConfigTest::tearDown()
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

    // Clean up after the test run.
    ::fwServices::registry::AppConfig::sptr appConfig;
    appConfig = ::fwServices::registry::AppConfig::getDefault();
    appConfig->clearRegistry();

    ::fwServices::registry::ActiveWorkers::sptr activeWorkers = ::fwServices::registry::ActiveWorkers::getDefault();
    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

void AppConfigTest::addConfigTest()
{
    ::fwServices::registry::AppConfig::sptr currentAppConfig = ::fwServices::registry::AppConfig::getDefault();

    const std::string configId(::fwServices::registry::AppConfig::getUniqueIdentifier());
    const std::string group("TestGroup");
    const std::string desc("Description");
    const std::string moduleId("servicesReg");
    const std::string moduleVersion("0.1");
    ::fwServices::registry::AppInfo::ParametersType parameters;

    core::runtime::ConfigurationElement::csptr config = this->buildConfig();

    currentAppConfig->addAppInfo(configId, group, desc, parameters, config, moduleId, moduleVersion);

    std::vector< std::string > allCconfigs = currentAppConfig->getAllConfigs();
    CPPUNIT_ASSERT_EQUAL(false, allCconfigs.empty());

    std::vector< std::string > configs = currentAppConfig->getConfigsFromGroup(group);
    CPPUNIT_ASSERT_EQUAL(size_t(1), configs.size());
    CPPUNIT_ASSERT_EQUAL(configId, configs.front());

    auto module = currentAppConfig->getModule(configId);
    CPPUNIT_ASSERT(module );
    CPPUNIT_ASSERT_EQUAL(moduleId, module->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(moduleVersion, module->getVersion().string());

    ::fwServices::registry::FieldAdaptorType replaceFields;

    core::runtime::ConfigurationElement::csptr configEltAdaptedConst;
    configEltAdaptedConst = currentAppConfig->getAdaptedTemplateConfig(configId, replaceFields, false);

    core::runtime::ConfigurationElement::sptr configEltAdapted;
    configEltAdapted = core::runtime::ConfigurationElement::constCast(configEltAdaptedConst);

    std::vector< core::runtime::ConfigurationElement::sptr >  objCfg = configEltAdapted->find("object");

    const std::string uid = objCfg.at(0)->getAttributeValue("uid");
    CPPUNIT_ASSERT_EQUAL(std::string("image"), uid );

    std::vector< core::runtime::ConfigurationElement::sptr >  servicesCfg = configEltAdapted->find("service");

    const std::string serviceUid1 = servicesCfg.at(0)->getAttributeValue("uid");
    CPPUNIT_ASSERT_EQUAL( std::string("myTestService1"), serviceUid1);
}

//------------------------------------------------------------------------------

void AppConfigTest::parametersConfigTest()
{
    ::fwServices::registry::AppConfig::sptr currentAppConfig = ::fwServices::registry::AppConfig::getDefault();

    const std::string configId("parametersConfigTest1");
    const std::string group("parametersGroup");

    ::fwServices::registry::FieldAdaptorType replaceFields;
    replaceFields["TEST_IMAGE"] = "objectUUID";

    std::vector< std::string > allCconfigs = currentAppConfig->getAllConfigs();
    auto it                                = std::find(allCconfigs.begin(), allCconfigs.end(), configId);
    CPPUNIT_ASSERT(it != allCconfigs.end());

    core::runtime::ConfigurationElement::csptr configEltAdaptedConst;
    configEltAdaptedConst = currentAppConfig->getAdaptedTemplateConfig(configId, replaceFields, false);

    core::runtime::ConfigurationElement::sptr configEltAdapted;
    configEltAdapted = core::runtime::ConfigurationElement::constCast(configEltAdaptedConst);

    std::vector< core::runtime::ConfigurationElement::sptr >  objCfg = configEltAdapted->find("object");

    const std::string uid = objCfg.at(0)->getAttributeValue("uid");
    CPPUNIT_ASSERT_EQUAL(std::string("objectUUID"), uid );

    std::vector< core::runtime::ConfigurationElement::sptr >  servicesCfg = configEltAdapted->find("service");

    const std::string serviceUid1 = servicesCfg.at(0)->getAttributeValue("uid");
    CPPUNIT_ASSERT_EQUAL( std::string("myTestService1"), serviceUid1);

    const std::string serviceUid2 = servicesCfg.at(1)->getAttributeValue("uid");
    CPPUNIT_ASSERT_EQUAL( std::string("myTestService2"), serviceUid2);
}

//------------------------------------------------------------------------------

::fwServices::AppConfigManager::sptr AppConfigTest::launchAppConfigMgr(
    const std::string& name,
    bool autoPrefix)
{
    auto appConfigMgr = ::fwServices::AppConfigManager::New();
    appConfigMgr->setIsUnitTest(!autoPrefix);

    const ::fwServices::registry::FieldAdaptorType fields;
    appConfigMgr->setConfig( name, fields );
    appConfigMgr->launch();

    return appConfigMgr;
}

//------------------------------------------------------------------------------

void AppConfigTest::startStopTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("startStopTest");

    // =================================================================================================================
    // Test manual start and stop of services, with or without data
    // =================================================================================================================

    auto data1 = std::dynamic_pointer_cast< ::fwData::Object >(core::tools::fwID::getObject("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // This service doesn't exist in the config
    CPPUNIT_ASSERT(core::tools::fwID::getObject("TestService142Uid") == nullptr);

    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(core::tools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    // This service has no data and is started by the config
    {
        core::tools::Object::sptr gnsrv1 = core::tools::fwID::getObject("TestService1Uid");
        auto srv1                        = ::fwServices::ut::TestService::dynamicCast(gnsrv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
    }

    // This service has no data and is NOT started by the config
    {
        core::tools::Object::sptr gnsrv2 = core::tools::fwID::getObject("TestService2Uid");
        auto srv2                        = ::fwServices::IService::dynamicCast(gnsrv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
        srv2->stop().wait();
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STOPPED, srv2->getStatus());
        srv2->start().wait();
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
    }

    // This service has a data and is NOT started by the config
    {
        core::tools::Object::sptr gnsrv3 = core::tools::fwID::getObject("TestService3Uid");
        auto srv3                        = ::fwServices::IService::dynamicCast(gnsrv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv3->getStatus());
    }

    // This service has a data that is not present yet (WID), so it is even not created
    {
        core::tools::Object::sptr gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
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
            auto gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
            auto srv4   = ::fwServices::IService::dynamicCast(gnsrv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv4->getStatus());
        }

        // Remove the data
        ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);
        fwTestWaitMacro(core::tools::fwID::exist("TestService4Uid") == false);

        // Now the service should have been stopped and destroyed automatically
        {
            auto gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
            CPPUNIT_ASSERT(gnsrv4 == nullptr);
        }

        // Register the data once again
        ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
        WAIT_SERVICE_STARTED("TestService4Uid");

        // Check again that the service was started automatically
        {
            auto gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
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
            core::tools::Object::sptr gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            CPPUNIT_ASSERT(gnsrv5 == nullptr);
        }

        // Create the remaining data
        ::fwData::Boolean::sptr data4 = ::fwData::Boolean::New();

        ::fwServices::OSR::registerServiceOutput(data4, "out4", genDataSrv);
        WAIT_SERVICE_STARTED("TestService5Uid");

        // Now the service should have been started automatically
        {
            auto gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            auto srv5   = ::fwServices::IService::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());
        }

        // Remove one data
        ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);

        // Now the service should have been stopped and destroyed automatically
        {
            fwTestWaitMacro(core::tools::fwID::exist("TestService5Uid") == false);
            auto gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            CPPUNIT_ASSERT(gnsrv5 == nullptr);
            // Test as well service 4, just to be sure
            fwTestWaitMacro(core::tools::fwID::exist("TestService4Uid") == false);
            auto gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
            CPPUNIT_ASSERT(gnsrv4 == nullptr);
        }

        // Put everything back
        ::fwServices::ut::TestService::s_START_COUNTER  = 0;
        ::fwServices::ut::TestService::s_UPDATE_COUNTER = 0;
        ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
        WAIT_SERVICE_STARTED("TestService5Uid");

        // Now the service should have been started automatically, check start order as well
        {
            auto gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            auto srv5   = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());

            // We have started, yet we may not have been updated, so wait for it just in case
            fwTestWaitMacro(1u == srv5->getUpdateOrder());
            CPPUNIT_ASSERT_EQUAL(0u, srv5->getStartOrder());
            CPPUNIT_ASSERT_EQUAL(1u, srv5->getUpdateOrder());

            // Test as well service 4, just to be sure
            auto gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
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
            auto gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            auto srv5   = ::fwServices::IService::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());
        }

        // Swap the data
        ::fwData::Boolean::sptr data5 = ::fwData::Boolean::New();

        ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);
        fwTestWaitMacro(core::tools::fwID::exist("TestService5Uid") == false);
        ::fwServices::OSR::registerServiceOutput(data5, "out2", genDataSrv);
        WAIT_SERVICE_STARTED("TestService5Uid");

        {
            auto gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            auto srv5   = ::fwServices::IService::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());
        }
    }

    m_appConfigMgr->stopAndDestroy();

    m_appConfigMgr = nullptr;
}

//------------------------------------------------------------------------------

void AppConfigTest::autoConnectTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("autoConnectTest");

    // =================================================================================================================
    // Test autoconnect with available data
    // =================================================================================================================

    auto data1 = ::fwData::Object::dynamicCast(core::tools::fwID::getObject("data1Id"));
    auto data2 = ::fwData::Object::dynamicCast(core::tools::fwID::getObject("data2Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    {
        core::tools::Object::sptr gnsrv1 = core::tools::fwID::getObject("TestService1Uid");
        auto srv1                        = ::fwServices::ut::TestService::dynamicCast(gnsrv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());

        core::tools::Object::sptr gnsrv2 = core::tools::fwID::getObject("TestService2Uid");
        auto srv2                        = ::fwServices::ut::TestService::dynamicCast(gnsrv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        core::tools::Object::sptr gnsrv3 = core::tools::fwID::getObject("TestService3Uid");
        auto srv3                        = ::fwServices::ut::TestSrvAutoconnect::dynamicCast(gnsrv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv3->getStatus());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getReceived());

        auto sig1 = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig1->asyncEmit();

        fwTestWaitMacro(srv2->getIsUpdated() && srv3->getIsUpdated());
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
        fwTestWaitMacro(srv2->getIsUpdated() && srv3->getReceived());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getReceived());
    }

    // =================================================================================================================
    // Test autoconnect with unavailable data at start
    // =================================================================================================================

    // Service used to generate data
    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(core::tools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);
    {
        // Check that dependent services are not created
        {
            core::tools::Object::sptr gnsrv3 = core::tools::fwID::getObject("TestService4Uid");
            CPPUNIT_ASSERT(gnsrv3 == nullptr);
            core::tools::Object::sptr gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            CPPUNIT_ASSERT(gnsrv5 == nullptr);
        }

        // Create the data
        ::fwData::Boolean::sptr data3 = ::fwData::Boolean::New();

        ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);

        WAIT_SERVICE_STARTED("TestService4Uid");
        WAIT_SERVICE_STARTED("TestService5Uid");
        {
            core::tools::Object::sptr gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
            auto srv4                        = ::fwServices::ut::TestService::dynamicCast(gnsrv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv4->getStatus());
            CPPUNIT_ASSERT(!srv4->getIsUpdated());

            core::tools::Object::sptr gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            auto srv5                        = ::fwServices::ut::TestSrvAutoconnect::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getReceived());

            auto sig = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
            fwTestWaitMacro(srv5->getReceived());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(srv5->getReceived());
        }

        // Remove one data
        ::fwServices::OSR::unregisterServiceOutput("out3", genDataSrv);
        fwTestWaitMacro(core::tools::fwID::exist("TestService4Uid") == false &&
                        core::tools::fwID::exist("TestService5Uid") == false);
        {
            core::tools::Object::sptr gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
            CPPUNIT_ASSERT(gnsrv4 == nullptr);
            core::tools::Object::sptr gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            CPPUNIT_ASSERT(gnsrv5 == nullptr);
        }

        // Emit, that should be ok
        ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);

        WAIT_SERVICE_STARTED("TestService4Uid");
        WAIT_SERVICE_STARTED("TestService5Uid");
        {
            core::tools::Object::sptr gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
            auto srv4                        = ::fwServices::ut::TestService::dynamicCast(gnsrv4);
            CPPUNIT_ASSERT(srv4 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv4->getStatus());
            CPPUNIT_ASSERT(!srv4->getIsUpdated());

            core::tools::Object::sptr gnsrv5 = core::tools::fwID::getObject("TestService5Uid");
            auto srv5                        = ::fwServices::ut::TestSrvAutoconnect::dynamicCast(gnsrv5);
            CPPUNIT_ASSERT(srv5 != nullptr);
            CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv5->getStatus());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getReceived());

            auto sig = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
            fwTestWaitMacro(srv5->getReceived());
            CPPUNIT_ASSERT(srv5->getReceived());

            CPPUNIT_ASSERT(!srv4->getIsUpdated());
            CPPUNIT_ASSERT(!srv5->getIsUpdated());
            CPPUNIT_ASSERT(srv5->getReceived());

            auto sig1 = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
            sig1->asyncEmit();
            fwTestWaitMacro(srv5->getIsUpdated());

            CPPUNIT_ASSERT(srv5->getIsUpdated());
        }
    }
}

//------------------------------------------------------------------------------

void AppConfigTest::connectionTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("connectionTest");

    // =================================================================================================================
    // Test connection without data
    // =================================================================================================================

    ::fwData::Composite::sptr composite;

    auto data1 = std::dynamic_pointer_cast< ::fwData::Object >(core::tools::fwID::getObject("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test connection without data or with available data
    // =================================================================================================================

    core::tools::Object::sptr gnsrv1 = core::tools::fwID::getObject("TestService1Uid");
    auto srv1                        = ::fwServices::ut::TestService::dynamicCast(gnsrv1);
    CPPUNIT_ASSERT(srv1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    core::tools::Object::sptr gnsrv2 = core::tools::fwID::getObject("TestService2Uid");
    auto srv2                        = ::fwServices::ut::TestService::dynamicCast(gnsrv2);
    CPPUNIT_ASSERT(srv2 != nullptr);
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
    CPPUNIT_ASSERT(!srv2->getIsUpdated());

    core::tools::Object::sptr gnsrv4 = core::tools::fwID::getObject("TestService4Uid");
    auto srv4                        = ::fwServices::ut::TestService::dynamicCast(gnsrv4);
    CPPUNIT_ASSERT(srv4 != nullptr);
    fwTestWaitMacro(::fwServices::IService::STARTED == srv4->getStatus());
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv4->getStatus());

    // Check connection
    auto sig = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
    fwTestWaitMacro(srv1->getIsUpdated() && srv2->getIsUpdated());

    CPPUNIT_ASSERT(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv2->getIsUpdated());

    // Service used to generate data
    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(core::tools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    // =================================================================================================================
    // Test connection with unavailable data at start
    // =================================================================================================================

    {
        core::tools::Object::sptr gnsrv3 = core::tools::fwID::getObject("TestService3Uid");
        CPPUNIT_ASSERT(gnsrv3 == nullptr);
    }

    // Emit a signal just for fun, anyway the service doesn't exist
    sig = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig->emit();

    srv2->resetIsUpdated();
    CPPUNIT_ASSERT(!srv2->getIsUpdated());

    // Check connection data4 -> srv2
    auto data4 = std::dynamic_pointer_cast< ::fwData::Object >(core::tools::fwID::getObject("data4Id"));
    CPPUNIT_ASSERT(data4 != nullptr);
    auto sig4 = data4->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig4->asyncEmit();
    fwTestWaitMacro(srv2->getIsUpdated());

    CPPUNIT_ASSERT(srv2->getIsUpdated());

    fwTestWaitMacro(!srv4->getIsUpdated());
    CPPUNIT_ASSERT(!srv4->getIsUpdated());
    CPPUNIT_ASSERT(!srv4->getIsUpdated2());

    // Create the missing data
    ::fwData::Boolean::sptr data2 = ::fwData::Boolean::New();
    ::fwData::Boolean::sptr data3 = ::fwData::Boolean::New();

    ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
    ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);
    WAIT_SERVICE_STARTED("TestService3Uid");
    {
        core::tools::Object::sptr gnsrv3 = core::tools::fwID::getObject("TestService3Uid");
        auto srv3                        = ::fwServices::ut::TestService::dynamicCast(gnsrv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        fwTestWaitMacro(::fwServices::IService::STARTED == srv3->getStatus());
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv3->getStatus());
        srv2->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        // Check "started" signal
        fwTestWaitMacro(srv4->getIsUpdated())
        CPPUNIT_ASSERT(srv4->getIsUpdated());
        CPPUNIT_ASSERT(!srv4->getIsUpdated2());

        sig->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        srv1->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig2->asyncEmit();
        fwTestWaitMacro(srv1->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data4 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        sig4->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data3 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig3->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());
    }

    // Remove one data
    ::fwServices::OSR::unregisterServiceOutput("out3", genDataSrv);
    fwTestWaitMacro(core::tools::fwID::exist("TestService3Uid") == false);

    // Service 3 should be removed
    {
        core::tools::Object::sptr gnsrv3 = core::tools::fwID::getObject("TestService3Uid");
        CPPUNIT_ASSERT(gnsrv3 == nullptr);
    }

    // Check that connection is still there for the working services
    srv2->resetIsUpdated();
    CPPUNIT_ASSERT(!srv2->getIsUpdated());
    sig->asyncEmit();
    fwTestWaitMacro(srv2->getIsUpdated());
    CPPUNIT_ASSERT(srv2->getIsUpdated());

    srv1->resetIsUpdated();
    CPPUNIT_ASSERT(!srv1->getIsUpdated());
    auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig2->asyncEmit();
    fwTestWaitMacro(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    // Check connection data4 -> srv2
    srv2->resetIsUpdated();
    CPPUNIT_ASSERT(!srv2->getIsUpdated());

    sig4->asyncEmit();
    fwTestWaitMacro(srv2->getIsUpdated());

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
        core::tools::Object::sptr gnsrv3 = core::tools::fwID::getObject("TestService3Uid");
        auto srv3                        = ::fwServices::ut::TestService::dynamicCast(gnsrv3);
        CPPUNIT_ASSERT(srv3 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv3->getStatus());
        srv2->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        fwTestWaitMacro(srv4->getIsUpdated())
        CPPUNIT_ASSERT(srv4->getIsUpdated());
        CPPUNIT_ASSERT(!srv4->getIsUpdated2());

        sig->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        srv1->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto modifiedSig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        modifiedSig2->asyncEmit();
        fwTestWaitMacro(srv1->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data4 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        sig4->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());

        // Check connection data3 -> srv2,srv3
        srv2->resetIsUpdated();
        srv3->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv3->getIsUpdated());

        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig3->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated() && srv3->getIsUpdated());

        CPPUNIT_ASSERT(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv3->getIsUpdated());
    }

}

//------------------------------------------------------------------------------

void AppConfigTest::optionalKeyTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("optionalKeyTest");

    // Service used to generate data
    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(core::tools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    auto data1 = std::dynamic_pointer_cast< ::fwData::Object >(core::tools::fwID::getObject("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test service with two optional attributes and available data at start
    // =================================================================================================================

    core::tools::Object::sptr gnsrv1 = core::tools::fwID::getObject("TestService1Uid");
    auto srv1                        = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv1);
    CPPUNIT_ASSERT(srv1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    // Check connection
    auto sig = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
    fwTestWaitMacro(srv1->getIsUpdated());

    CPPUNIT_ASSERT(srv1->getIsUpdated());
    srv1->resetIsUpdated();

    CPPUNIT_ASSERT(data1 == srv1->getLockedInput< ::fwData::Object>("data1").get_shared() );
    CPPUNIT_ASSERT(srv1->getWeakInput< ::fwData::Object>("data2").expired());
    CPPUNIT_ASSERT(srv1->getWeakInput< ::fwData::Object>("data3").expired() );

    // Check that null data object throws an exception
    CPPUNIT_ASSERT_THROW(srv1->getLockedInput< ::fwData::Object>("data2"), ::fwData::Exception);

    // Create data 2
    ::fwData::Boolean::sptr data2 = ::fwData::Boolean::New();

    ::fwServices::OSR::registerServiceOutput(data2, "out2", genDataSrv);
    fwTestWaitMacro(!srv1->getWeakInput< ::fwData::Object>("data2").expired() &&
                    data2 == srv1->getLockedInput< ::fwData::Object>("data2").get_shared());

    CPPUNIT_ASSERT(data2 == srv1->getLockedInput< ::fwData::Object>("data2").get_shared() );
    CPPUNIT_ASSERT(srv1->getWeakInput< ::fwData::Object>("data3").expired() );

    fwTestWaitMacro("data2" == srv1->getSwappedObjectKey());
    CPPUNIT_ASSERT_EQUAL(std::string("data2"), srv1->getSwappedObjectKey() );
    CPPUNIT_ASSERT(data2 == srv1->getSwappedObject() );

    // Check no connection with data 2
    CPPUNIT_ASSERT(!srv1->getIsUpdated());
    auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig2->asyncEmit();
    fwTestWaitMacro(!srv1->getIsUpdated());
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    // Create data 3 and 4
    ::fwData::Boolean::sptr data3 = ::fwData::Boolean::New();
    ::fwData::Boolean::sptr data4 = ::fwData::Boolean::New();

    ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);
    ::fwServices::OSR::registerServiceOutput(data4, "out4", genDataSrv);

    fwTestWaitMacro(!srv1->getWeakInput< ::fwData::Object>("data3").expired() &&
                    !srv1->getWeakInput< ::fwData::Object>("data4").expired() &&
                    data3 == srv1->getLockedInput< ::fwData::Object>("data3").get_shared() &&
                    data4 == srv1->getLockedInput< ::fwData::Object>("data4").get_shared());

    CPPUNIT_ASSERT(data3 == srv1->getLockedInput< ::fwData::Object>("data3").get_shared() );
    CPPUNIT_ASSERT(data4 == srv1->getLockedInput< ::fwData::Object>("data4").get_shared() );
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());

    // Check connection with data 3
    srv1->resetIsUpdated();
    auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig3->asyncEmit();
    fwTestWaitMacro(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    // Check connection with data 4
    srv1->resetIsUpdated();
    auto sig4 = data4->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig4->asyncEmit();
    fwTestWaitMacro(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    // Remove data 2 and 3
    ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);
    fwTestWaitMacro(srv1->getWeakInput< ::fwData::Object>("data2").expired());

    fwTestWaitMacro("data2" == srv1->getSwappedObjectKey());
    CPPUNIT_ASSERT_EQUAL(std::string("data2"), srv1->getSwappedObjectKey() );
    CPPUNIT_ASSERT(nullptr == srv1->getSwappedObject() );
    CPPUNIT_ASSERT(srv1->getWeakInput< ::fwData::Object>("data2").expired());

    ::fwServices::OSR::unregisterServiceOutput("out3", genDataSrv);
    fwTestWaitMacro(!srv1->getWeakInput< ::fwData::Object>(
                        "data3").expired() && nullptr == srv1->getLockedInput< ::fwData::Object>("data3").get_shared());

    fwTestWaitMacro("data3" == srv1->getSwappedObjectKey());
    CPPUNIT_ASSERT_EQUAL(std::string("data3"), srv1->getSwappedObjectKey() );
    CPPUNIT_ASSERT(nullptr == srv1->getSwappedObject() );
    CPPUNIT_ASSERT(srv1->getWeakInput< ::fwData::Object>("data3").expired());

    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());

    // Create data 3
    ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);
    fwTestWaitMacro(!srv1->getWeakInput< ::fwData::Object>(
                        "data3").expired() && data3 == srv1->getLockedInput< ::fwData::Object>("data3").get_shared());

    CPPUNIT_ASSERT(srv1->getWeakInput< ::fwData::Object>("data2").expired());
    CPPUNIT_ASSERT(data3 == srv1->getLockedInput< ::fwData::Object>("data3").get_shared());
    CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());

    // =================================================================================================================
    // Test service with two optional attributes and unavailable data at start
    // =================================================================================================================

    // Create data 5
    ::fwData::Boolean::sptr data5 = ::fwData::Boolean::New();
    {
        core::tools::Object::sptr gnsrv2 = core::tools::fwID::getObject("TestService2Uid");
        CPPUNIT_ASSERT(gnsrv2 == nullptr);

        ::fwServices::OSR::registerServiceOutput(data5, "out5", genDataSrv);
        WAIT_SERVICE_STARTED("TestService2Uid");

        gnsrv2 = core::tools::fwID::getObject("TestService2Uid");
        CPPUNIT_ASSERT(gnsrv2 != nullptr);
        auto srv2 = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        CPPUNIT_ASSERT(data5 == srv2->getLockedInput< ::fwData::Object>("data5").get_shared());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("data2").expired());
        CPPUNIT_ASSERT(data3 == srv2->getLockedInput< ::fwData::Object>("data3").get_shared());
        CPPUNIT_ASSERT(data4 == srv2->getLockedInput< ::fwData::Object>("data4").get_shared());

        // Check connection with data 4
        srv2->resetIsUpdated();
        auto modifiedSig4 = data4->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        modifiedSig4->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated());

        // Remove data 3 and 4
        ::fwServices::OSR::unregisterServiceOutput("out3", genDataSrv);
        ::fwServices::OSR::unregisterServiceOutput("out4", genDataSrv);

        fwTestWaitMacro(!srv2->getWeakInput< ::fwData::Object>("data3").expired()&&
                        nullptr == srv2->getInput< ::fwData::Object>("data4"));

        CPPUNIT_ASSERT(data5 == srv2->getLockedInput< ::fwData::Object>("data5").get_shared());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("data2").expired());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("data3").expired());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("data4").expired());

        // Create data 3
        ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);
        fwTestWaitMacro(nullptr != srv2->getInput< ::fwData::Object>("data3"));

        CPPUNIT_ASSERT(data5 == srv2->getLockedInput< ::fwData::Object>("data5").get_shared());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("data2").expired());
        CPPUNIT_ASSERT(data3 == srv2->getLockedInput< ::fwData::Object>("data3").get_shared());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("data4").expired());
    }

    // Remove data 5
    {
        ::fwServices::OSR::unregisterServiceOutput("out5", genDataSrv);

        fwTestWaitMacro(false == core::tools::fwID::exist("TestService2Uid"));

        core::tools::Object::sptr gnsrv5 = core::tools::fwID::getObject("TestService2Uid");
        CPPUNIT_ASSERT(gnsrv5 == nullptr);
    }

    {
        // Create data 5
        ::fwServices::OSR::registerServiceOutput(data5, "out5", genDataSrv);
        WAIT_SERVICE_STARTED("TestService2Uid");

        auto gnsrv2 = core::tools::fwID::getObject("TestService2Uid");
        auto srv2   = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        CPPUNIT_ASSERT(data5 == srv2->getLockedInput< ::fwData::Object>("data5").get_shared());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("data2").expired());
        CPPUNIT_ASSERT(data3 == srv2->getLockedInput< ::fwData::Object>("data3").get_shared());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("data4").expired());

        // Check connection with data 3
        srv2->resetIsUpdated();
        auto modifiedSig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        modifiedSig3->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());

        // Create data 2
        ::fwData::Boolean::sptr data2b = ::fwData::Boolean::New();

        ::fwServices::OSR::registerServiceOutput(data2b, "out2", genDataSrv);
        fwTestWaitMacro(!srv2->getWeakInput< ::fwData::Object>(
                            "data2").expired() &&
                        data2b == srv2->getLockedInput< ::fwData::Object>("data2").get_shared());

        CPPUNIT_ASSERT(data5 == srv2->getLockedInput< ::fwData::Object>("data5").get_shared());
        CPPUNIT_ASSERT(data2b == srv2->getLockedInput< ::fwData::Object>("data2").get_shared());
        CPPUNIT_ASSERT(data3 == srv2->getLockedInput< ::fwData::Object>("data3").get_shared());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("data4").expired());

        // Check no connection with data 2
        srv2->resetIsUpdated();
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        auto modifiedSig2 = data2b->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        modifiedSig2->asyncEmit();
        fwTestWaitMacro(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        // Overwrite data 2 with a new data generated by an another service
        ::fwData::Boolean::sptr data2bis = ::fwData::Boolean::New();

        auto genDataSrv2 = ::fwServices::ut::TestService::dynamicCast(core::tools::fwID::getObject("SGenerateData2"));
        CPPUNIT_ASSERT(genDataSrv2 != nullptr);

        ::fwServices::OSR::registerServiceOutput(data2bis, "out", genDataSrv2);
        fwTestWaitMacro(!srv2->getWeakInput< ::fwData::Object>(
                            "data2").expired() && data2bis == srv2->getLockedInput< ::fwData::Object>(
                            "data2").get_shared());

        CPPUNIT_ASSERT(data2bis == srv2->getLockedInput< ::fwData::Object>("data2").get_shared());
        CPPUNIT_ASSERT("data2" == srv2->getSwappedObjectKey() );
        CPPUNIT_ASSERT(data2bis == srv2->getSwappedObject() );

        // Check that the output of SGenerateData changed as well
        CPPUNIT_ASSERT( data2bis ==
                        ::fwServices::OSR::getRegistered("out2", ::fwServices::IService::AccessType::OUTPUT,
                                                         genDataSrv));

        // Revert that
        ::fwServices::OSR::registerServiceOutput(data2b, "out", genDataSrv2);
        fwTestWaitMacro(!srv2->getWeakInput< ::fwData::Object>("data2").expired() &&
                        data2b == srv2->getLockedInput< ::fwData::Object>("data2").get_shared());

        CPPUNIT_ASSERT(data2b == srv2->getLockedInput< ::fwData::Object>("data2").get_shared());
        CPPUNIT_ASSERT("data2" == srv2->getSwappedObjectKey() );
        CPPUNIT_ASSERT(data2b == srv2->getSwappedObject() );

        // Check that the output of SGenerateData changed as well
        CPPUNIT_ASSERT( data2b ==
                        ::fwServices::OSR::getRegistered("out2", ::fwServices::IService::AccessType::OUTPUT,
                                                         genDataSrv));
    }
}

//------------------------------------------------------------------------------

void AppConfigTest::keyGroupTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("keyGroupTest");

    // Service used to generate data
    auto genDataSrv = ::fwServices::ut::TestService::dynamicCast(core::tools::fwID::getObject("SGenerateData"));
    CPPUNIT_ASSERT(genDataSrv != nullptr);

    auto data1 = std::dynamic_pointer_cast< ::fwData::Object >(core::tools::fwID::getObject("data1Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    auto data4 = std::dynamic_pointer_cast< ::fwData::Object >(core::tools::fwID::getObject("data4Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    auto data5 = std::dynamic_pointer_cast< ::fwData::Object >(core::tools::fwID::getObject("data5Id"));
    CPPUNIT_ASSERT(data1 != nullptr);

    // =================================================================================================================
    // Test service with one key group of two data
    // =================================================================================================================

    ::fwData::Image::sptr data3;
    {
        core::tools::Object::sptr gnsrv1 = core::tools::fwID::getObject("TestService1Uid");
        CPPUNIT_ASSERT(gnsrv1 == nullptr);

        // Create data 2b
        ::fwData::Boolean::sptr data2b = ::fwData::Boolean::New();
        ::fwServices::OSR::registerServiceOutput(data2b, "out2", genDataSrv);
        WAIT_SERVICE_STARTED("TestService1Uid");

        gnsrv1 = core::tools::fwID::getObject("TestService1Uid");
        auto srv1 = ::fwServices::ut::TestServiceImplementation::dynamicCast(gnsrv1);
        CPPUNIT_ASSERT(srv1 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv1->getStatus());
        CPPUNIT_ASSERT(!srv1->getIsUpdated());

        CPPUNIT_ASSERT(data1 == srv1->getLockedInput< ::fwData::Object>("data1").get_shared());
        CPPUNIT_ASSERT(data2b == srv1->getLockedInput< ::fwData::Object>("dataGroup#0").get_shared());
        CPPUNIT_ASSERT(srv1->getWeakInput< ::fwData::Object>("dataGroup#1").expired());

        CPPUNIT_ASSERT(data2b == srv1->getLockedInput< ::fwData::Object>("dataGroup", 0 ).get_shared());
        CPPUNIT_ASSERT(srv1->getWeakInput< ::fwData::Object>("dataGroup", 1 ).expired());
        CPPUNIT_ASSERT(srv1->getWeakInput< ::fwData::Object>("dataGroup", 1 ).expired());

        CPPUNIT_ASSERT(2 == srv1->getKeyGroupSize("dataGroup") );

        // Check connection with data 2
        CPPUNIT_ASSERT(!srv1->getIsUpdated());
        auto sig2 = data2b->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig2->asyncEmit();
        fwTestWaitMacro(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv1->getIsUpdated());

        // Create data 3
        data3 = ::fwData::Image::New();

        ::fwServices::OSR::registerServiceOutput(data3, "out3", genDataSrv);

        fwTestWaitMacro(!srv1->getWeakInput< ::fwData::Object>(
                            "dataGroup#1").expired() && data3 == srv1->getLockedInput< ::fwData::Object>(
                            "dataGroup#1").get_shared());

        CPPUNIT_ASSERT(data2b == srv1->getLockedInput< ::fwData::Object>("dataGroup#0").get_shared());
        CPPUNIT_ASSERT(data3 == srv1->getLockedInput< ::fwData::Object>("dataGroup#1").get_shared());

        CPPUNIT_ASSERT(data2b == srv1->getLockedInput< ::fwData::Object>("dataGroup", 0 ).get_shared());
        CPPUNIT_ASSERT(data3 == srv1->getLockedInput< ::fwData::Object>("dataGroup", 1 ).get_shared());

        // Check connection with data 3
        srv1->resetIsUpdated();
        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig3->asyncEmit();
        fwTestWaitMacro(srv1->getIsUpdated());
        CPPUNIT_ASSERT(srv1->getIsUpdated());
    }

    // Remove data 2
    {
        ::fwServices::OSR::unregisterServiceOutput("out2", genDataSrv);

        fwTestWaitMacro(false == core::tools::fwID::exist("TestService1Uid"));

        core::tools::Object::sptr gnsrv5 = core::tools::fwID::getObject("TestService1Uid");
        CPPUNIT_ASSERT(gnsrv5 == nullptr);
    }

    {
        WAIT_SERVICE_STARTED("TestService2Uid");
        core::tools::Object::sptr gnsrv2 = core::tools::fwID::getObject("TestService2Uid");
        auto srv2                        = ::fwServices::ut::TestSrvAutoconnect::dynamicCast(gnsrv2);
        CPPUNIT_ASSERT(srv2 != nullptr);
        CPPUNIT_ASSERT_EQUAL(::fwServices::IService::STARTED, srv2->getStatus());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        fwTestWaitMacro(!srv2->getWeakInput< ::fwData::Object>(
                            "dataGroup1#0").expired() && data3 == srv2->getLockedInput< ::fwData::Object>(
                            "dataGroup1#0").get_shared());

        CPPUNIT_ASSERT(2 == srv2->getKeyGroupSize("dataGroup0") );
        CPPUNIT_ASSERT(data1 == srv2->getLockedInput< ::fwData::Object>("dataGroup0#0").get_shared());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("dataGroup0#1").expired());

        CPPUNIT_ASSERT(data1 == srv2->getLockedInput< ::fwData::Object>("dataGroup0", 0 ).get_shared());
        CPPUNIT_ASSERT(srv2->getWeakInput< ::fwData::Object>("dataGroup0", 1 ).expired());

        CPPUNIT_ASSERT(3 == srv2->getKeyGroupSize("dataGroup1") );
        CPPUNIT_ASSERT(data3 == srv2->getLockedInput< ::fwData::Object>("dataGroup1#0").get_shared());
        CPPUNIT_ASSERT(data4 == srv2->getLockedInput< ::fwData::Object>("dataGroup1#1").get_shared());
        CPPUNIT_ASSERT(data5 == srv2->getLockedInput< ::fwData::Object>("dataGroup1#2").get_shared());

        CPPUNIT_ASSERT(data3 == srv2->getLockedInput< ::fwData::Object>("dataGroup1", 0).get_shared());
        CPPUNIT_ASSERT(data4 == srv2->getLockedInput< ::fwData::Object>("dataGroup1", 1).get_shared());
        CPPUNIT_ASSERT(data5 == srv2->getLockedInput< ::fwData::Object>("dataGroup1", 2).get_shared());

        // Check connection with data 1
        srv2->resetIsUpdated();
        auto sig1 = data1->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig1->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());

        // Check no connection with data 3
        srv2->resetIsUpdated();
        auto sig3 = data3->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig3->asyncEmit();
        fwTestWaitMacro(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        // Check connection with data 4
        srv2->resetIsUpdated();
        auto sig4 = data4->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig4->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated(), 2500);
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
        auto sigIm4 = data4->signal< ::fwData::Image::BufferModifiedSignalType>(::fwData::Image::s_BUFFER_MODIFIED_SIG);
        sigIm4->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated());
        CPPUNIT_ASSERT(srv2->getIsUpdated());

        // Check no connection with data 5
        srv2->resetIsUpdated();
        auto sig5 = data5->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
        sig5->asyncEmit();
        fwTestWaitMacro(!srv2->getIsUpdated());
        CPPUNIT_ASSERT(!srv2->getIsUpdated());

        auto sigIm5 = data5->signal< ::fwData::Image::BufferModifiedSignalType>(::fwData::Image::s_BUFFER_MODIFIED_SIG);
        sigIm5->asyncEmit();
        fwTestWaitMacro(srv2->getIsUpdated(), 2500);
        CPPUNIT_ASSERT(!srv2->getIsUpdated());
    }

    return;
}

//-----------------------------------------------------------------------------

void AppConfigTest::concurentAccessToAppConfigTest()
{
    const auto fn = std::bind(&AppConfigTest::parametersConfigTest, this);
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

    ::fwServices::registry::AppConfig::getDefault()->clearRegistry();
    std::vector< std::string > allCconfigs = ::fwServices::registry::AppConfig::getDefault()->getAllConfigs();
    CPPUNIT_ASSERT(allCconfigs.empty());

}

//------------------------------------------------------------------------------

void AppConfigTest::parameterReplaceTest()
{
    m_appConfigMgr = this->launchAppConfigMgr("parameterReplaceTest", true);

    unsigned int i = 0, j = 0;
    core::tools::Object::sptr gnsrv1, gnsrv2;

    // Not really elegant, but we have to "guess" how it is replaced
    while (gnsrv1 == nullptr && i++ < 200)
    {
        gnsrv1 = core::tools::fwID::getObject("parameterReplaceTest_" + std::to_string(i) + "_TestService1Uid");
    }

    auto srv1 = ::fwServices::ut::TestService::dynamicCast(gnsrv1);
    CPPUNIT_ASSERT(srv1 != nullptr);

    gnsrv2 = core::tools::fwID::getObject("parameterReplaceTest_" + std::to_string(i) + "_TestService2Uid");
    auto srv2          = ::fwServices::IService::dynamicCast(gnsrv2);
    auto adaptedConfig = srv2->getConfiguration();

    typedef core::runtime::ConfigurationElement::sptr ConfigType;
    const std::vector< ConfigType > paramsCfg = adaptedConfig->find("parameter");
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), paramsCfg.size());

    std::string replaceBy;

    CPPUNIT_ASSERT_EQUAL(std::string("patient"), paramsCfg[0]->getAttributeValue("replace"));
    CPPUNIT_ASSERT_EQUAL(std::string("name"), paramsCfg[0]->getAttributeValue("by"));

    replaceBy = paramsCfg[1]->getAttributeValue("by");
    CPPUNIT_ASSERT_EQUAL(std::string("parameterReplaceTest_" + std::to_string(i) + "_Channel No5"), replaceBy);

    replaceBy = paramsCfg[2]->getAttributeValue("by");
    CPPUNIT_ASSERT_EQUAL(std::string("parameterReplaceTest_" + std::to_string(i) + "_disneyChannel"), replaceBy);

    replaceBy = paramsCfg[3]->getAttributeValue("by");
    CPPUNIT_ASSERT_EQUAL(std::string("parameterReplaceTest_" + std::to_string(i) + "_view1"), replaceBy);

    core::tools::Object::sptr gnsubsrv;
    // Not really elegant, but we have to "guess" how it is replaced
    while (gnsubsrv == nullptr && j++ < 200)
    {
        gnsubsrv = core::tools::fwID::getObject( "parameterReplaceTestSubConfig_"  + std::to_string(j) +
                                                 "_TestServiceUid");
    }

    auto srvInSubConfig = ::fwServices::ut::TestService::dynamicCast(gnsubsrv);
    CPPUNIT_ASSERT(srvInSubConfig != nullptr);

    CPPUNIT_ASSERT(srvInSubConfig->isStarted());

    auto data1 = srv1->getInput< ::fwData::Object >("data1");
    CPPUNIT_ASSERT(data1 != nullptr);
    CPPUNIT_ASSERT_EQUAL(std::string("data1Id"), data1->getID());

    auto data2 = srv1->getInput< ::fwData::Object >("data2");
    CPPUNIT_ASSERT(data2 != nullptr);

    auto data1SubSrv = srvInSubConfig->getInput< ::fwData::Object >("data1");
    CPPUNIT_ASSERT(data1 == data1SubSrv);

    auto data2SubSrv = srvInSubConfig->getInput< ::fwData::Object >("data2");
    CPPUNIT_ASSERT(data2 == data2SubSrv);

    // check connections through the subconfig channel
    CPPUNIT_ASSERT(!srv1->getIsUpdated());

    auto sig1 = data1SubSrv->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig1->asyncEmit();

    fwTestWaitMacro(srv1->getIsUpdated());
    CPPUNIT_ASSERT(srv1->getIsUpdated());

    CPPUNIT_ASSERT(!srvInSubConfig->getIsUpdated());

    auto sig2 = data2->signal< ::fwData::Object::ModifiedSignalType>(::fwData::Object::s_MODIFIED_SIG);
    sig2->asyncEmit();

    fwTestWaitMacro(srvInSubConfig->getIsUpdated());
    CPPUNIT_ASSERT(srvInSubConfig->getIsUpdated());
}

//------------------------------------------------------------------------------

core::runtime::ConfigurationElement::sptr AppConfigTest::buildConfig()
{
    // Object
    std::shared_ptr< core::runtime::EConfigurationElement > cfg( new core::runtime::EConfigurationElement("object"));
    cfg->setAttributeValue( "uid", "image");
    cfg->setAttributeValue( "type", "::fwData::Image");

    // Service
    std::shared_ptr< core::runtime::EConfigurationElement > serviceA = cfg->addConfigurationElement("service");
    serviceA->setAttributeValue( "uid", "myTestService1" );
    serviceA->setAttributeValue("type", "::fwServices::ut::TestServiceImplementationImage" );
    serviceA->setAttributeValue( "autoConnect", "no" );

    // Connections
    std::shared_ptr< core::runtime::EConfigurationElement > connect1 = cfg->addConfigurationElement("connect");
    connect1->setAttributeValue( "channel", "channel1" );
    connect1->addConfigurationElement("signal")->setValue( "image/modified" );
    connect1->addConfigurationElement("slot")->setValue( "myTestService1/update" );

    // Start method from object's services
    std::shared_ptr< core::runtime::EConfigurationElement > startA = cfg->addConfigurationElement("start");
    startA->setAttributeValue( "uid", "myTestService1" );

    // Update method from object's services
    std::shared_ptr< core::runtime::EConfigurationElement > updateA = cfg->addConfigurationElement("update");
    updateA->setAttributeValue( "uid", "myTestService1" );

    return cfg;
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace fwServices
