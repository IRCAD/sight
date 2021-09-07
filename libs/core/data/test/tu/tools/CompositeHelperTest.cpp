/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "CompositeHelperTest.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>

#include <data/helper/Composite.hpp>
#include <data/String.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::data::tools::ut::CompositeHelperTest);

namespace sight::data::tools
{

namespace ut
{

//------------------------------------------------------------------------------

void CompositeHelperTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CompositeHelperTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void CompositeHelperTest::testHelper()
{
    const std::string COMPOSITE_ID1 = "COMPOSITE_ID1";
    const std::string COMPOSITE_ID2 = "COMPOSITE_ID2";
    const std::string COMPOSITE_ID3 = "COMPOSITE_ID3";

    data::Object::sptr nullobj;
    data::Composite::sptr obj        = data::Composite::New();
    data::Object::sptr compositeObj1 = data::String::New();
    data::Object::sptr compositeObj2 = data::String::New();
    data::Object::sptr compositeObj3 = data::String::New();

    core::thread::Worker::sptr worker = core::thread::Worker::New();

    // Setup to test notifications
    unsigned int numAddedNotif = 0;
    std::mutex mutex;
    std::condition_variable condition;

    data::Composite::ContainerType addedComposites;
    std::function<void(data::Composite::ContainerType)> fnAdd =
        [&](data::Composite::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numAddedNotif;
                addedComposites = f;
            }
            condition.notify_one();
        };

    auto slotAdded = core::com::newSlot(fnAdd);
    slotAdded->setWorker(worker);
    auto sigAdded = obj->signal<data::Composite::AddedObjectsSignalType>(data::Composite::s_ADDED_OBJECTS_SIG);
    sigAdded->connect(slotAdded);

    unsigned int numRemovedNotif = 0;
    data::Composite::ContainerType removedComposites;

    std::function<void(data::Composite::ContainerType)> fnRemove =
        [&](data::Composite::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numRemovedNotif;
                removedComposites = f;
            }
            condition.notify_one();
        };
    auto slotRemoved = core::com::newSlot(fnRemove);
    slotRemoved->setWorker(worker);
    auto sigRemoved = obj->signal<data::Composite::RemovedObjectsSignalType>(
        data::Composite::s_REMOVED_OBJECTS_SIG
    );
    sigRemoved->connect(slotRemoved);

    unsigned int numChangedNotif = 0;
    data::Composite::ContainerType newComposites;
    data::Composite::ContainerType oldComposites;

    std::function<void(data::Composite::ContainerType, data::Composite::ContainerType)> fnChange =
        [&](data::Composite::ContainerType newF, data::Composite::ContainerType oldF)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numChangedNotif;
                newComposites = newF;
                oldComposites = oldF;
            }
            condition.notify_one();
        };
    auto slotChanged = core::com::newSlot(fnChange);
    slotChanged->setWorker(worker);
    auto sigChanged = obj->signal<data::Composite::ChangedObjectsSignalType>(
        data::Composite::s_CHANGED_OBJECTS_SIG
    );
    sigChanged->connect(slotChanged);

    auto clearArrays =
        [&](){addedComposites.clear(); removedComposites.clear(); newComposites.clear(); oldComposites.clear();};

    {
        // Test add()
        data::helper::Composite compositeHelper(obj);
        compositeHelper.add(COMPOSITE_ID1, compositeObj1);
        compositeHelper.add(COMPOSITE_ID2, compositeObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(2), obj->getContainer().size());
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID1] == compositeObj1);
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID2] == compositeObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(0), obj->count(COMPOSITE_ID3));
        CPPUNIT_ASSERT_EQUAL(size_t(2), obj->getContainer().size());
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numAddedNotif == 1;});

        CPPUNIT_ASSERT_EQUAL(size_t(2), addedComposites.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), removedComposites.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), newComposites.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), oldComposites.size());
        CPPUNIT_ASSERT(addedComposites[COMPOSITE_ID1] == compositeObj1);
        CPPUNIT_ASSERT(addedComposites[COMPOSITE_ID2] == compositeObj2);
        clearArrays();
    }

    {
        // Test replacement with swap()
        data::helper::Composite compositeHelper(obj);
        compositeHelper.swap(COMPOSITE_ID1, compositeObj3);
        CPPUNIT_ASSERT_EQUAL(size_t(2), obj->getContainer().size());
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID1] == compositeObj3);
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID2] == compositeObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(0), obj->count(COMPOSITE_ID3));

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        compositeHelper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numChangedNotif == 1;});

        CPPUNIT_ASSERT_EQUAL(size_t(0), addedComposites.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), removedComposites.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), newComposites.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), oldComposites.size());
        CPPUNIT_ASSERT(oldComposites[COMPOSITE_ID1] == compositeObj1);
        CPPUNIT_ASSERT(newComposites[COMPOSITE_ID1] == compositeObj3);
        clearArrays();
    }

    {
        // Test add(), swap() and remove()
        data::helper::Composite compositeHelper(obj);

        compositeHelper.swap(COMPOSITE_ID2, compositeObj3);
        CPPUNIT_ASSERT_EQUAL(size_t(2), obj->getContainer().size());
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID1] == compositeObj3);
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID2] == compositeObj3);
        CPPUNIT_ASSERT_EQUAL(size_t(0), obj->count(COMPOSITE_ID3));

        compositeHelper.remove(COMPOSITE_ID1);
        CPPUNIT_ASSERT_EQUAL(size_t(1), obj->getContainer().size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), obj->count(COMPOSITE_ID1));
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID2] == compositeObj3);
        CPPUNIT_ASSERT_EQUAL(size_t(0), obj->count(COMPOSITE_ID3));

        compositeHelper.add(COMPOSITE_ID3, compositeObj1);
        CPPUNIT_ASSERT_EQUAL(size_t(2), obj->getContainer().size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), obj->count(COMPOSITE_ID1));
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID2] == compositeObj3);
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID3] == compositeObj1);

        compositeHelper.add(COMPOSITE_ID1, compositeObj3);
        CPPUNIT_ASSERT_EQUAL(size_t(3), obj->getContainer().size());
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID1] == compositeObj3);
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID2] == compositeObj3);
        CPPUNIT_ASSERT((*obj)[COMPOSITE_ID3] == compositeObj1);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numAddedNotif == 2 && numRemovedNotif == 1 && numChangedNotif == 2;});

        CPPUNIT_ASSERT_EQUAL(size_t(2), addedComposites.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), removedComposites.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), newComposites.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), oldComposites.size());
        CPPUNIT_ASSERT(addedComposites[COMPOSITE_ID1] == compositeObj3);
        CPPUNIT_ASSERT(addedComposites[COMPOSITE_ID3] == compositeObj1);
        CPPUNIT_ASSERT(oldComposites[COMPOSITE_ID2] == compositeObj2);
        CPPUNIT_ASSERT(newComposites[COMPOSITE_ID2] == compositeObj3);
        CPPUNIT_ASSERT(removedComposites[COMPOSITE_ID1] == compositeObj3);
        clearArrays();
    }

    {
        // Test clear()
        data::helper::Composite compositeHelper(obj);
        compositeHelper.clear();
        CPPUNIT_ASSERT_EQUAL(size_t(0), obj->getContainer().size());
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numRemovedNotif == 2;});

        CPPUNIT_ASSERT_EQUAL(size_t(3), removedComposites.size());
        CPPUNIT_ASSERT(removedComposites[COMPOSITE_ID1] == compositeObj3);
        CPPUNIT_ASSERT(removedComposites[COMPOSITE_ID2] == compositeObj3);
        CPPUNIT_ASSERT(removedComposites[COMPOSITE_ID3] == compositeObj1);
        clearArrays();
    }

    worker->stop();
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::data::tools
