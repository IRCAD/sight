/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
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

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>

#include <unordered_set>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ServiceFactoryTest );

//------------------------------------------------------------------------------

namespace fwServices
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

class TestEmptyService : public fwServices::IService
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
    fwCoreServiceClassDefinitionsMacro( (TestSrvOneData)(::fwServices::IService) )

};
class TestSrvThreeData : public TestEmptyService
{
public:
    fwCoreServiceClassDefinitionsMacro( (TestSrvThreeData)(::fwServices::IService) )

};

fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestSrvOneData, ::fwData::Float );
fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestSrvThreeData );
fwServicesRegisterObjectMacro( ::fwServices::ut::TestSrvThreeData, ::fwData::Float );
fwServicesRegisterObjectMacro( ::fwServices::ut::TestSrvThreeData, ::fwData::Integer );
fwServicesRegisterObjectMacro( ::fwServices::ut::TestSrvThreeData, ::fwData::Float );

//------------------------------------------------------------------------------

void ServiceFactoryTest::factoryTest()
{
    const auto serviceFactory = ::fwServices::registry::ServiceFactory::getDefault();
    {
        const auto servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::fwData::Object",
                                                                                       "::fwServices::ut::TestService");

        auto it = std::find(servicesImpl.begin(), servicesImpl.end(), "::fwServices::ut::TestServiceImplementation");
        CPPUNIT_ASSERT_MESSAGE("::fwServices::ut::TestServiceImplementation not found ", it != servicesImpl.end());
    }
    {
        const auto servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::fwData::Float",
                                                                                       "::fwServices::ut::TestService");

        auto it = std::find(servicesImpl.begin(), servicesImpl.end(), "::fwServices::ut::TestSrvOneData");
        CPPUNIT_ASSERT_MESSAGE("::fwServices::ut::TestSrvOneData not found ", it != servicesImpl.end());

        const auto objects = serviceFactory->getServiceObjects("::fwServices::ut::TestSrvOneData");
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Float"), objects[0]);
    }
    {
        auto servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::fwData::Float",
                                                                                 "::fwServices::ut::TestService");

        auto it = std::find(servicesImpl.begin(), servicesImpl.end(), "::fwServices::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_MESSAGE("::fwServices::ut::TestSrvThreeData not found ", it != servicesImpl.end());

        servicesImpl = serviceFactory->getImplementationIdFromObjectAndType("::fwData::Integer",
                                                                            "::fwServices::ut::TestService");

        it = std::find(servicesImpl.begin(), servicesImpl.end(), "::fwServices::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_MESSAGE("::fwServices::ut::TestSrvThreeData not found ", it != servicesImpl.end());

        const auto objects = serviceFactory->getServiceObjects("::fwServices::ut::TestSrvThreeData");
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), objects.size());
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Float"), objects[0]);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Integer"), objects[1]);
        CPPUNIT_ASSERT_EQUAL(std::string("::fwData::Float"), objects[2]);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
