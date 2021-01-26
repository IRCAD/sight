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

#include <services/IService.hpp>
#include <services/macros.hpp>
#include <services/op/Add.hpp>
#include <services/op/Get.hpp>
#include <services/registry/ServiceFactory.hpp>

#include <data/Float.hpp>
#include <data/Integer.hpp>

#include <unordered_set>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::services::ut::ServiceFactoryTest );

//------------------------------------------------------------------------------

namespace sight::services
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

class TestEmptyService : public services::IService
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
    fwCoreServiceMacro(TestSrvOneData, services::IService)

};
class TestSrvThreeData : public TestEmptyService
{
public:
    fwCoreServiceMacro(TestSrvThreeData, services::IService)

};

fwServicesRegisterMacro( ::sight::services::ut::TestService, ::sight::services::ut::TestSrvOneData,
                         ::sight::data::Float )
fwServicesRegisterMacro( ::sight::services::ut::TestService, ::sight::services::ut::TestSrvThreeData )
fwServicesRegisterObjectMacro(  ::sight::services::ut::TestSrvThreeData, ::sight::data::Float );
fwServicesRegisterObjectMacro(  ::sight::services::ut::TestSrvThreeData, ::sight::data::Integer );
fwServicesRegisterObjectMacro(  ::sight::services::ut::TestSrvThreeData, ::sight::data::Float );

//------------------------------------------------------------------------------

void ServiceFactoryTest::factoryTest()
{
    const auto serviceFactory = services::registry::ServiceFactory::getDefault();
    {
        const auto servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::sight::data::Object",
                                                                                       "::sight::services::ut::TestService");

        auto it =
            std::find(servicesImpl.begin(), servicesImpl.end(), "::sight::services::ut::TestServiceImplementation");
        CPPUNIT_ASSERT_MESSAGE("::sight::services::ut::TestServiceImplementation not found ", it != servicesImpl.end());
    }
    {
        const auto servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::sight::data::Float",
                                                                                       "::sight::services::ut::TestService");

        auto it = std::find(servicesImpl.begin(), servicesImpl.end(), "::sight::services::ut::TestSrvOneData");
        CPPUNIT_ASSERT_MESSAGE("::sight::services::ut::TestSrvOneData not found ", it != servicesImpl.end());

        const auto objects = serviceFactory->getServiceObjects("::sight::services::ut::TestSrvOneData");
        CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Float"), objects[0]);
    }
    {
        auto servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::sight::data::Float",
                                                                                 "::sight::services::ut::TestService");

        auto it = std::find(servicesImpl.begin(), servicesImpl.end(), "::sight::services::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_MESSAGE("::sight::services::ut::TestSrvThreeData not found ", it != servicesImpl.end());

        servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::sight::data::Integer",
                                                                            "::sight::services::ut::TestService");

        it = std::find(servicesImpl.begin(), servicesImpl.end(), "::sight::services::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_MESSAGE("::sight::services::ut::TestSrvThreeData not found ", it != servicesImpl.end());

        const auto objects = serviceFactory->getServiceObjects("::sight::services::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), objects.size());
        CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Float"), objects[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Integer"), objects[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("::sight::data::Float"), objects[2]);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::services
