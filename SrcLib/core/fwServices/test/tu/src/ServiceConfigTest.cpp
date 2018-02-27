/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ServiceConfigTest.hpp"

#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwData/String.hpp>

#include <fwTest/helper/Thread.hpp>

#include <functional>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ServiceConfigTest );

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

void ServiceConfigTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ServiceConfigTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ServiceConfigTest::serviceConfigTest()
{
    ::fwServices::registry::ServiceConfig::sptr currentServiceConfig;
    currentServiceConfig = ::fwServices::registry::ServiceConfig::getDefault();

    const std::string configId(::fwServices::registry::AppConfig::getUniqueIdentifier());
    const std::string service("::fwServices::ut::TestService");
    const std::string desc("Description of config");
    ::fwRuntime::ConfigurationElement::csptr config = this->buildConfig();

    currentServiceConfig->addServiceConfigInfo( configId, service, desc, config );

    ::fwRuntime::ConfigurationElement::csptr serviceConfig = currentServiceConfig->getServiceConfig(configId, service);
    CPPUNIT_ASSERT( serviceConfig );
    CPPUNIT_ASSERT(serviceConfig->hasAttribute("uid"));
    CPPUNIT_ASSERT_EQUAL(std::string("serviceUUID"), serviceConfig->getAttributeValue("uid"));

    std::vector< std::string > configs = currentServiceConfig->getAllConfigForService(service);
    CPPUNIT_ASSERT( !configs.empty());
    CPPUNIT_ASSERT(std::find(configs.begin(), configs.end(), configId) != configs.end());
}

//------------------------------------------------------------------------------

void ServiceConfigTest::concurentAccessToServiceConfigTest()
{
    const unsigned int nbThreads = 20;
    std::vector< SPTR(::fwTest::helper::Thread) > threads;
    for (int i = 0; i < nbThreads; ++i)
    {
        SPTR(::fwTest::helper::Thread) thread;
        thread = std::shared_ptr< ::fwTest::helper::Thread >(
            new ::fwTest::helper::Thread(std::bind(&ServiceConfigTest::serviceConfigTest, this)));
        threads.push_back(thread);
    }

    for (int i = 0; i < nbThreads; ++i)
    {
        std::stringstream str;
        str << "thread " << i;
        CPPUNIT_ASSERT_MESSAGE(str.str(), threads[i]->timedJoin(1000));
    }

    ::fwServices::registry::ServiceConfig::sptr currentServiceConfig;
    currentServiceConfig = ::fwServices::registry::ServiceConfig::getDefault();
    currentServiceConfig->clearRegistry();
    std::vector< std::string > allCconfigs = currentServiceConfig->getAllConfigForService(
        "::fwServices::ut::TestService");
    CPPUNIT_ASSERT(allCconfigs.empty());
}

//------------------------------------------------------------------------------

void ServiceConfigTest::getAllConfigsTest()
{
    ::fwServices::registry::ServiceConfig::sptr currentServiceConfig;
    currentServiceConfig = ::fwServices::registry::ServiceConfig::getDefault();

    const std::string serviceName0    = "::wow::MuchService";
    const std::string serviceName1    = "::no::BadService";
    const std::string configIdPrefix0 = "muchServiceConfig";
    const std::string configIdPrefix1 = "badServiceConfig";
    const std::string configIdPrefix2 = "emptyConfig";

    std::vector<std::string> vectConfigTest0;
    std::vector<std::string> vectConfigTest1;

    for(int i = 0; i < 6; ++i)
    {
        const std::string configId = configIdPrefix0 + std::to_string(i);

        vectConfigTest0.push_back(configId);

        currentServiceConfig->addServiceConfigInfo(configId,
                                                   serviceName0,
                                                   "Much test, oh wow",
                                                   nullptr);
    }

    for(int i = 0; i < 4; ++i)
    {
        const std::string configId = configIdPrefix1 + std::to_string(i);

        vectConfigTest1.push_back(configId);

        currentServiceConfig->addServiceConfigInfo(configId,
                                                   serviceName1,
                                                   "No, bad test ! Bad !",
                                                   nullptr);
    }

    for(int i = 0; i < 3; ++i)
    {
        const std::string configId = configIdPrefix2 + std::to_string(i);

        vectConfigTest0.push_back(configId);
        vectConfigTest1.push_back(configId);

        currentServiceConfig->addServiceConfigInfo(configId,
                                                   "", // Empty type.
                                                   "Such empty, much space, wow.",
                                                   nullptr);
    }

    const std::vector<std::string> test0 = currentServiceConfig->getAllConfigForService(serviceName0);
    CPPUNIT_ASSERT_EQUAL(size_t(9), test0.size());

    for(size_t i = 0; i < test0.size(); ++i)
    {
        CPPUNIT_ASSERT_EQUAL(true, std::find(vectConfigTest0.begin(), vectConfigTest0.end(),
                                             test0[i]) != vectConfigTest0.end() );
    }

    const std::vector<std::string> test1 = currentServiceConfig->getAllConfigForService(serviceName1);
    CPPUNIT_ASSERT_EQUAL(size_t(7), test1.size());

    for(size_t i = 0; i < test1.size(); ++i)
    {
        CPPUNIT_ASSERT_EQUAL(true, std::find(vectConfigTest1.begin(), vectConfigTest1.end(),
                                             test1[i]) != vectConfigTest1.end() );
    }

    const std::vector<std::string> test2 = currentServiceConfig->getAllConfigForService("");
    CPPUNIT_ASSERT_EQUAL(size_t(3), test2.size());

    for(size_t i = 0; i < test2.size(); ++i)
    {
        const std::string expected = configIdPrefix2 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test2[i]);
    }

    const std::vector<std::string> test3 = currentServiceConfig->getAllConfigForService(serviceName0, true);
    CPPUNIT_ASSERT_EQUAL(size_t(6), test3.size());

    for(size_t i = 0; i < test3.size(); ++i)
    {
        const std::string expected = configIdPrefix0 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test3[i]);
    }

    const std::vector<std::string> test4 = currentServiceConfig->getAllConfigForService(serviceName1, true);
    CPPUNIT_ASSERT_EQUAL(size_t(4), test4.size());

    for(size_t i = 0; i < test4.size(); ++i)
    {
        const std::string expected = configIdPrefix1 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test4[i]);
    }

    const std::vector<std::string> test5 = currentServiceConfig->getAllConfigForService("", true);
    CPPUNIT_ASSERT_EQUAL(size_t(3), test5.size());

    for(size_t i = 0; i < test5.size(); ++i)
    {
        const std::string expected = configIdPrefix2 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test5[i]);
    }
}

//------------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr ServiceConfigTest::buildConfig()
{
    std::shared_ptr< ::fwRuntime::EConfigurationElement > serviceCfg( new ::fwRuntime::EConfigurationElement(
                                                                          "config"));
    serviceCfg->setAttributeValue( "uid", "serviceUUID");
    serviceCfg->setAttributeValue( "type", "serviceType");

    ::fwRuntime::EConfigurationElement::sptr cfg = serviceCfg->addConfigurationElement("param");
    cfg->setValue("Parameter");

    return serviceCfg;
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
