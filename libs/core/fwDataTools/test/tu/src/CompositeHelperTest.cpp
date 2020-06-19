/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include <fwDataTools/helper/Composite.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>

#include <fwData/String.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::CompositeHelperTest );

namespace fwDataTools
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

    ::fwData::Object::sptr nullobj;
    ::fwData::Composite::sptr obj        = ::fwData::Composite::New();
    ::fwData::Object::sptr compositeObj1 = ::fwData::String::New();
    ::fwData::Object::sptr compositeObj2 = ::fwData::String::New();
    ::fwData::Object::sptr compositeObj3 = ::fwData::String::New();

    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

    // Setup to test notifications
    unsigned int numAddedNotif = 0;
    std::mutex mutex;
    std::condition_variable condition;

    ::fwData::Composite::ContainerType addedComposites;
    std::function<void ( ::fwData::Composite::ContainerType)> fnAdd =
        [&](::fwData::Composite::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numAddedNotif;
                addedComposites = f;
            }
            condition.notify_one();
        };

    auto slotAdded = ::fwCom::newSlot( fnAdd );
    slotAdded->setWorker(worker);
    auto sigAdded = obj->signal< ::fwData::Composite::AddedObjectsSignalType>(::fwData::Composite::s_ADDED_OBJECTS_SIG);
    sigAdded->connect(slotAdded);

    unsigned int numRemovedNotif = 0;
    ::fwData::Composite::ContainerType removedComposites;

    std::function<void ( ::fwData::Composite::ContainerType)> fnRemove =
        [&](::fwData::Composite::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numRemovedNotif;
                removedComposites = f;
            }
            condition.notify_one();
        };
    auto slotRemoved = ::fwCom::newSlot( fnRemove );
    slotRemoved->setWorker(worker);
    auto sigRemoved = obj->signal< ::fwData::Composite::RemovedObjectsSignalType>(
        ::fwData::Composite::s_REMOVED_OBJECTS_SIG);
    sigRemoved->connect(slotRemoved);

    unsigned int numChangedNotif = 0;
    ::fwData::Composite::ContainerType newComposites;
    ::fwData::Composite::ContainerType oldComposites;

    std::function<void ( ::fwData::Composite::ContainerType, ::fwData::Composite::ContainerType)> fnChange =
        [&](::fwData::Composite::ContainerType newF, ::fwData::Composite::ContainerType oldF)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numChangedNotif;
                newComposites = newF;
                oldComposites = oldF;
            }
            condition.notify_one();
        };
    auto slotChanged = ::fwCom::newSlot( fnChange );
    slotChanged->setWorker(worker);
    auto sigChanged = obj->signal< ::fwData::Composite::ChangedObjectsSignalType>(
        ::fwData::Composite::s_CHANGED_OBJECTS_SIG);
    sigChanged->connect(slotChanged);

    auto clearArrays =
        [&]() { addedComposites.clear(); removedComposites.clear(); newComposites.clear(); oldComposites.clear(); };

    {
        // Test add()
        ::fwDataTools::helper::Composite compositeHelper(obj);
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
        condition.wait(lock, [&] { return numAddedNotif == 1; });

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
        ::fwDataTools::helper::Composite compositeHelper(obj);
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
        condition.wait(lock, [&] { return numChangedNotif == 1; });

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
        ::fwDataTools::helper::Composite compositeHelper(obj);

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
        condition.wait(lock, [&] { return numAddedNotif == 2 && numRemovedNotif == 1 && numChangedNotif == 2; });

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
        ::fwDataTools::helper::Composite compositeHelper(obj);
        compositeHelper.clear();
        CPPUNIT_ASSERT_EQUAL(size_t(0), obj->getContainer().size());
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numRemovedNotif == 2; });

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
} //namespace fwDataTools
