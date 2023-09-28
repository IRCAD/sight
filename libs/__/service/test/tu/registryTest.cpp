/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "registryTest.hpp"

#include "TestService.hpp"

#include <data/Float.hpp>
#include <data/Integer.hpp>

#include <service/base.hpp>
#include <service/extension/Factory.hpp>
#include <service/op.hpp>
#include <service/registry.hpp>

#include <unordered_set>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::registryTest);

//------------------------------------------------------------------------------

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void registryTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void registryTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void registryTest::registerKeyTest()
{
    const std::string srvType("sight::service::ut::test_srv");
    const std::string srvImplementation1("sight::service::ut::STestNoData");
    const std::string srvImplementation2("sight::service::ut::STestNoData2");

    data::Integer::sptr obj1 = std::make_shared<data::Integer>();
    data::Integer::sptr obj2 = std::make_shared<data::Integer>();
    data::Integer::sptr obj3 = std::make_shared<data::Integer>();

    auto service1 = service::extension::Factory::getDefault()->create(srvImplementation1);
    auto service2 = service::extension::Factory::getDefault()->create(srvImplementation2);
    auto service3 = service::extension::Factory::getDefault()->create(srvImplementation1);

    sight::service::registerService(service1);
    sight::service::registerService(service2);
    sight::service::registerService(service3);

    // 3 services in total
    {
        auto servicesByType         = sight::service::getServices(srvType);
        auto servicesByTemplateType = sight::service::getServices<service::ut::test_srv>();

        CPPUNIT_ASSERT_EQUAL(std::size_t(3), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    // 2 services of type "sight::service::ut::STestNoData"
    {
        auto servicesByType         = sight::service::getServices(srvImplementation1);
        auto servicesByTemplateType = sight::service::getServices<service::ut::STestNoData>();

        CPPUNIT_ASSERT_EQUAL(std::size_t(2), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    // 1 service of type "sight::service::ut::STestNoData2"
    {
        auto servicesByType         = sight::service::getServices(srvImplementation2);
        auto servicesByTemplateType = sight::service::getServices<service::ut::STestNoData2>();

        CPPUNIT_ASSERT_EQUAL(std::size_t(1), servicesByType.size());
        CPPUNIT_ASSERT(std::equal(servicesByType.begin(), servicesByType.end(), servicesByTemplateType.begin()));
    }

    auto servicesByType = sight::service::getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), servicesByType.size());

    CPPUNIT_ASSERT(servicesByType.find(service1) != servicesByType.end());
    CPPUNIT_ASSERT(servicesByType.find(service2) != servicesByType.end());
    CPPUNIT_ASSERT(servicesByType.find(service3) != servicesByType.end());

    // Remove key 1 from service 1 and check consistency
    servicesByType = sight::service::getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), servicesByType.size());

    sight::service::unregisterService(service1);
    servicesByType = sight::service::getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), servicesByType.size());

    servicesByType = sight::service::getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), servicesByType.size());

    sight::service::registerService(service1);
    servicesByType = sight::service::getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), servicesByType.size());

    // Remove service 3 and check consistency
    sight::service::unregisterService(service3);
    servicesByType = sight::service::getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), servicesByType.size());

    sight::service::unregisterService(service2);

    servicesByType = sight::service::getServices(srvType);
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), servicesByType.size());

    sight::service::unregisterService(service1);
    servicesByType = sight::service::getServices(srvType);
    CPPUNIT_ASSERT(servicesByType.empty());
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
