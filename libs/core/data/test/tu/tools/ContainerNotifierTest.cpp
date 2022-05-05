/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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
#include <data/Vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::ContainerNotifierTest);

namespace sight::data::tools::ut
{

//------------------------------------------------------------------------------

template<typename T>
inline static void containerNotifierTest()
{
    auto container = std::make_shared<T>();

    static const auto description1 = core::tools::UUID::generateUUID();
    static const auto description2 = core::tools::UUID::generateUUID();
    static const auto description3 = core::tools::UUID::generateUUID();

    // Create three ActivitySeries
    const auto& series1 = std::make_shared<ActivitySeries>();
    series1->setDescription(description1);

    const auto& series2 = std::make_shared<ActivitySeries>();
    series2->setDescription(description2);

    const auto& series3 = std::make_shared<ActivitySeries>();
    series3->setDescription(description3);

    // Create three Cameras (for CameraSet test)
    const auto& camera1 = std::make_shared<Camera>();
    camera1->setCameraID(description1);

    const auto& camera2 = std::make_shared<Camera>();
    camera2->setCameraID(description2);

    const auto& camera3 = std::make_shared<Camera>();
    camera3->setCameraID(description3);

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

        {
            const auto scoped_emitter = container->scoped_emit();

            if constexpr(std::is_same<CameraSet, T>::value)
            {
                container->add_camera(camera1);
                container->add_camera(camera2);
                container->add_camera(camera3);
            }
            else if constexpr(core::tools::is_map_like<T>::value)
            {
                container->insert({description1, series1});
                container->insert({description2, series2});
                container->insert({description3, series3});
            }
            else
            {
                container->push_back(series1);
                container->push_back(series2);
                container->push_back(series3);
            }
        }

        // Check results
        {
            std::unique_lock lock(mutex);
            condition_variable.wait(lock, [&]{return add_call_count == 1;});

            // Stop the worker here, to not interfere in case of CPPUNIT_ASSERT faillure.
            worker->stop();

            CPPUNIT_ASSERT(added_from_slot.size() == 3);
            CPPUNIT_ASSERT(add_call_count == 1);

            if constexpr(std::is_same<CameraSet, T>::value)
            {
                CPPUNIT_ASSERT(added_from_slot[0].first == camera1);
                CPPUNIT_ASSERT(added_from_slot[1].first == camera2);
                CPPUNIT_ASSERT(added_from_slot[2].first == camera3);
            }
            else if constexpr(core::tools::is_map_like<T>::value)
            {
                CPPUNIT_ASSERT(added_from_slot[description1] == series1);
                CPPUNIT_ASSERT(added_from_slot[description2] == series2);
                CPPUNIT_ASSERT(added_from_slot[description3] == series3);
            }
            else
            {
                CPPUNIT_ASSERT(added_from_slot[0] == series1);
                CPPUNIT_ASSERT(added_from_slot[1] == series2);
                CPPUNIT_ASSERT(added_from_slot[2] == series3);
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

        {
            const auto scoped_emitter = container->scoped_emit();

            // Erase the first element
            if constexpr(std::is_same<CameraSet, T>::value)
            {
                container->remove_camera(camera1);
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
            std::unique_lock lock(mutex);
            condition_variable.wait(lock, [&]{return remove_call_count == 1;});

            // Stop the worker here, to not interfere in case of CPPUNIT_ASSERT faillure.
            worker->stop();

            CPPUNIT_ASSERT(removed_from_slot.size() == 1);
            CPPUNIT_ASSERT(remove_call_count == 1);

            if constexpr(std::is_same<CameraSet, T>::value)
            {
                CPPUNIT_ASSERT(removed_from_slot[0].first == camera1);
            }
            else if constexpr(core::tools::is_map_like<T>::value)
            {
                CPPUNIT_ASSERT(removed_from_slot[description1] == series1);
            }
            else
            {
                CPPUNIT_ASSERT(removed_from_slot[0] == series1);
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

        {
            // Insert value before creating a notifier
            container->insert({description1, series1});
            container->insert({description2, series2});

            // Change one element
            const auto scoped_emitter = container->scoped_emit();
            container->insert_or_assign(description2, series3);
        }

        // Check results
        {
            std::unique_lock lock(mutex);
            condition_variable.wait(lock, [&]{return change_call_count == 1;});

            // Stop the worker here, to not interfere in case of CPPUNIT_ASSERT faillure.
            worker->stop();

            CPPUNIT_ASSERT(old_from_slot.size() == 1);
            CPPUNIT_ASSERT(new_from_slot.size() == 1);
            CPPUNIT_ASSERT(change_call_count == 1);

            CPPUNIT_ASSERT(old_from_slot[description2] == series2);
            CPPUNIT_ASSERT(new_from_slot[description2] == series3);
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
