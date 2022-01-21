/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "VectorHelperTest.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>

#include <data/helper/Vector.hpp>
#include <data/String.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::tools::ut::VectorHelperTest);

namespace sight::data::tools
{

namespace ut
{

//------------------------------------------------------------------------------

void VectorHelperTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void VectorHelperTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void VectorHelperTest::testHelper()
{
    data::Object::sptr nullobj;
    data::Vector::sptr vector     = data::Vector::New();
    data::Object::sptr vectorObj1 = data::String::New();
    data::Object::sptr vectorObj2 = data::String::New();
    data::Object::sptr vectorObj3 = data::String::New();

    core::thread::Worker::sptr worker = core::thread::Worker::New();

    // Setup to test notifications
    unsigned int numAddedNotif = 0;
    std::mutex mutex;
    std::condition_variable condition;

    data::Vector::ContainerType addedVectors;
    std::function<void(data::Vector::ContainerType)> fnAdd =
        [&](data::Vector::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numAddedNotif;
                addedVectors = f;
            }
            condition.notify_one();
        };

    auto slotAdded = core::com::newSlot(fnAdd);
    slotAdded->setWorker(worker);
    auto sigAdded = vector->signal<data::Vector::AddedObjectsSignalType>(data::Vector::s_ADDED_OBJECTS_SIG);
    sigAdded->connect(slotAdded);

    unsigned int numRemovedNotif = 0;
    data::Vector::ContainerType removedVectors;

    std::function<void(data::Vector::ContainerType)> fnRemove =
        [&](data::Vector::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numRemovedNotif;
                removedVectors = f;
            }
            condition.notify_one();
        };
    auto slotRemoved = core::com::newSlot(fnRemove);
    slotRemoved->setWorker(worker);
    auto sigRemoved = vector->signal<data::Vector::RemovedObjectsSignalType>(
        data::Vector::s_REMOVED_OBJECTS_SIG
    );
    sigRemoved->connect(slotRemoved);

    auto clearArrays = [&](){addedVectors.clear(); removedVectors.clear();};

    {
        // Test add()
        data::helper::Vector vectorHelper(vector);
        vectorHelper.add(vectorObj1);
        vectorHelper.add(vectorObj2);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj1);
        CPPUNIT_ASSERT((*vector)[1] == vectorObj2);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numAddedNotif == 1;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(2), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), removedVectors.size());
        CPPUNIT_ASSERT(addedVectors[0] == vectorObj1);
        CPPUNIT_ASSERT(addedVectors[1] == vectorObj2);
        clearArrays();
    }

    {
        // Test add()
        data::helper::Vector vectorHelper(vector);
        vectorHelper.add(vectorObj3);
        CPPUNIT_ASSERT_EQUAL(std::size_t(3), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj1);
        CPPUNIT_ASSERT((*vector)[1] == vectorObj2);
        CPPUNIT_ASSERT((*vector)[2] == vectorObj3);

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        vectorHelper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numAddedNotif == 2;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(1), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), removedVectors.size());
        CPPUNIT_ASSERT(addedVectors[0] == vectorObj3);
        clearArrays();
    }

    {
        // Test remove()
        data::helper::Vector vectorHelper(vector);
        vectorHelper.remove(vectorObj1);
        vectorHelper.remove(vectorObj2);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj3);

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        vectorHelper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numRemovedNotif == 1;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(0), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), removedVectors.size());
        CPPUNIT_ASSERT(removedVectors[0] == vectorObj1);
        CPPUNIT_ASSERT(removedVectors[1] == vectorObj2);
        clearArrays();
    }

    {
        // Test add(), and clear()
        data::helper::Vector vectorHelper(vector);
        vectorHelper.add(vectorObj2);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj3);
        CPPUNIT_ASSERT((*vector)[1] == vectorObj2);

        vectorHelper.clear();
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), vector->size());
        CPPUNIT_ASSERT_EQUAL(true, vector->empty());
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numAddedNotif == 3 && numRemovedNotif == 2;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(1), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), removedVectors.size());
        CPPUNIT_ASSERT(addedVectors[0] == vectorObj2);
        CPPUNIT_ASSERT(removedVectors[0] == vectorObj3);
        CPPUNIT_ASSERT(removedVectors[1] == vectorObj2);
        clearArrays();
    }

    worker->stop();
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::data::tools
