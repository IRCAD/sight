/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ServiceConfigTest.hpp"

#include <fwServices/registry/AppConfig.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwData/String.hpp>

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
    const auto fn = std::bind(&ServiceConfigTest::serviceConfigTest, this);
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

    ::fwServices::registry::ServiceConfig::sptr currentServiceConfig;
    currentServiceConfig = ::fwServices::registry::ServiceConfig::getDefault();
    currentServiceConfig->clearRegistry();
    std::vector< std::string > allCconfigs = currentServiceConfig->getAllConfigForService(
        "::fwServices::ut::TestService");
    CPPUNIT_ASSERT(allCconfigs.empty());
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
