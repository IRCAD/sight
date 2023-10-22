/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "service_factory_test.hpp"

#include "test_service.hpp"

#include <data/integer.hpp>
#include <data/real.hpp>

#include <service/base.hpp>
#include <service/extension/factory.hpp>
#include <service/macros.hpp>
#include <service/op.hpp>

#include <unordered_set>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::service_factory_test);

//------------------------------------------------------------------------------

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void service_factory_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void service_factory_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

class TestEmptyService : public service::base
{
public:

    //------------------------------------------------------------------------------

    void configuring() override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }
};

class TestSrvOneData : public TestEmptyService
{
public:

    SIGHT_DECLARE_SERVICE(TestSrvOneData, service::base);
};
class TestSrvThreeData : public TestEmptyService
{
public:

    SIGHT_DECLARE_SERVICE(TestSrvThreeData, service::base);
};

SIGHT_REGISTER_SERVICE(
    sight::service::ut::test_service,
    sight::service::ut::TestSrvOneData,
    sight::data::real
)
SIGHT_REGISTER_SERVICE(sight::service::ut::test_service, sight::service::ut::TestSrvThreeData);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::TestSrvThreeData, sight::data::real);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::TestSrvThreeData, sight::data::integer);
SIGHT_REGISTER_SERVICE_OBJECT(sight::service::ut::TestSrvThreeData, sight::data::real);

//------------------------------------------------------------------------------

void service_factory_test::factoryTest()
{
    const auto service_factory = service::extension::factory::get();
    {
        const auto services_impl = service_factory->getImplementationIdFromObjectAndType(
            "sight::data::real",
            "sight::service::ut::test_service"
        );

        auto it = std::find(services_impl.begin(), services_impl.end(), "sight::service::ut::TestSrvOneData");
        CPPUNIT_ASSERT_MESSAGE("sight::service::ut::TestSrvOneData not found ", it != services_impl.end());

        const auto objects = service_factory->getServiceObjects("sight::service::ut::TestSrvOneData");
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::real"), objects[0]);
    }
    {
        auto services_impl = service_factory->getImplementationIdFromObjectAndType(
            "sight::data::real",
            "sight::service::ut::test_service"
        );

        auto it = std::find(services_impl.begin(), services_impl.end(), "sight::service::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_MESSAGE("sight::service::ut::TestSrvThreeData not found ", it != services_impl.end());

        services_impl = service_factory->getImplementationIdFromObjectAndType(
            "sight::data::integer",
            "sight::service::ut::test_service"
        );

        it = std::find(services_impl.begin(), services_impl.end(), "sight::service::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_MESSAGE("sight::service::ut::TestSrvThreeData not found ", it != services_impl.end());

        const auto objects = service_factory->getServiceObjects("sight::service::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), objects.size());
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::real"), objects[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::integer"), objects[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("sight::data::real"), objects[2]);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
