/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "IHasServicesTest.hpp"

#include "TestService.hpp"

#include <data/Boolean.hpp>

#include <service/IHasServices.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::IHasServicesTest);

//------------------------------------------------------------------------------

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void IHasServicesTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IHasServicesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

struct TestIHasServices : public service::IHasServices
{
    ~TestIHasServices() noexcept override
    {
        // just in case a test does not pass, we unregister all the services to avoid a crash
        this->unregisterServices();
    }

    //------------------------------------------------------------------------------

    void testConnection()
    {
        data::Boolean::sptr data1 = data::Boolean::New();
        data::Boolean::sptr data2 = data::Boolean::New();
        data::Boolean::sptr data3 = data::Boolean::New();

        auto sig1 = data1->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        auto sig2 = data2->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        auto sig3 = data3->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        {
            auto testService = this->registerService<service::ut::ISTest>("sight::service::ut::STest3InoutsV2");
            testService->setInOut(data1, "data1", true);
            testService->setInOut(data2, "data2", false);
            testService->setInOut(data3, "data3", true);
            testService->start().wait();

            CPPUNIT_ASSERT(!testService->getIsUpdated());
            CPPUNIT_ASSERT(!testService->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(testService->getIsUpdated());
            sig2->emit();
            CPPUNIT_ASSERT(!testService->getReceived());
            sig3->emit();
            CPPUNIT_ASSERT(testService->getReceived());
        }

        {
            auto testService = this->registerService<service::ut::ISTest>("sight::service::ut::STest3InoutsV2");
            testService->setInOut(data1, "data1", true);
            testService->setInOut(data2, "data2", true);
            testService->setInOut(data3, "data3");
            testService->start().wait();

            CPPUNIT_ASSERT(!testService->getIsUpdated());
            CPPUNIT_ASSERT(!testService->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(testService->getIsUpdated());
            sig3->emit();
            CPPUNIT_ASSERT(!testService->getReceived());
            sig2->emit();
            CPPUNIT_ASSERT(testService->getReceived());
        }

        {
            // same test but with input instead of inout
            auto testService = this->registerService<service::ut::ISTest>("sight::service::ut::STest3InoutsV2");
            testService->setInOut(data1, "data1", true);
            testService->setInOut(data2, "data2", true);
            testService->setInOut(data3, "data3");
            testService->start().wait();

            CPPUNIT_ASSERT(!testService->getIsUpdated());
            CPPUNIT_ASSERT(!testService->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(testService->getIsUpdated());
            sig3->emit();
            CPPUNIT_ASSERT(!testService->getReceived());
            sig2->emit();
            CPPUNIT_ASSERT(testService->getReceived());
        }

        // The destructor of service::IHasServices would assert if unregister is not done properly
        // So if the test passes, that means we are ok with the unregistering
        this->unregisterServices();
    }

    //------------------------------------------------------------------------------

    void testRegistration()
    {
        data::Boolean::sptr data1 = data::Boolean::New();

        {
            service::IService::wptr refService1;
            {
                auto testService1 = this->registerService("sight::service::ut::STest1Inout");
                testService1->setInOut(data1, "data1", true);
                testService1->start().wait();
                refService1 = testService1;
            }

            service::IService::wptr refService2;
            {
                auto testService2 = this->registerService("sight::service::ut::STestNoData");
                testService2->start().wait();
                refService2 = testService2;
            }

            CPPUNIT_ASSERT(!refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());
            CPPUNIT_ASSERT(refService1.lock()->getInOut<data::Boolean>("data1").lock() == data1);

            // The destructor of service::IHasServices would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterService(refService1.lock()->getID());
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            this->unregisterService(refService2.lock()->getID());
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(refService2.expired());
        }
        {
            service::IService::wptr refService1;
            {
                auto testService1 = this->registerService("sight::service::ut::STest1Input");
                testService1->setInput(data1, "data1", true);
                testService1->start().wait();
                refService1 = testService1;
            }

            service::IService::wptr refService2;
            {
                auto testService2 = this->registerService("sight::service::ut::STestNoData");
                testService2->start().wait();
                refService2 = testService2;
            }

            CPPUNIT_ASSERT(!refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());
            CPPUNIT_ASSERT(refService1.lock()->getInput<data::Boolean>("data1").lock() == data1);

            // The destructor of service::IHasServices would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterService(refService1.lock());
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            this->unregisterService(refService2.lock());
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(refService2.expired());
        }
        {
            service::IService::wptr refService1;
            {
                auto testService1 = this->registerService("sight::service::ut::STest1Inout");
                testService1->setInOut(data1, "data1", true);
                testService1->start().wait();
                refService1 = testService1;
            }

            service::IService::wptr refService2;
            {
                auto testService2 = this->registerService("sight::service::ut::STestNoData");
                testService2->start().wait();
                refService2 = testService2;
            }

            CPPUNIT_ASSERT(!refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            // The destructor of service::IHasServices would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterServices("sight::service::ut::STest1Inout");
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(!refService2.expired());

            this->unregisterServices("sight::service::ut::STestNoData");
            CPPUNIT_ASSERT(refService1.expired());
            CPPUNIT_ASSERT(refService2.expired());
        }
        {
            auto testService1 = this->registerService("sight::service::ut::STest1Inout");
            testService1->setInOut(data1, "data1", true);
            testService1->start().wait();

            auto testService2 = this->registerService("sight::service::ut::STestNoData");
            testService2->start().wait();

            auto testService3 = this->registerService("sight::service::ut::STest1Inout");
            testService3->setInOut(data1, "data1", true);
            testService3->start().wait();

            // The destructor of service::IHasServices would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterServices("sight::service::ut::STest1Inout");
            this->unregisterServices("sight::service::ut::STestNoData");

            CPPUNIT_ASSERT_EQUAL(std::size_t(0), this->getRegisteredServices().size());
        }
    }

    //------------------------------------------------------------------------------

    void testOptionalInputs()
    {
        data::Boolean::sptr data1 = data::Boolean::New();
        data::Boolean::sptr data2 = data::Boolean::New();
        data::Boolean::sptr data3 = data::Boolean::New();

        {
            auto testService = this->registerService<sight::service::ut::ISTest>(
                "sight::service::ut::STest1Input1OptInput1OptInOut"
            );
            testService->setInput(data1, "data1", true, false);
            testService->start().wait();

            CPPUNIT_ASSERT(testService->getSwappedObjectKey().empty());
            CPPUNIT_ASSERT(nullptr == testService->getSwappedObject());

            testService->setInput(data2, "data2");
            testService->swapKey("data2", nullptr).wait();
            CPPUNIT_ASSERT_EQUAL(std::string("data2"), testService->getSwappedObjectKey());
            CPPUNIT_ASSERT(data2 == testService->getSwappedObject());

            testService->setInOut(data3, "data3");
            testService->swapKey("data3", nullptr).wait();
            CPPUNIT_ASSERT_EQUAL(std::string("data3"), testService->getSwappedObjectKey());

            testService->setInput(nullptr, "data2");
            testService->swapKey("data2", nullptr).wait();
            CPPUNIT_ASSERT_EQUAL(std::string("data2"), testService->getSwappedObjectKey());
            CPPUNIT_ASSERT(nullptr == testService->getSwappedObject());
        }

        auto sig1 = data1->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        auto sig2 = data2->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        auto sig3 = data3->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);

        {
            auto testService = this->registerService<sight::service::ut::ISTest>(
                "sight::service::ut::STest1Input1OptInput1OptInOut"
            );
            testService->setInput(data1, "data1", true, false);
            testService->start().wait();

            CPPUNIT_ASSERT(!testService->getIsUpdated());
            CPPUNIT_ASSERT(!testService->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(testService->getIsUpdated());

            testService->setInput(data2, "data2", false, true);
            testService->swapKey("data2", nullptr).wait();

            sig2->emit();
            CPPUNIT_ASSERT(!testService->getReceived());

            testService->setInOut(data3, "data3", true, true);
            testService->swapKey("data3", data3).wait();

            sig3->emit();
            CPPUNIT_ASSERT(testService->getReceived());
        }

        this->unregisterServices();
    }
};

//------------------------------------------------------------------------------

void IHasServicesTest::testRegistration()
{
    TestIHasServices testHelper;
    testHelper.testRegistration();
}

//------------------------------------------------------------------------------

void IHasServicesTest::testConnection()
{
    TestIHasServices testHelper;
    testHelper.testConnection();
}

//------------------------------------------------------------------------------

void IHasServicesTest::testOptionalInputs()
{
    TestIHasServices testHelper;
    testHelper.testOptionalInputs();
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
