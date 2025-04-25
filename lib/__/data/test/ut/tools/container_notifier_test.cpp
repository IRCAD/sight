/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

#include "container_notifier_test.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/compare.hpp>
#include <core/thread/worker.hpp>
#include <core/tools/uuid.hpp>

#include <data/activity_set.hpp>
#include <data/camera_set.hpp>
#include <data/map.hpp>
#include <data/series_set.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::container_notifier_test);

namespace sight::data::tools::ut
{

//------------------------------------------------------------------------------

template<typename T>
inline static void container_notifier_test_fn()
{
    auto container = std::make_shared<T>();

    static const auto s_DESCRIPTION1 = core::tools::uuid::generate();
    static const auto s_DESCRIPTION2 = core::tools::uuid::generate();
    static const auto s_DESCRIPTION3 = core::tools::uuid::generate();

    object::sptr object1;
    object::sptr object2;
    object::sptr object3;

    if constexpr(std::is_same_v<camera_set, T>)
    {
        object1 = std::make_shared<camera>();
        object2 = std::make_shared<camera>();
        object3 = std::make_shared<camera>();
    }
    else if constexpr(std::is_same_v<activity_set, T>)
    {
        object1 = std::make_shared<activity>();
        object2 = std::make_shared<activity>();
        object3 = std::make_shared<activity>();
    }
    else if constexpr(std::is_same_v<series_set, T>)
    {
        object1 = std::make_shared<series>();
        object2 = std::make_shared<series>();
        object3 = std::make_shared<series>();
    }
    else
    {
        object1 = std::make_shared<string>(s_DESCRIPTION1);
        object2 = std::make_shared<string>(s_DESCRIPTION1);
        object3 = std::make_shared<string>(s_DESCRIPTION1);
    }

    std::mutex mutex;
    std::condition_variable condition_variable;

    // Test add
    {
        auto worker                = core::thread::worker::make();
        std::size_t add_call_count = 0;
        typename T::container_t added_from_slot;

        std::function<void(typename T::container_t)> add =
            [&](typename T::container_t _added_from_signal)
            {
                {
                    std::unique_lock lock(mutex);
                    added_from_slot = _added_from_signal;
                    ++add_call_count;
                }
                condition_variable.notify_one();
            };

        auto added_slot = core::com::new_slot(add);
        added_slot->set_worker(worker);
        auto added_sig = container->template signal<typename T::added_signal_t>(T::ADDED_OBJECTS_SIG);
        added_sig->connect(added_slot);

        bool blocked_add_called           = false;
        std::function<void()> blocked_add =
            [&]()
            {
                {
                    std::unique_lock lock(mutex);
                    blocked_add_called = true;
                }
                condition_variable.notify_one();
            };

        auto blocked_added_slot = core::com::new_slot(blocked_add);
        blocked_added_slot->set_worker(worker);
        added_sig->connect(blocked_added_slot);

        {
            const auto scoped_emitter = container->scoped_emit();
            scoped_emitter->block(blocked_added_slot);

            if constexpr(std::is_same_v<camera_set, T>)
            {
                container->add_camera(std::static_pointer_cast<camera>(object1));
                container->add_camera(std::static_pointer_cast<camera>(object2));
                container->add_camera(std::static_pointer_cast<camera>(object3));
            }
            else if constexpr(std::is_same_v<activity_set, T>)
            {
                container->push_back(std::static_pointer_cast<activity>(object1));
                container->push_back(std::static_pointer_cast<activity>(object2));
                container->push_back(std::static_pointer_cast<activity>(object3));
            }
            else if constexpr(std::is_same_v<series_set, T>)
            {
                container->push_back(std::dynamic_pointer_cast<series>(object1));
                container->push_back(std::dynamic_pointer_cast<series>(object2));
                container->push_back(std::dynamic_pointer_cast<series>(object3));
            }
            else if constexpr(core::is_map_like<T>::value)
            {
                container->insert({s_DESCRIPTION1, object1});
                container->insert({s_DESCRIPTION2, object2});
                container->insert({s_DESCRIPTION3, object3});
            }
            else
            {
                container->push_back(object1);
                container->push_back(object2);
                container->push_back(object3);
            }
        }

        // Check results
        {
            {
                std::unique_lock lock(mutex);
                condition_variable.wait(lock, [&]{return add_call_count == 1;});
            }

            // Stop the worker here, to not interfere in case of CPPUNIT_ASSERT faillure.
            worker->stop();

            CPPUNIT_ASSERT(added_from_slot.size() == 3);
            CPPUNIT_ASSERT(add_call_count == 1);
            CPPUNIT_ASSERT(blocked_add_called == false);

            if constexpr(std::is_same_v<camera_set, T>)
            {
                CPPUNIT_ASSERT(added_from_slot[0].first == object1);
                CPPUNIT_ASSERT(added_from_slot[1].first == object2);
                CPPUNIT_ASSERT(added_from_slot[2].first == object3);
            }
            else if constexpr(core::is_map_like<T>::value)
            {
                CPPUNIT_ASSERT(added_from_slot[s_DESCRIPTION1] == object1);
                CPPUNIT_ASSERT(added_from_slot[s_DESCRIPTION2] == object2);
                CPPUNIT_ASSERT(added_from_slot[s_DESCRIPTION3] == object3);
            }
            else
            {
                CPPUNIT_ASSERT(added_from_slot[0] == object1);
                CPPUNIT_ASSERT(added_from_slot[1] == object2);
                CPPUNIT_ASSERT(added_from_slot[2] == object3);
            }
        }
    }

    // Test remove
    {
        auto worker                   = core::thread::worker::make();
        std::size_t remove_call_count = 0;
        typename T::container_t removed_from_slot;

        std::function<void(typename T::container_t)> remove =
            [&](typename T::container_t _removed_from_signal)
            {
                {
                    std::unique_lock lock(mutex);
                    removed_from_slot = _removed_from_signal;
                    ++remove_call_count;
                }
                condition_variable.notify_one();
            };

        auto removed_slot = core::com::new_slot(remove);
        removed_slot->set_worker(worker);
        auto removed_sig = container->template signal<typename T::removed_signal_t>(T::REMOVED_OBJECTS_SIG);
        removed_sig->connect(removed_slot);

        bool blocked_remove_called           = false;
        std::function<void()> blocked_remove =
            [&]()
            {
                {
                    std::unique_lock lock(mutex);
                    blocked_remove_called = true;
                }
                condition_variable.notify_one();
            };

        auto blocked_removed_slot1 = core::com::new_slot(blocked_remove);
        auto blocked_removed_slot2 = core::com::new_slot(blocked_remove);
        blocked_removed_slot1->set_worker(worker);
        blocked_removed_slot2->set_worker(worker);
        removed_sig->connect(blocked_removed_slot1);
        removed_sig->connect(blocked_removed_slot2);

        {
            const auto scoped_emitter = container->scoped_emit();
            scoped_emitter->block(blocked_removed_slot1);
            scoped_emitter->block(blocked_removed_slot2);

            // Erase the first element
            if constexpr(std::is_same_v<camera_set, T>)
            {
                container->remove_camera(std::static_pointer_cast<camera>(object1));
            }
            else if constexpr(core::is_map_like<T>::value)
            {
                container->erase(s_DESCRIPTION1);
            }
            else
            {
                container->erase(container->cbegin());
            }
        }

        // Check results
        {
            {
                std::unique_lock lock(mutex);
                condition_variable.wait(lock, [&]{return remove_call_count == 1;});
            }

            // Stop the worker here, to not interfere in case of CPPUNIT_ASSERT faillure.
            worker->stop();

            CPPUNIT_ASSERT(removed_from_slot.size() == 1);
            CPPUNIT_ASSERT(remove_call_count == 1);
            CPPUNIT_ASSERT(blocked_remove_called == false);

            if constexpr(std::is_same_v<camera_set, T>)
            {
                CPPUNIT_ASSERT(removed_from_slot[0].first == object1);
            }
            else if constexpr(core::is_map_like<T>::value)
            {
                CPPUNIT_ASSERT(removed_from_slot[s_DESCRIPTION1] == object1);
            }
            else
            {
                CPPUNIT_ASSERT(removed_from_slot[0] == object1);
            }
        }
    }

    // Test changed signal
    if constexpr(core::is_map_like<T>::value)
    {
        auto worker                   = core::thread::worker::make();
        std::size_t change_call_count = 0;
        typename T::container_t old_from_slot;
        typename T::container_t new_from_slot;

        std::function<void(typename T::container_t, typename T::container_t)> change =
            [&](typename T::container_t _old_from_signal, typename T::container_t _new_from_signal)
            {
                {
                    std::unique_lock lock(mutex);
                    old_from_slot = _old_from_signal;
                    new_from_slot = _new_from_signal;
                    ++change_call_count;
                }
                condition_variable.notify_one();
            };

        auto changed_slot = core::com::new_slot(change);
        changed_slot->set_worker(worker);
        auto changed_sig = container->template signal<typename T::changed_signal_t>(T::CHANGED_OBJECTS_SIG);
        changed_sig->connect(changed_slot);

        bool blocked_change_called           = false;
        std::function<void()> blocked_change =
            [&]()
            {
                {
                    std::unique_lock lock(mutex);
                    blocked_change_called = true;
                }
                condition_variable.notify_one();
            };

        auto blocked_changed_slot = core::com::new_slot(blocked_change);
        blocked_changed_slot->set_worker(worker);
        changed_sig->connect(blocked_changed_slot);

        {
            // Insert value before creating a notifier
            container->insert({s_DESCRIPTION1, object1});
            container->insert({s_DESCRIPTION2, object2});

            // Change one element
            const auto scoped_emitter = container->scoped_emit();
            scoped_emitter->block(blocked_changed_slot);
            container->insert_or_assign(s_DESCRIPTION2, object3);
        }

        // Check results
        {
            {
                std::unique_lock lock(mutex);
                condition_variable.wait(lock, [&]{return change_call_count == 1;});
            }

            // Stop the worker here, to not interfere in case of CPPUNIT_ASSERT faillure.
            worker->stop();

            CPPUNIT_ASSERT(old_from_slot.size() == 1);
            CPPUNIT_ASSERT(new_from_slot.size() == 1);
            CPPUNIT_ASSERT(change_call_count == 1);
            CPPUNIT_ASSERT(blocked_change_called == false);

            CPPUNIT_ASSERT(old_from_slot[s_DESCRIPTION2] == object2);
            CPPUNIT_ASSERT(new_from_slot[s_DESCRIPTION2] == object3);
        }
    }
}

//------------------------------------------------------------------------------

void container_notifier_test::setUp()
{
}

//------------------------------------------------------------------------------

void container_notifier_test::tearDown()
{
}

//------------------------------------------------------------------------------

void container_notifier_test::vector_test()
{
    container_notifier_test_fn<vector>();
}

//------------------------------------------------------------------------------

void container_notifier_test::map_test()
{
    container_notifier_test_fn<map>();
}

//------------------------------------------------------------------------------

void container_notifier_test::series_set_test()
{
    container_notifier_test_fn<series_set>();
}

//------------------------------------------------------------------------------

void container_notifier_test::activity_set_test()
{
    container_notifier_test_fn<activity_set>();
}

//------------------------------------------------------------------------------

void container_notifier_test::camera_set_test()
{
    container_notifier_test_fn<camera_set>();
}

} // namespace sight::data::tools::ut
