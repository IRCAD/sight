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
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::has_services_test);

//------------------------------------------------------------------------------

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void has_services_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void has_services_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

struct testhas_services : public service::has_services
{
    ~testhas_services() noexcept override
    {
        // just in case a test does not pass, we unregister all the services to avoid a crash
        this->unregister_services();
    }

    //------------------------------------------------------------------------------

    void test_connection()
    {
        data::boolean::sptr data1 = std::make_shared<data::boolean>();
        data::boolean::sptr data2 = std::make_shared<data::boolean>();
        data::boolean::sptr data3 = std::make_shared<data::boolean>();

        auto sig1 = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        auto sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        auto sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        {
            auto test_service = this->register_service<service::ut::test_srv>("sight::service::ut::test3_inouts_v2");
            test_service->set_inout(data1, "data1", true);
            test_service->set_inout(data2, "data2", false);
            test_service->set_inout(data3, "data3", true);
            test_service->start().wait();

            CPPUNIT_ASSERT(!test_service->get_is_updated());
            CPPUNIT_ASSERT(!test_service->get_received());

            sig1->emit();
            CPPUNIT_ASSERT(test_service->get_is_updated());
            sig2->emit();
            CPPUNIT_ASSERT(!test_service->get_received());
            sig3->emit();
            CPPUNIT_ASSERT(test_service->get_received());
        }

        {
            auto test_service = this->register_service<service::ut::test_srv>("sight::service::ut::test3_inouts_v2");
            test_service->set_inout(data1, "data1", true);
            test_service->set_inout(data2, "data2", true);
            test_service->set_inout(data3, "data3");
            test_service->start().wait();

            CPPUNIT_ASSERT(!test_service->get_is_updated());
            CPPUNIT_ASSERT(!test_service->get_received());

            sig1->emit();
            CPPUNIT_ASSERT(test_service->get_is_updated());
            sig3->emit();
            CPPUNIT_ASSERT(!test_service->get_received());
            sig2->emit();
            CPPUNIT_ASSERT(test_service->get_received());
        }

        {
            // same test but with input instead of inout
            auto test_service = this->register_service<service::ut::test_srv>("sight::service::ut::test3_inouts_v2");
            test_service->set_inout(data1, "data1", true);
            test_service->set_inout(data2, "data2", true);
            test_service->set_inout(data3, "data3");
            test_service->start().wait();

            CPPUNIT_ASSERT(!test_service->get_is_updated());
            CPPUNIT_ASSERT(!test_service->get_received());

            sig1->emit();
            CPPUNIT_ASSERT(test_service->get_is_updated());
            sig3->emit();
            CPPUNIT_ASSERT(!test_service->get_received());
            sig2->emit();
            CPPUNIT_ASSERT(test_service->get_received());
        }

        // The destructor of service::has_services would assert if unregister is not done properly
        // So if the test passes, that means we are ok with the unregistering
        this->unregister_services();
    }

    //------------------------------------------------------------------------------

    void test_registration()
    {
        data::boolean::sptr data1 = std::make_shared<data::boolean>();

        {
            service::base::wptr ref_service1;
            {
                auto test_service1 = this->register_service("sight::service::ut::test1_inout");
                test_service1->set_inout(data1, "data1", true);
                test_service1->start().wait();
                ref_service1 = test_service1;
            }

            service::base::wptr ref_service2;
            {
                auto test_service2 = this->register_service("sight::service::ut::test_no_data");
                test_service2->start().wait();
                ref_service2 = test_service2;
            }

            CPPUNIT_ASSERT(!ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());
            CPPUNIT_ASSERT(ref_service1.lock()->inout<data::boolean>("data1").lock() == data1);

            // The destructor of service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregister_service(ref_service1.lock()->get_id());
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());

            this->unregister_service(ref_service2.lock()->get_id());
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(ref_service2.expired());
        }
        {
            service::base::wptr ref_service1;
            {
                auto test_service1 = this->register_service("sight::service::ut::test1_input");
                test_service1->set_input(data1, "data1", true);
                test_service1->start().wait();
                ref_service1 = test_service1;
            }

            service::base::wptr ref_service2;
            {
                auto test_service2 = this->register_service("sight::service::ut::test_no_data");
                test_service2->start().wait();
                ref_service2 = test_service2;
            }

            CPPUNIT_ASSERT(!ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());
            CPPUNIT_ASSERT(ref_service1.lock()->input<data::boolean>("data1").lock() == data1);

            // The destructor of service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregister_service(ref_service1.lock());
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());

            this->unregister_service(ref_service2.lock());
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(ref_service2.expired());
        }
        {
            service::base::wptr ref_service1;
            {
                auto test_service1 = this->register_service("sight::service::ut::test1_inout");
                test_service1->set_inout(data1, "data1", true);
                test_service1->start().wait();
                ref_service1 = test_service1;
            }

            service::base::wptr ref_service2;
            {
                auto test_service2 = this->register_service("sight::service::ut::test_no_data");
                test_service2->start().wait();
                ref_service2 = test_service2;
            }

            CPPUNIT_ASSERT(!ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());

            // The destructor of service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregister_services("sight::service::ut::test1_inout");
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(!ref_service2.expired());

            this->unregister_services("sight::service::ut::test_no_data");
            CPPUNIT_ASSERT(ref_service1.expired());
            CPPUNIT_ASSERT(ref_service2.expired());
        }
        {
            auto test_service1 = this->register_service("sight::service::ut::test1_inout");
            test_service1->set_inout(data1, "data1", true);
            test_service1->start().wait();

            auto test_service2 = this->register_service("sight::service::ut::test_no_data");
            test_service2->start().wait();

            auto test_service3 = this->register_service("sight::service::ut::test1_inout");
            test_service3->set_inout(data1, "data1", true);
            test_service3->start().wait();

            // The destructor of service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregister_services("sight::service::ut::test1_inout");
            this->unregister_services("sight::service::ut::test_no_data");

            CPPUNIT_ASSERT_EQUAL(std::size_t(0), this->get_registered_services().size());
        }
    }

    //------------------------------------------------------------------------------

    void test_optional_inputs()
    {
        data::boolean::sptr data1 = std::make_shared<data::boolean>();
        data::boolean::sptr data2 = std::make_shared<data::boolean>();
        data::boolean::sptr data3 = std::make_shared<data::boolean>();

        {
            auto test_service = this->register_service<sight::service::ut::test_srv>(
                "sight::service::ut::test1_input1_opt_input1_opt_in_out"
            );
            test_service->set_input(data1, "data1", true, false);
            test_service->start().wait();

            CPPUNIT_ASSERT(test_service->get_swapped_object_key().empty());
            CPPUNIT_ASSERT(nullptr == test_service->get_swapped_object());

            test_service->set_input(data2, "data2");
            test_service->swap_key("data2", nullptr).wait();
            CPPUNIT_ASSERT_EQUAL(std::string("data2"), test_service->get_swapped_object_key());
            CPPUNIT_ASSERT(data2 == test_service->get_swapped_object());

            test_service->set_inout(data3, "data3");
            test_service->swap_key("data3", nullptr).wait();
            CPPUNIT_ASSERT_EQUAL(std::string("data3"), test_service->get_swapped_object_key());

            test_service->set_input(nullptr, "data2");
            test_service->swap_key("data2", nullptr).wait();
            CPPUNIT_ASSERT_EQUAL(std::string("data2"), test_service->get_swapped_object_key());
            CPPUNIT_ASSERT(nullptr == test_service->get_swapped_object());
        }

        auto sig1 = data1->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        auto sig2 = data2->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        auto sig3 = data3->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);

        {
            auto test_service = this->register_service<sight::service::ut::test_srv>(
                "sight::service::ut::test1_input1_opt_input1_opt_in_out"
            );
            test_service->set_input(data1, "data1", true, false);
            test_service->start().wait();

            CPPUNIT_ASSERT(!test_service->get_is_updated());
            CPPUNIT_ASSERT(!test_service->get_received());

            sig1->emit();
            CPPUNIT_ASSERT(test_service->get_is_updated());

            test_service->set_input(data2, "data2", false, true);
            test_service->swap_key("data2", nullptr).wait();

            sig2->emit();
            CPPUNIT_ASSERT(!test_service->get_received());

            test_service->set_inout(data3, "data3", true, true);
            test_service->swap_key("data3", data3).wait();

            sig3->emit();
            CPPUNIT_ASSERT(test_service->get_received());
        }

        this->unregister_services();
    }
};

//------------------------------------------------------------------------------

void has_services_test::test_registration()
{
    testhas_services test_helper;
    test_helper.test_registration();
}

//------------------------------------------------------------------------------

void has_services_test::test_connection()
{
    testhas_services test_helper;
    test_helper.test_connection();
}

//------------------------------------------------------------------------------

void has_services_test::test_optional_inputs()
{
    testhas_services test_helper;
    test_helper.test_optional_inputs();
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
