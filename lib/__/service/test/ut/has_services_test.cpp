/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "test_service.hpp"

#include <data/boolean.hpp>

#include <service/has_services.hpp>

#include <doctest/doctest.h>

namespace
{

struct fixture : public sight::service::has_services
{
    ~fixture() noexcept override
    {
        // just in case a test does not pass, we unregister all the services to avoid a crash
        this->unregister_services();
    }
};

} // namespace

TEST_SUITE("sight::service::has_services")
{
    TEST_CASE_FIXTURE(fixture, "test_registration")
    {
        sight::data::boolean::sptr data1 = std::make_shared<sight::data::boolean>();

        {
            sight::service::base::wptr ref_service1;
            {
                auto test_service1 = this->register_service("sight::service::ut::test1_inout");
                test_service1->set_inout(data1, "data1", true);
                test_service1->start().wait();
                ref_service1 = test_service1;
            }

            sight::service::base::wptr ref_service2;
            {
                auto test_service2 = this->register_service("sight::service::ut::test_no_data");
                test_service2->start().wait();
                ref_service2 = test_service2;
            }

            CHECK(!ref_service1.expired());
            CHECK(!ref_service2.expired());
            CHECK(ref_service1.lock()->inout<sight::data::boolean>("data1").lock() == data1);

            // The destructor of sight::service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregister_service(ref_service1.lock()->get_id());
            CHECK(ref_service1.expired());
            CHECK(!ref_service2.expired());

            this->unregister_service(ref_service2.lock()->get_id());
            CHECK(ref_service1.expired());
            CHECK(ref_service2.expired());
        }
        {
            sight::service::base::wptr ref_service1;
            {
                auto test_service1 = this->register_service("sight::service::ut::test1_input");
                test_service1->set_input(data1, "data1", true);
                test_service1->start().wait();
                ref_service1 = test_service1;
            }

            sight::service::base::wptr ref_service2;
            {
                auto test_service2 = this->register_service("sight::service::ut::test_no_data");
                test_service2->start().wait();
                ref_service2 = test_service2;
            }

            CHECK(!ref_service1.expired());
            CHECK(!ref_service2.expired());
            CHECK(ref_service1.lock()->input<sight::data::boolean>("data1").lock() == data1);

            // The destructor of sight::service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregister_service(ref_service1.lock());
            CHECK(ref_service1.expired());
            CHECK(!ref_service2.expired());

            this->unregister_service(ref_service2.lock());
            CHECK(ref_service1.expired());
            CHECK(ref_service2.expired());
        }
        {
            sight::service::base::wptr ref_service1;
            {
                auto test_service1 = this->register_service("sight::service::ut::test1_inout");
                test_service1->set_inout(data1, "data1", true);
                test_service1->start().wait();
                ref_service1 = test_service1;
            }

            sight::service::base::wptr ref_service2;
            {
                auto test_service2 = this->register_service("sight::service::ut::test_no_data");
                test_service2->start().wait();
                ref_service2 = test_service2;
            }

            CHECK(!ref_service1.expired());
            CHECK(!ref_service2.expired());

            // The destructor of sight::service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregister_services("sight::service::ut::test1_inout");
            CHECK(ref_service1.expired());
            CHECK(!ref_service2.expired());

            this->unregister_services("sight::service::ut::test_no_data");
            CHECK(ref_service1.expired());
            CHECK(ref_service2.expired());
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

            // The destructor of sight::service::has_services would assert if unregister is not done properly
            // So if the test passes, that means we are ok with the unregistering
            this->unregister_services("sight::service::ut::test1_inout");
            this->unregister_services("sight::service::ut::test_no_data");

            CHECK_EQ(std::size_t(0), this->get_registered_services().size());
        }
    }

    TEST_CASE_FIXTURE(fixture, "test_connection")
    {
        sight::data::boolean::sptr data1 = std::make_shared<sight::data::boolean>();
        sight::data::boolean::sptr data2 = std::make_shared<sight::data::boolean>();
        sight::data::boolean::sptr data3 = std::make_shared<sight::data::boolean>();

        auto sig1 = data1->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG);
        auto sig2 = data2->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG);
        auto sig3 = data3->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG);
        {
            auto test_service =
                this->register_service<sight::service::ut::test_srv>("sight::service::ut::test3_inouts_v2");
            test_service->set_inout(data1, "data1", true);
            test_service->set_inout(data2, "data2", false);
            test_service->set_inout(data3, "data3", true);
            test_service->start().wait();

            CHECK(!test_service->is_updated());
            CHECK(!test_service->get_received());

            sig1->emit();
            CHECK(test_service->is_updated());
            sig2->emit();
            CHECK(!test_service->get_received());
            sig3->emit();
            CHECK(test_service->get_received());
        }

        {
            auto test_service =
                this->register_service<sight::service::ut::test_srv>("sight::service::ut::test3_inouts_v2");
            test_service->set_inout(data1, "data1", true);
            test_service->set_inout(data2, "data2", true);
            test_service->set_inout(data3, "data3", false);
            test_service->start().wait();

            CHECK(!test_service->is_updated());
            CHECK(!test_service->get_received());

            sig1->emit();
            CHECK(test_service->is_updated());
            sig3->emit();
            CHECK(!test_service->get_received());
            sig2->emit();
            CHECK(test_service->get_received());
        }

        {
            // same test but with input instead of inout
            auto test_service =
                this->register_service<sight::service::ut::test_srv>("sight::service::ut::test3_inouts_v2");
            test_service->set_inout(data1, "data1", true);
            test_service->set_inout(data2, "data2", true);
            test_service->set_inout(data3, "data3", false);
            test_service->start().wait();

            CHECK(!test_service->is_updated());
            CHECK(!test_service->get_received());

            sig1->emit();
            CHECK(test_service->is_updated());
            sig3->emit();
            CHECK(!test_service->get_received());
            sig2->emit();
            CHECK(test_service->get_received());
        }

        // The destructor of sight::service::has_services would assert if unregister is not done properly
        // So if the test passes, that means we are ok with the unregistering
        this->unregister_services();
    }

    TEST_CASE_FIXTURE(fixture, "test_optional_inputs")
    {
        sight::data::boolean::sptr data1 = std::make_shared<sight::data::boolean>();
        sight::data::boolean::sptr data2 = std::make_shared<sight::data::boolean>();
        sight::data::boolean::sptr data3 = std::make_shared<sight::data::boolean>();

        {
            auto test_service = this->register_service<sight::service::ut::test_srv>(
                "sight::service::ut::test1_input1_opt_input1_opt_in_out"
            );
            test_service->set_input(data1, "data1", true, false);
            test_service->start().wait();

            CHECK(test_service->get_swapped_object_key().empty());
            CHECK(nullptr == test_service->get_swapped_object());

            test_service->set_input(data2, "data2");
            test_service->swap_key("data2", nullptr).wait();
            CHECK_EQ(std::string("data2"), test_service->get_swapped_object_key());
            CHECK(data2 == test_service->get_swapped_object());

            test_service->set_inout(data3, "data3");
            test_service->swap_key("data3", nullptr).wait();
            CHECK_EQ(std::string("data3"), test_service->get_swapped_object_key());

            test_service->set_input(nullptr, "data2");
            test_service->swap_key("data2", nullptr).wait();
            CHECK_EQ(std::string("data2"), test_service->get_swapped_object_key());
            CHECK(nullptr == test_service->get_swapped_object());
        }

        auto sig1 = data1->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG);
        auto sig2 = data2->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG);
        auto sig3 = data3->signal<sight::data::object::modified_signal_t>(sight::data::object::MODIFIED_SIG);

        {
            auto test_service = this->register_service<sight::service::ut::test_srv>(
                "sight::service::ut::test1_input1_opt_input1_opt_in_out"
            );
            test_service->set_input(data1, "data1", true, false);
            test_service->start().wait();

            CHECK(!test_service->is_updated());
            CHECK(!test_service->get_received());

            sig1->emit();
            CHECK(test_service->is_updated());

            test_service->set_input(data2, "data2", false, true);
            test_service->swap_key("data2", nullptr).wait();

            sig2->emit();
            CHECK(!test_service->get_received());

            test_service->set_inout(data3, "data3", true, true);
            test_service->swap_key("data3", data3).wait();

            sig3->emit();
            CHECK(test_service->get_received());
        }

        this->unregister_services();
    }
}
