/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "has_services_test.hpp"

#include "test_service.hpp"

#include <data/boolean.hpp>

#include <service/has_services.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::has_servicesTest);

//------------------------------------------------------------------------------

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void has_servicesTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void has_servicesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

struct Testhas_services : public service::has_services
{
    ~Testhas_services() noexcept override
    {
        // just in case a test does not pass, we unregister all the services to avoid a crash
        this->unregisterServices();
    }

    //------------------------------------------------------------------------------

    void testConnection()
    {
        data::boolean::sptr data1 = std::make_shared<data::boolean>();
        data::boolean::sptr data2 = std::make_shared<data::boolean>();
        data::boolean::sptr data3 = std::make_shared<data::boolean>();

        auto sig1 = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        auto sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        auto sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        {
            auto test_service = this->registerService<service::ut::test_srv>("sight::service::ut::STest3InoutsV2");
            test_service->set_inout(data1, "data1", true);
            test_service->set_inout(data2, "data2", false);
            test_service->set_inout(data3, "data3", true);
            test_service->start().wait();

            CPPUNIT_ASSERT(!test_service->getIsUpdated());
            CPPUNIT_ASSERT(!test_service->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(test_service->getIsUpdated());
            sig2->emit();
            CPPUNIT_ASSERT(!test_service->getReceived());
            sig3->emit();
            CPPUNIT_ASSERT(test_service->getReceived());
        }

        {
            auto test_service = this->registerService<service::ut::test_srv>("sight::service::ut::STest3InoutsV2");
            test_service->set_inout(data1, "data1", true);
            test_service->set_inout(data2, "data2", true);
            test_service->set_inout(data3, "data3");
            test_service->start().wait();

            CPPUNIT_ASSERT(!test_service->getIsUpdated());
            CPPUNIT_ASSERT(!test_service->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(test_service->getIsUpdated());
            sig3->emit();
            CPPUNIT_ASSERT(!test_service->getReceived());
            sig2->emit();
            CPPUNIT_ASSERT(test_service->getReceived());
        }

        {
            // same test but with input instead of inout
            auto test_service = this->registerService<service::ut::test_srv>("sight::service::ut::STest3InoutsV2");
            test_service->set_inout(data1, "data1", true);
            test_service->set_inout(data2, "data2", true);
            test_service->set_inout(data3, "data3");
            test_service->start().wait();

            CPPUNIT_ASSERT(!test_service->getIsUpdated());
            CPPUNIT_ASSERT(!test_service->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(test_service->getIsUpdated());
            sig3->emit();
            CPPUNIT_ASSERT(!test_service->getReceived());
            sig2->emit();
            CPPUNIT_ASSERT(test_service->getReceived());
        }

        // The destructor of service::has_services would assert if unregister is not done properly
        // So if the test passes, that means we are ok with the unregistering
        this->unregisterServices();
    }

    //------------------------------------------------------------------------------

    void testRegistration()
    {
        data::boolean::sptr data1 = std::make_shared<data::boolean>();

        {
            service::base::wptr ref_service1;
            {
                auto test_service1 = this->registerService("sight::service::ut::STest1Inout");
                test_service1->set_inout(data1, "data1", true);
                test_service1->start().wait();
                ref_service1 = test_service1;
            }

            service::base::wptr ref_service2;
            {
                auto test_service2 = this->registerService("sight::service::ut::STestNoData");
                test_service2->start().wait();
                ref_service2 = test_service2;
            }

            CPPUNIT_ASSERT(!ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());
            CPPUNIT_ASSERT(ref_service1.lock()->inout<data::boolean>("data1").lock() == data1);

            // The destructor of service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterService(ref_service1.lock()->get_id());
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());

            this->unregisterService(ref_service2.lock()->get_id());
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(ref_service2.expired());
        }
        {
            service::base::wptr ref_service1;
            {
                auto test_service1 = this->registerService("sight::service::ut::STest1Input");
                test_service1->set_input(data1, "data1", true);
                test_service1->start().wait();
                ref_service1 = test_service1;
            }

            service::base::wptr ref_service2;
            {
                auto test_service2 = this->registerService("sight::service::ut::STestNoData");
                test_service2->start().wait();
                ref_service2 = test_service2;
            }

            CPPUNIT_ASSERT(!ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());
            CPPUNIT_ASSERT(ref_service1.lock()->input<data::boolean>("data1").lock() == data1);

            // The destructor of service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterService(ref_service1.lock());
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());

            this->unregisterService(ref_service2.lock());
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(ref_service2.expired());
        }
        {
            service::base::wptr ref_service1;
            {
                auto test_service1 = this->registerService("sight::service::ut::STest1Inout");
                test_service1->set_inout(data1, "data1", true);
                test_service1->start().wait();
                ref_service1 = test_service1;
            }

            service::base::wptr ref_service2;
            {
                auto test_service2 = this->registerService("sight::service::ut::STestNoData");
                test_service2->start().wait();
                ref_service2 = test_service2;
            }

            CPPUNIT_ASSERT(!ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());

            // The destructor of service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterServices("sight::service::ut::STest1Inout");
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());

            this->unregisterServices("sight::service::ut::STestNoData");
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(ref_service2.expired());
        }
        {
            auto test_service1 = this->registerService("sight::service::ut::STest1Inout");
            test_service1->set_inout(data1, "data1", true);
            test_service1->start().wait();

            auto test_service2 = this->registerService("sight::service::ut::STestNoData");
            test_service2->start().wait();

            auto test_service3 = this->registerService("sight::service::ut::STest1Inout");
            test_service3->set_inout(data1, "data1", true);
            test_service3->start().wait();

            // The destructor of service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregisterServices("sight::service::ut::STest1Inout");
            this->unregisterServices("sight::service::ut::STestNoData");

            CPPUNIT_ASSERT_EQUAL(std::size_t(0), this->getRegisteredServices().size());
        }
    }

    //------------------------------------------------------------------------------

    void testOptionalInputs()
    {
        data::boolean::sptr data1 = std::make_shared<data::boolean>();
        data::boolean::sptr data2 = std::make_shared<data::boolean>();
        data::boolean::sptr data3 = std::make_shared<data::boolean>();

        {
            auto test_service = this->registerService<sight::service::ut::test_srv>(
                "sight::service::ut::STest1Input1OptInput1OptInOut"
            );
            test_service->set_input(data1, "data1", true, false);
            test_service->start().wait();

            CPPUNIT_ASSERT(test_service->getSwappedObjectKey().empty());
            CPPUNIT_ASSERT(nullptr == test_service->getSwappedObject());

            test_service->set_input(data2, "data2");
            test_service->swap_key("data2", nullptr).wait();
            CPPUNIT_ASSERT_EQUAL(std::string("data2"), test_service->getSwappedObjectKey());
            CPPUNIT_ASSERT(data2 == test_service->getSwappedObject());

            test_service->set_inout(data3, "data3");
            test_service->swap_key("data3", nullptr).wait();
            CPPUNIT_ASSERT_EQUAL(std::string("data3"), test_service->getSwappedObjectKey());

            test_service->set_input(nullptr, "data2");
            test_service->swap_key("data2", nullptr).wait();
            CPPUNIT_ASSERT_EQUAL(std::string("data2"), test_service->getSwappedObjectKey());
            CPPUNIT_ASSERT(nullptr == test_service->getSwappedObject());
        }

        auto sig1 = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        auto sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        auto sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);

        {
            auto test_service = this->registerService<sight::service::ut::test_srv>(
                "sight::service::ut::STest1Input1OptInput1OptInOut"
            );
            test_service->set_input(data1, "data1", true, false);
            test_service->start().wait();

            CPPUNIT_ASSERT(!test_service->getIsUpdated());
            CPPUNIT_ASSERT(!test_service->getReceived());

            sig1->emit();
            CPPUNIT_ASSERT(test_service->getIsUpdated());

            test_service->set_input(data2, "data2", false, true);
            test_service->swap_key("data2", nullptr).wait();

            sig2->emit();
            CPPUNIT_ASSERT(!test_service->getReceived());

            test_service->set_inout(data3, "data3", true, true);
            test_service->swap_key("data3", data3).wait();

            sig3->emit();
            CPPUNIT_ASSERT(test_service->getReceived());
        }

        this->unregisterServices();
    }
};

//------------------------------------------------------------------------------

void has_servicesTest::testRegistration()
{
    Testhas_services test_helper;
    test_helper.testRegistration();
}

//------------------------------------------------------------------------------

void has_servicesTest::testConnection()
{
    Testhas_services test_helper;
    test_helper.testConnection();
}

//------------------------------------------------------------------------------

void has_servicesTest::testOptionalInputs()
{
    Testhas_services test_helper;
    test_helper.testOptionalInputs();
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
