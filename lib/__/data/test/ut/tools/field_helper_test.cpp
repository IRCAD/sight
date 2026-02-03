/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>

#include <data/helper/field.hpp>
#include <data/string.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::tools::ut")
{
//------------------------------------------------------------------------------

    TEST_CASE("test_helper")
    {
        const std::string field_i_d1 = "FIELD_ID1";
        const std::string field_i_d2 = "FIELD_ID2";
        const std::string field_i_d3 = "FIELD_ID3";

        sight::data::object::sptr nullobj;
        sight::data::object::sptr obj        = std::make_shared<sight::data::string>();
        sight::data::object::sptr field_obj1 = std::make_shared<sight::data::string>();
        sight::data::object::sptr field_obj2 = std::make_shared<sight::data::string>();
        sight::data::object::sptr field_obj3 = std::make_shared<sight::data::string>();

        sight::core::thread::worker::sptr worker = sight::core::thread::worker::make();

        // Setup to test notifications
        unsigned int num_added_notif = 0;
        std::mutex mutex;
        std::condition_variable condition;

        sight::data::fields_container_t added_fields;
        std::function<void(sight::data::fields_container_t)> fn_add =
            [&](sight::data::fields_container_t _f)
            {
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    ++num_added_notif;
                    added_fields = _f;
                }
                condition.notify_one();
            };

        auto slot_added = sight::core::com::new_slot(fn_add);
        slot_added->set_worker(worker);
        auto sig_added = obj->signal<sight::data::object::added_fields_signal_t>(sight::data::object::ADDED_FIELDS_SIG);
        sig_added->connect(slot_added);

        unsigned int num_removed_notif = 0;
        sight::data::fields_container_t removed_fields;

        std::function<void(sight::data::fields_container_t)> fn_remove =
            [&](sight::data::fields_container_t _f)
            {
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    ++num_removed_notif;
                    removed_fields = _f;
                }
                condition.notify_one();
            };
        auto slot_removed = sight::core::com::new_slot(fn_remove);
        slot_removed->set_worker(worker);
        auto sig_removed =
            obj->signal<sight::data::object::removed_fields_signal_t>(sight::data::object::REMOVED_FIELDS_SIG);
        sig_removed->connect(slot_removed);

        unsigned int num_changed_notif = 0;
        sight::data::fields_container_t new_fields;
        sight::data::fields_container_t old_fields;

        std::function<void(sight::data::fields_container_t, sight::data::fields_container_t)> fn_change =
            [&](sight::data::fields_container_t _new_f, sight::data::fields_container_t _old_f)
            {
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    ++num_changed_notif;
                    new_fields = _new_f;
                    old_fields = _old_f;
                }
                condition.notify_one();
            };
        auto slot_changed = sight::core::com::new_slot(fn_change);
        slot_changed->set_worker(worker);
        auto sig_changed =
            obj->signal<sight::data::object::changed_fields_signal_t>(sight::data::object::CHANGED_FIELDS_SIG);
        sig_changed->connect(slot_changed);

        auto clear_arrays = [&]()
                            {
                                added_fields.clear();
                                removed_fields.clear();
                                new_fields.clear();
                                old_fields.clear();
                            };

        {
            // Test set_field()
            sight::data::helper::field field_helper(obj);
            field_helper.set_field(field_i_d1, field_obj1);
            field_helper.set_field(field_i_d2, field_obj2);
            CHECK_EQ(std::size_t(2), obj->get_fields().size());
            CHECK(obj->get_field(field_i_d1) == field_obj1);
            CHECK(obj->get_field(field_i_d2) == field_obj2);
            CHECK(obj->get_field(field_i_d3) == nullobj);
        }
        {
            // Check notification
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [&]{return num_added_notif == 1;});

            CHECK_EQ(std::size_t(2), added_fields.size());
            CHECK_EQ(std::size_t(0), removed_fields.size());
            CHECK_EQ(std::size_t(0), new_fields.size());
            CHECK_EQ(std::size_t(0), old_fields.size());
            CHECK(added_fields[field_i_d1] == field_obj1);
            CHECK(added_fields[field_i_d2] == field_obj2);
            clear_arrays();
        }

        {
            // Test set_fields()
            sight::data::object::field_map_t fields_with_obj1 = {{field_i_d1, field_obj3}};
            sight::data::helper::field field_helper(obj);
            field_helper.set_fields(fields_with_obj1);
            CHECK_EQ(std::size_t(1), obj->get_fields().size());
            CHECK(obj->get_field(field_i_d1) == field_obj3);
            CHECK(obj->get_field(field_i_d2) == nullobj);
            CHECK(obj->get_field(field_i_d3) == nullobj);
        }
        {
            // Check notification
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [&]{return num_removed_notif == 1 && num_changed_notif == 1;});

            CHECK_EQ(std::size_t(0), added_fields.size());
            CHECK_EQ(std::size_t(1), removed_fields.size());
            CHECK_EQ(std::size_t(1), new_fields.size());
            CHECK_EQ(std::size_t(1), old_fields.size());
            CHECK(removed_fields[field_i_d2] == field_obj2);
            CHECK(old_fields[field_i_d1] == field_obj1);
            CHECK(new_fields[field_i_d1] == field_obj3);
            clear_arrays();
        }

        {
            // Test replacement with set_field()
            sight::data::helper::field field_helper(obj);
            field_helper.set_field(field_i_d1, field_obj2);
            CHECK_EQ(std::size_t(1), obj->get_fields().size());
            CHECK(obj->get_field(field_i_d1) == field_obj2);
            CHECK(obj->get_field(field_i_d2) == nullobj);
            CHECK(obj->get_field(field_i_d3) == nullobj);

            // Notify explicitly, this should change nothing since the destructor skip it in this case
            field_helper.notify();
        }
        {
            // Check notification
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [&]{return num_changed_notif == 2;});

            CHECK_EQ(std::size_t(0), added_fields.size());
            CHECK_EQ(std::size_t(0), removed_fields.size());
            CHECK_EQ(std::size_t(1), new_fields.size());
            CHECK_EQ(std::size_t(1), old_fields.size());
            CHECK(old_fields[field_i_d1] == field_obj3);
            CHECK(new_fields[field_i_d1] == field_obj2);
            clear_arrays();
        }

        {
            // Test add(), addOrSwap() and remove()
            sight::data::helper::field field_helper(obj);
            field_helper.add(field_i_d2, field_obj1);
            CHECK_EQ(std::size_t(2), obj->get_fields().size());
            CHECK(obj->get_field(field_i_d1) == field_obj2);
            CHECK(obj->get_field(field_i_d2) == field_obj1);
            CHECK(obj->get_field(field_i_d3) == nullobj);

            field_helper.add_or_swap(field_i_d2, field_obj3);
            CHECK_EQ(std::size_t(2), obj->get_fields().size());
            CHECK(obj->get_field(field_i_d1) == field_obj2);
            CHECK(obj->get_field(field_i_d2) == field_obj3);
            CHECK(obj->get_field(field_i_d3) == nullobj);

            field_helper.remove(field_i_d1);
            CHECK_EQ(std::size_t(1), obj->get_fields().size());
            CHECK(obj->get_field(field_i_d1) == nullobj);
            CHECK(obj->get_field(field_i_d2) == field_obj3);
            CHECK(obj->get_field(field_i_d3) == nullobj);

            field_helper.add(field_i_d3, field_obj1);
            CHECK_EQ(std::size_t(2), obj->get_fields().size());
            CHECK(obj->get_field(field_i_d1) == nullobj);
            CHECK(obj->get_field(field_i_d2) == field_obj3);
            CHECK(obj->get_field(field_i_d3) == field_obj1);
        }
        {
            // Check notification
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [&]{return num_added_notif == 2 && num_removed_notif == 2 && num_changed_notif == 3;});

            CHECK_EQ(std::size_t(2), added_fields.size());
            CHECK_EQ(std::size_t(1), removed_fields.size());
            CHECK_EQ(std::size_t(1), new_fields.size());
            CHECK_EQ(std::size_t(1), old_fields.size());
            CHECK(added_fields[field_i_d2] == field_obj1);
            CHECK(added_fields[field_i_d3] == field_obj1);
            CHECK(old_fields[field_i_d2] == field_obj1);
            CHECK(new_fields[field_i_d2] == field_obj3);
            CHECK(removed_fields[field_i_d1] == field_obj2);
            clear_arrays();
        }

        {
            // Test clear()
            sight::data::helper::field field_helper(obj);
            field_helper.clear();
            CHECK_EQ(std::size_t(0), obj->get_fields().size());
            CHECK(obj->get_field(field_i_d1) == nullobj);
            CHECK(obj->get_field(field_i_d2) == nullobj);
            CHECK(obj->get_field(field_i_d3) == nullobj);
        }
        {
            // Check notification
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [&]{return num_removed_notif == 3;});

            CHECK_EQ(std::size_t(2), removed_fields.size());
            CHECK(removed_fields[field_i_d2] == field_obj3);
            CHECK(removed_fields[field_i_d3] == field_obj1);
            clear_arrays();
        }

        worker->stop();
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::tools::ut")
