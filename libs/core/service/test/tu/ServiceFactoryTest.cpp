/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "ServiceFactoryTest.hpp"

#include "TestService.hpp"

#include <data/Float.hpp>
#include <data/Integer.hpp>

#include <service/extension/Factory.hpp>
#include <service/IService.hpp>
#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/op/Get.hpp>

#include <unordered_set>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::service::ut::ServiceFactoryTest );

//------------------------------------------------------------------------------

namespace sight::service
{
namespace ut
{

//------------------------------------------------------------------------------

void ServiceFactoryTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ServiceFactoryTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

class TestEmptyService : public service::IService
{
public:
    //------------------------------------------------------------------------------

    virtual void configuring() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void starting() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void stopping() override
    {
    }
    //------------------------------------------------------------------------------

    virtual void updating() override
    {
    }
};

class TestSrvOneData : public TestEmptyService
{
public:
    SIGHT_DECLARE_SERVICE(TestSrvOneData, service::IService)

};
class TestSrvThreeData : public TestEmptyService
{
public:
    SIGHT_DECLARE_SERVICE(TestSrvThreeData, service::IService)

};

SIGHT_REGISTER_SERVICE( ::sight::service::ut::TestService, ::sight::service::ut::TestSrvOneData,
                        ::sight::data::Float )
SIGHT_REGISTER_SERVICE( ::sight::service::ut::TestService, ::sight::service::ut::TestSrvThreeData )
SIGHT_REGISTER_SERVICE_OBJECT(  ::sight::service::ut::TestSrvThreeData, ::sight::data::Float );
SIGHT_REGISTER_SERVICE_OBJECT(  ::sight::service::ut::TestSrvThreeData, ::sight::data::Integer );
SIGHT_REGISTER_SERVICE_OBJECT(  ::sight::service::ut::TestSrvThreeData, ::sight::data::Float );

//------------------------------------------------------------------------------

void ServiceFactoryTest::factoryTest()
{
    const auto serviceFactory = service::extension::Factory::getDefault();
    {
        const auto servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::sight::data::Object",
                                                                                       "::sight::service::ut::TestService");

        auto it =
            std::find(servicesImpl.begin(), servicesImpl.end(), "::sight::service::ut::TestServiceImplementation");
        CPPUNIT_ASSERT_MESSAGE("::sight::service::ut::TestServiceImplementation not found ", it != servicesImpl.end());
    }
    {
        const auto servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::sight::data::Float",
                                                                                       "::sight::service::ut::TestService");

        auto it = std::find(servicesImpl.begin(), servicesImpl.end(), "::sight::service::ut::TestSrvOneData");
        CPPUNIT_ASSERT_MESSAGE("::sight::service::ut::TestSrvOneData not found ", it != servicesImpl.end());

        const auto objects = serviceFactory->getServiceObjects("::sight::service::ut::TestSrvOneData");
        CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Float"), objects[0]);
    }
    {
        auto servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::sight::data::Float",
                                                                                 "::sight::service::ut::TestService");

        auto it = std::find(servicesImpl.begin(), servicesImpl.end(), "::sight::service::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_MESSAGE("::sight::service::ut::TestSrvThreeData not found ", it != servicesImpl.end());

        servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::sight::data::Integer",
                                                                            "::sight::service::ut::TestService");

        it = std::find(servicesImpl.begin(), servicesImpl.end(), "::sight::service::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_MESSAGE("::sight::service::ut::TestSrvThreeData not found ", it != servicesImpl.end());

        const auto objects = serviceFactory->getServiceObjects("::sight::service::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), objects.size());
        CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Float"), objects[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Integer"), objects[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Float"), objects[2]);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::service
