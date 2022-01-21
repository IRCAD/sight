/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ServiceConfigTest.hpp"

#include <data/String.hpp>

#include <service/extension/AppConfig.hpp>
#include <service/extension/Config.hpp>

#include <utest/wait.hpp>

#include <functional>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::ServiceConfigTest);

namespace sight::service
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
    service::extension::Config::sptr currentServiceConfig;
    currentServiceConfig = service::extension::Config::getDefault();

    const std::string configId(service::extension::AppConfig::getUniqueIdentifier());
    const std::string service("sight::service::ut::TestService");
    const std::string desc("Description of config");
    core::runtime::ConfigurationElement::csptr config = this->buildConfig();

    currentServiceConfig->addServiceConfigInfo(configId, service, desc, config);

    core::runtime::ConfigurationElement::csptr serviceConfig =
        currentServiceConfig->getServiceConfig(configId, service);
    CPPUNIT_ASSERT(serviceConfig);
    CPPUNIT_ASSERT(serviceConfig->hasAttribute("uid"));
    CPPUNIT_ASSERT_EQUAL(std::string("serviceUUID"), serviceConfig->getAttributeValue("uid"));

    std::vector<std::string> configs = currentServiceConfig->getAllConfigForService(service);
    CPPUNIT_ASSERT(!configs.empty());
    CPPUNIT_ASSERT(std::find(configs.begin(), configs.end(), configId) != configs.end());
}

//------------------------------------------------------------------------------

void ServiceConfigTest::concurentAccessToServiceConfigTest()
{
    const auto fn = std::bind(&ServiceConfigTest::serviceConfigTest, this);
    std::vector<std::future<void> > futures;
    for(unsigned int i = 0 ; i < 20 ; ++i)
    {
        futures.push_back(std::async(std::launch::async, fn));
    }

    std::for_each(futures.begin(), futures.end(), std::mem_fn(&std::shared_future<void>::wait));

    service::extension::Config::sptr currentServiceConfig;
    currentServiceConfig = service::extension::Config::getDefault();
    currentServiceConfig->clearRegistry();
    std::vector<std::string> allConfigs = currentServiceConfig->getAllConfigForService(
        "sight::service::ut::TestService"
    );
    CPPUNIT_ASSERT(allConfigs.empty());
}

//------------------------------------------------------------------------------

void ServiceConfigTest::getAllConfigsTest()
{
    service::extension::Config::sptr currentServiceConfig;
    currentServiceConfig = service::extension::Config::getDefault();

    const std::string serviceName0    = "::wow::MuchService";
    const std::string serviceName1    = "::no::BadService";
    const std::string configIdPrefix0 = "muchServiceConfig";
    const std::string configIdPrefix1 = "badServiceConfig";
    const std::string configIdPrefix2 = "emptyConfig";

    std::vector<std::string> vectConfigTest0;
    std::vector<std::string> vectConfigTest1;

    for(int i = 0 ; i < 6 ; ++i)
    {
        const std::string configId = configIdPrefix0 + std::to_string(i);

        vectConfigTest0.push_back(configId);

        currentServiceConfig->addServiceConfigInfo(
            configId,
            serviceName0,
            "Much test, oh wow",
            nullptr
        );
    }

    for(int i = 0 ; i < 4 ; ++i)
    {
        const std::string configId = configIdPrefix1 + std::to_string(i);

        vectConfigTest1.push_back(configId);

        currentServiceConfig->addServiceConfigInfo(
            configId,
            serviceName1,
            "No, bad test ! Bad !",
            nullptr
        );
    }

    for(int i = 0 ; i < 3 ; ++i)
    {
        const std::string configId = configIdPrefix2 + std::to_string(i);

        vectConfigTest0.push_back(configId);
        vectConfigTest1.push_back(configId);

        currentServiceConfig->addServiceConfigInfo(
            configId,
            "", // Empty type.
            "Such empty, much space, wow.",
            nullptr
        );
    }

    const std::vector<std::string> test0 = currentServiceConfig->getAllConfigForService(serviceName0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(9), test0.size());

    for(std::size_t i = 0 ; i < test0.size() ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(
            true,
            std::find(
                vectConfigTest0.begin(),
                vectConfigTest0.end(),
                test0[i]
            ) != vectConfigTest0.end()
        );
    }

    const std::vector<std::string> test1 = currentServiceConfig->getAllConfigForService(serviceName1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(7), test1.size());

    for(std::size_t i = 0 ; i < test1.size() ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(
            true,
            std::find(
                vectConfigTest1.begin(),
                vectConfigTest1.end(),
                test1[i]
            ) != vectConfigTest1.end()
        );
    }

    const std::vector<std::string> test2 = currentServiceConfig->getAllConfigForService("");
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), test2.size());

    for(std::size_t i = 0 ; i < test2.size() ; ++i)
    {
        const std::string expected = configIdPrefix2 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test2[i]);
    }

    const std::vector<std::string> test3 = currentServiceConfig->getAllConfigForService(serviceName0, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(6), test3.size());

    for(std::size_t i = 0 ; i < test3.size() ; ++i)
    {
        const std::string expected = configIdPrefix0 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test3[i]);
    }

    const std::vector<std::string> test4 = currentServiceConfig->getAllConfigForService(serviceName1, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(4), test4.size());

    for(std::size_t i = 0 ; i < test4.size() ; ++i)
    {
        const std::string expected = configIdPrefix1 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test4[i]);
    }

    const std::vector<std::string> test5 = currentServiceConfig->getAllConfigForService("", true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), test5.size());

    for(std::size_t i = 0 ; i < test5.size() ; ++i)
    {
        const std::string expected = configIdPrefix2 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test5[i]);
    }
}

//------------------------------------------------------------------------------

core::runtime::ConfigurationElement::sptr ServiceConfigTest::buildConfig()
{
    std::shared_ptr<core::runtime::EConfigurationElement> serviceCfg(new core::runtime::EConfigurationElement(
                                                                         "config"
    ));
    serviceCfg->setAttributeValue("uid", "serviceUUID");
    serviceCfg->setAttributeValue("type", "serviceType");

    core::runtime::EConfigurationElement::sptr cfg = serviceCfg->addConfigurationElement("param");
    cfg->setValue("Parameter");

    return serviceCfg;
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::service
