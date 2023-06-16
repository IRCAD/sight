/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "ContainerNotifierTest.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/thread/Worker.hpp>
#include <core/tools/compare.hpp>
#include <core/tools/UUID.hpp>

#include <data/ActivitySet.hpp>
#include <data/CameraSet.hpp>
#include <data/Composite.hpp>
#include <data/SeriesSet.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::ContainerNotifierTest);

namespace sight::data::tools::ut
{

//------------------------------------------------------------------------------

template<typename T>
inline static void containerNotifierTest()
{
    auto container = T::New();

    static const auto description1 = core::tools::UUID::generateUUID();
    static const auto description2 = core::tools::UUID::generateUUID();
    static const auto description3 = core::tools::UUID::generateUUID();

    Object::sptr object1;
    Object::sptr object2;
    Object::sptr object3;

    if constexpr(std::is_same_v<CameraSet, T>)
    {
        object1 = Camera::New();
        object2 = Camera::New();
        object3 = Camera::New();
    }
    else if constexpr(std::is_same_v<ActivitySet, T>)
    {
        object1 = Activity::New();
        object2 = Activity::New();
        object3 = Activity::New();
    }
    else if constexpr(std::is_same_v<SeriesSet, T>)
    {
        object1 = Series::New();
        object2 = Series::New();
        object3 = Series::New();
    }
    else
    {
        object1 = String::New(description1);
        object2 = String::New(description1);
        object3 = String::New(description1);
    }

    std::mutex mutex;
    std::condition_variable condition_variable;

    // Test add
    {
        auto worker                = core::thread::Worker::New();
        std::size_t add_call_count = 0;
        typename T::container_type added_from_slot;

        std::function<void(typename T::container_type)> add =
            [&](typename T::container_type added_from_signal)
            {
                {
                    std::unique_lock lock(mutex);
                    added_from_slot = added_from_signal;
                    ++add_call_count;
                }
                condition_variable.notify_one();
            };

        auto added_slot = core::com::newSlot(add);
        added_slot->setWorker(worker);
        auto added_sig = container->template signal<typename T::added_signal_t>(T::s_ADDED_OBJECTS_SIG);
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

        auto blocked_added_slot = core::com::newSlot(blocked_add);
        blocked_added_slot->setWorker(worker);
        added_sig->connect(blocked_added_slot);

        {
            const auto scoped_emitter = container->scoped_emit();
            scoped_emitter->block(blocked_added_slot);

            if constexpr(std::is_same_v<CameraSet, T>)
            {
                container->add_camera(std::static_pointer_cast<Camera>(object1));
                container->add_camera(std::static_pointer_cast<Camera>(object2));
                container->add_camera(std::static_pointer_cast<Camera>(object3));
            }
            else if constexpr(std::is_same_v<ActivitySet, T>)
            {
                container->push_back(std::static_pointer_cast<Activity>(object1));
                container->push_back(std::static_pointer_cast<Activity>(object2));
                container->push_back(std::static_pointer_cast<Activity>(object3));
            }
            else if constexpr(std::is_same_v<SeriesSet, T>)
            {
                container->push_back(std::dynamic_pointer_cast<Series>(object1));
                container->push_back(std::dynamic_pointer_cast<Series>(object2));
                container->push_back(std::dynamic_pointer_cast<Series>(object3));
            }
            else if constexpr(core::tools::is_map_like<T>::value)
            {
                container->insert({description1, object1});
                container->insert({description2, object2});
                container->insert({description3, object3});
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

            if constexpr(std::is_same<CameraSet, T>::value)
            {
                CPPUNIT_ASSERT(added_from_slot[0].first == object1);
                CPPUNIT_ASSERT(added_from_slot[1].first == object2);
                CPPUNIT_ASSERT(added_from_slot[2].first == object3);
            }
            else if constexpr(core::tools::is_map_like<T>::value)
            {
                CPPUNIT_ASSERT(added_from_slot[description1] == object1);
                CPPUNIT_ASSERT(added_from_slot[description2] == object2);
                CPPUNIT_ASSERT(added_from_slot[description3] == object3);
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
        auto worker                   = core::thread::Worker::New();
        std::size_t remove_call_count = 0;
        typename T::container_type removed_from_slot;

        std::function<void(typename T::container_type)> remove =
            [&](typename T::container_type removed_from_signal)
            {
                {
                    std::unique_lock lock(mutex);
                    removed_from_slot = removed_from_signal;
                    ++remove_call_count;
                }
                condition_variable.notify_one();
            };

        auto removed_slot = core::com::newSlot(remove);
        removed_slot->setWorker(worker);
        auto removed_sig = container->template signal<typename T::removed_signal_t>(T::s_REMOVED_OBJECTS_SIG);
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

        auto blocked_removed_slot1 = core::com::newSlot(blocked_remove);
        auto blocked_removed_slot2 = core::com::newSlot(blocked_remove);
        blocked_removed_slot1->setWorker(worker);
        blocked_removed_slot2->setWorker(worker);
        removed_sig->connect(blocked_removed_slot1);
        removed_sig->connect(blocked_removed_slot2);

        {
            const auto scoped_emitter = container->scoped_emit();
            scoped_emitter->block(blocked_removed_slot1);
            scoped_emitter->block(blocked_removed_slot2);

            // Erase the first element
            if constexpr(std::is_same<CameraSet, T>::value)
            {
                container->remove_camera(std::static_pointer_cast<Camera>(object1));
            }
            else if constexpr(core::tools::is_map_like<T>::value)
            {
                container->erase(description1);
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

            if constexpr(std::is_same<CameraSet, T>::value)
            {
                CPPUNIT_ASSERT(removed_from_slot[0].first == object1);
            }
            else if constexpr(core::tools::is_map_like<T>::value)
            {
                CPPUNIT_ASSERT(removed_from_slot[description1] == object1);
            }
            else
            {
                CPPUNIT_ASSERT(removed_from_slot[0] == object1);
            }
        }
    }

    // Test changed signal
    if constexpr(core::tools::is_map_like<T>::value)
    {
        auto worker                   = core::thread::Worker::New();
        std::size_t change_call_count = 0;
        typename T::container_type old_from_slot;
        typename T::container_type new_from_slot;

        std::function<void(typename T::container_type, typename T::container_type)> change =
            [&](typename T::container_type old_from_signal, typename T::container_type new_from_signal)
            {
                {
                    std::unique_lock lock(mutex);
                    old_from_slot = old_from_signal;
                    new_from_slot = new_from_signal;
                    ++change_call_count;
                }
                condition_variable.notify_one();
            };

        auto changed_slot = core::com::newSlot(change);
        changed_slot->setWorker(worker);
        auto changed_sig = container->template signal<typename T::changed_signal_t>(T::s_CHANGED_OBJECTS_SIG);
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

        auto blocked_changed_slot = core::com::newSlot(blocked_change);
        blocked_changed_slot->setWorker(worker);
        changed_sig->connect(blocked_changed_slot);

        {
            // Insert value before creating a notifier
            container->insert({description1, object1});
            container->insert({description2, object2});

            // Change one element
            const auto scoped_emitter = container->scoped_emit();
            scoped_emitter->block(blocked_changed_slot);
            container->insert_or_assign(description2, object3);
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

            CPPUNIT_ASSERT(old_from_slot[description2] == object2);
            CPPUNIT_ASSERT(new_from_slot[description2] == object3);
        }
    }
}

//------------------------------------------------------------------------------

void ContainerNotifierTest::setUp()
{
}

//------------------------------------------------------------------------------

void ContainerNotifierTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ContainerNotifierTest::vectorTest()
{
    containerNotifierTest<Vector>();
}

//------------------------------------------------------------------------------

void ContainerNotifierTest::compositeTest()
{
    containerNotifierTest<Composite>();
}

//------------------------------------------------------------------------------

void ContainerNotifierTest::seriesSetTest()
{
    containerNotifierTest<SeriesSet>();
}

//------------------------------------------------------------------------------

void ContainerNotifierTest::activitySetTest()
{
    containerNotifierTest<ActivitySet>();
}

//------------------------------------------------------------------------------

void ContainerNotifierTest::cameraSetTest()
{
    containerNotifierTest<CameraSet>();
}

} // namespace sight::data::tools::ut
