/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "field_helper_test.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>

#include <data/helper/field.hpp>
#include <data/string.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::field_helper_test);

namespace sight::data::tools::ut
{

//------------------------------------------------------------------------------

void field_helper_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void field_helper_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void field_helper_test::test_helper()
{
    const std::string field_i_d1 = "FIELD_ID1";
    const std::string field_i_d2 = "FIELD_ID2";
    const std::string field_i_d3 = "FIELD_ID3";

    data::object::sptr nullobj;
    data::object::sptr obj        = std::make_shared<data::string>();
    data::object::sptr field_obj1 = std::make_shared<data::string>();
    data::object::sptr field_obj2 = std::make_shared<data::string>();
    data::object::sptr field_obj3 = std::make_shared<data::string>();

    core::thread::worker::sptr worker = core::thread::worker::make();

    // Setup to test notifications
    unsigned int num_added_notif = 0;
    std::mutex mutex;
    std::condition_variable condition;

    data::fields_container_t added_fields;
    std::function<void(data::fields_container_t)> fn_add =
        [&](data::fields_container_t _f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++num_added_notif;
                added_fields = _f;
            }
            condition.notify_one();
        };

    auto slot_added = core::com::new_slot(fn_add);
    slot_added->set_worker(worker);
    auto sig_added = obj->signal<data::object::added_fields_signal_t>(data::object::ADDED_FIELDS_SIG);
    sig_added->connect(slot_added);

    unsigned int num_removed_notif = 0;
    data::fields_container_t removed_fields;

    std::function<void(data::fields_container_t)> fn_remove =
        [&](data::fields_container_t _f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++num_removed_notif;
                removed_fields = _f;
            }
            condition.notify_one();
        };
    auto slot_removed = core::com::new_slot(fn_remove);
    slot_removed->set_worker(worker);
    auto sig_removed = obj->signal<data::object::removed_fields_signal_t>(data::object::REMOVED_FIELDS_SIG);
    sig_removed->connect(slot_removed);

    unsigned int num_changed_notif = 0;
    data::fields_container_t new_fields;
    data::fields_container_t old_fields;

    std::function<void(data::fields_container_t, data::fields_container_t)> fn_change =
        [&](data::fields_container_t _new_f, data::fields_container_t _old_f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++num_changed_notif;
                new_fields = _new_f;
                old_fields = _old_f;
            }
            condition.notify_one();
        };
    auto slot_changed = core::com::new_slot(fn_change);
    slot_changed->set_worker(worker);
    auto sig_changed = obj->signal<data::object::changed_fields_signal_t>(data::object::CHANGED_FIELDS_SIG);
    sig_changed->connect(slot_changed);

    auto clear_arrays = [&](){added_fields.clear(); removed_fields.clear(); new_fields.clear(); old_fields.clear();};

    {
        // Test set_field()
        data::helper::field field_helper(obj);
        field_helper.set_field(field_i_d1, field_obj1);
        field_helper.set_field(field_i_d2, field_obj2);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), obj->get_fields().size());
        CPPUNIT_ASSERT(obj->get_field(field_i_d1) == field_obj1);
        CPPUNIT_ASSERT(obj->get_field(field_i_d2) == field_obj2);
        CPPUNIT_ASSERT(obj->get_field(field_i_d3) == nullobj);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return num_added_notif == 1;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(2), added_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), removed_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), new_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), old_fields.size());
        CPPUNIT_ASSERT(added_fields[field_i_d1] == field_obj1);
        CPPUNIT_ASSERT(added_fields[field_i_d2] == field_obj2);
        clear_arrays();
    }

    {
        // Test set_fields()
        data::object::field_map_t fields_with_obj1 = {{field_i_d1, field_obj3}};
        data::helper::field field_helper(obj);
        field_helper.set_fields(fields_with_obj1);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), obj->get_fields().size());
        CPPUNIT_ASSERT(obj->get_field(field_i_d1) == field_obj3);
        CPPUNIT_ASSERT(obj->get_field(field_i_d2) == nullobj);
        CPPUNIT_ASSERT(obj->get_field(field_i_d3) == nullobj);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return num_removed_notif == 1 && num_changed_notif == 1;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(0), added_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), removed_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), new_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), old_fields.size());
        CPPUNIT_ASSERT(removed_fields[field_i_d2] == field_obj2);
        CPPUNIT_ASSERT(old_fields[field_i_d1] == field_obj1);
        CPPUNIT_ASSERT(new_fields[field_i_d1] == field_obj3);
        clear_arrays();
    }

    {
        // Test replacement with set_field()
        data::helper::field field_helper(obj);
        field_helper.set_field(field_i_d1, field_obj2);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), obj->get_fields().size());
        CPPUNIT_ASSERT(obj->get_field(field_i_d1) == field_obj2);
        CPPUNIT_ASSERT(obj->get_field(field_i_d2) == nullobj);
        CPPUNIT_ASSERT(obj->get_field(field_i_d3) == nullobj);

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        field_helper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return num_changed_notif == 2;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(0), added_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), removed_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), new_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), old_fields.size());
        CPPUNIT_ASSERT(old_fields[field_i_d1] == field_obj3);
        CPPUNIT_ASSERT(new_fields[field_i_d1] == field_obj2);
        clear_arrays();
    }

    {
        // Test add(), addOrSwap() and remove()
        data::helper::field field_helper(obj);
        field_helper.add(field_i_d2, field_obj1);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), obj->get_fields().size());
        CPPUNIT_ASSERT(obj->get_field(field_i_d1) == field_obj2);
        CPPUNIT_ASSERT(obj->get_field(field_i_d2) == field_obj1);
        CPPUNIT_ASSERT(obj->get_field(field_i_d3) == nullobj);

        field_helper.add_or_swap(field_i_d2, field_obj3);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), obj->get_fields().size());
        CPPUNIT_ASSERT(obj->get_field(field_i_d1) == field_obj2);
        CPPUNIT_ASSERT(obj->get_field(field_i_d2) == field_obj3);
        CPPUNIT_ASSERT(obj->get_field(field_i_d3) == nullobj);

        field_helper.remove(field_i_d1);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), obj->get_fields().size());
        CPPUNIT_ASSERT(obj->get_field(field_i_d1) == nullobj);
        CPPUNIT_ASSERT(obj->get_field(field_i_d2) == field_obj3);
        CPPUNIT_ASSERT(obj->get_field(field_i_d3) == nullobj);

        field_helper.add(field_i_d3, field_obj1);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), obj->get_fields().size());
        CPPUNIT_ASSERT(obj->get_field(field_i_d1) == nullobj);
        CPPUNIT_ASSERT(obj->get_field(field_i_d2) == field_obj3);
        CPPUNIT_ASSERT(obj->get_field(field_i_d3) == field_obj1);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return num_added_notif == 2 && num_removed_notif == 2 && num_changed_notif == 3;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(2), added_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), removed_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), new_fields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), old_fields.size());
        CPPUNIT_ASSERT(added_fields[field_i_d2] == field_obj1);
        CPPUNIT_ASSERT(added_fields[field_i_d3] == field_obj1);
        CPPUNIT_ASSERT(old_fields[field_i_d2] == field_obj1);
        CPPUNIT_ASSERT(new_fields[field_i_d2] == field_obj3);
        CPPUNIT_ASSERT(removed_fields[field_i_d1] == field_obj2);
        clear_arrays();
    }

    {
        // Test clear()
        data::helper::field field_helper(obj);
        field_helper.clear();
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), obj->get_fields().size());
        CPPUNIT_ASSERT(obj->get_field(field_i_d1) == nullobj);
        CPPUNIT_ASSERT(obj->get_field(field_i_d2) == nullobj);
        CPPUNIT_ASSERT(obj->get_field(field_i_d3) == nullobj);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return num_removed_notif == 3;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(2), removed_fields.size());
        CPPUNIT_ASSERT(removed_fields[field_i_d2] == field_obj3);
        CPPUNIT_ASSERT(removed_fields[field_i_d3] == field_obj1);
        clear_arrays();
    }

    worker->stop();
}

//------------------------------------------------------------------------------

} // namespace sight::data::tools::ut
