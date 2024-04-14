/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "service_config_test.hpp"

#include <data/string.hpp>

#include <service/extension/config.hpp>

#include <utest/wait.hpp>

#include <functional>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::service_config_test);

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void service_config_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void service_config_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void service_config_test::config_test()
{
    service::extension::config::sptr current_service_config;
    current_service_config = service::extension::config::get_default();

    static std::atomic<unsigned int> i = 0;
    const std::string config_id("appTest" + std::to_string(i++));
    const std::string service("sight::service::ut::test_service");
    const std::string desc("Description of config");
    service::config_t config = build_config();

    current_service_config->add_service_config_info(config_id, service, desc, config);

    const auto service_config = current_service_config->get_service_config(config_id, service);
    CPPUNIT_ASSERT(!service_config.empty());
    CPPUNIT_ASSERT(service_config.get_optional<std::string>("service.<xmlattr>.uid").has_value());
    CPPUNIT_ASSERT_EQUAL(std::string("serviceUUID"), service_config.get<std::string>("service.<xmlattr>.uid"));

    std::vector<std::string> configs = current_service_config->get_all_config_for_service(service);
    CPPUNIT_ASSERT(!configs.empty());
    CPPUNIT_ASSERT(std::find(configs.begin(), configs.end(), config_id) != configs.end());
}

//------------------------------------------------------------------------------

void service_config_test::concurrent_access_to_service_config_test()
{
    std::vector<std::future<void> > futures;
    for(unsigned int i = 0 ; i < 20 ; ++i)
    {
        futures.push_back(std::async(std::launch::async, config_test));
    }

    std::for_each(futures.begin(), futures.end(), std::mem_fn(&std::shared_future<void>::wait));

    service::extension::config::sptr current_service_config;
    current_service_config = service::extension::config::get_default();
    current_service_config->clear_registry();
    std::vector<std::string> all_configs = current_service_config->get_all_config_for_service(
        "sight::service::ut::test_service"
    );
    CPPUNIT_ASSERT(all_configs.empty());
}

//------------------------------------------------------------------------------

void service_config_test::get_all_configs_test()
{
    service::extension::config::sptr current_service_config;
    current_service_config = service::extension::config::get_default();

    const std::string service_name0     = "::wow::MuchService";
    const std::string service_name1     = "::no::BadService";
    const std::string config_id_prefix0 = "muchServiceConfig";
    const std::string config_id_prefix1 = "badServiceConfig";
    const std::string config_id_prefix2 = "emptyConfig";

    std::vector<std::string> vect_config_test0;
    std::vector<std::string> vect_config_test1;

    for(int i = 0 ; i < 6 ; ++i)
    {
        const std::string config_id = config_id_prefix0 + std::to_string(i);

        vect_config_test0.push_back(config_id);

        current_service_config->add_service_config_info(
            config_id,
            service_name0,
            "Much test, oh wow",
            boost::property_tree::ptree()
        );
    }

    for(int i = 0 ; i < 4 ; ++i)
    {
        const std::string config_id = config_id_prefix1 + std::to_string(i);

        vect_config_test1.push_back(config_id);

        current_service_config->add_service_config_info(
            config_id,
            service_name1,
            "No, bad test ! Bad !",
            boost::property_tree::ptree()
        );
    }

    for(int i = 0 ; i < 3 ; ++i)
    {
        const std::string config_id = config_id_prefix2 + std::to_string(i);

        vect_config_test0.push_back(config_id);
        vect_config_test1.push_back(config_id);

        current_service_config->add_service_config_info(
            config_id,
            "", // Empty type.
            "Such empty, much space, wow.",
            boost::property_tree::ptree()
        );
    }

    const std::vector<std::string> test0 = current_service_config->get_all_config_for_service(service_name0);
    CPPUNIT_ASSERT_EQUAL(std::size_t(9), test0.size());

    for(const auto& i : test0)
    {
        CPPUNIT_ASSERT_EQUAL(
            true,
            std::find(
                vect_config_test0.begin(),
                vect_config_test0.end(),
                i
            ) != vect_config_test0.end()
        );
    }

    const std::vector<std::string> test1 = current_service_config->get_all_config_for_service(service_name1);
    CPPUNIT_ASSERT_EQUAL(std::size_t(7), test1.size());

    for(const auto& i : test1)
    {
        CPPUNIT_ASSERT_EQUAL(
            true,
            std::find(
                vect_config_test1.begin(),
                vect_config_test1.end(),
                i
            ) != vect_config_test1.end()
        );
    }

    const std::vector<std::string> test2 = current_service_config->get_all_config_for_service("");
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), test2.size());

    for(std::size_t i = 0 ; i < test2.size() ; ++i)
    {
        const std::string expected = config_id_prefix2 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test2[i]);
    }

    const std::vector<std::string> test3 = current_service_config->get_all_config_for_service(service_name0, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(6), test3.size());

    for(std::size_t i = 0 ; i < test3.size() ; ++i)
    {
        const std::string expected = config_id_prefix0 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test3[i]);
    }

    const std::vector<std::string> test4 = current_service_config->get_all_config_for_service(service_name1, true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(4), test4.size());

    for(std::size_t i = 0 ; i < test4.size() ; ++i)
    {
        const std::string expected = config_id_prefix1 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test4[i]);
    }

    const std::vector<std::string> test5 = current_service_config->get_all_config_for_service("", true);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), test5.size());

    for(std::size_t i = 0 ; i < test5.size() ; ++i)
    {
        const std::string expected = config_id_prefix2 + std::to_string(i);
        CPPUNIT_ASSERT_EQUAL(expected, test5[i]);
    }
}

//------------------------------------------------------------------------------

service::config_t service_config_test::build_config()
{
    service::config_t config;

    service::config_t service_cfg;
    service_cfg.add("<xmlattr>.uid", "serviceUUID");
    service_cfg.add("<xmlattr>.type", "serviceType");
    service_cfg.add("param", "Parameter");

    config.add_child("service", service_cfg);
    return config;
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
