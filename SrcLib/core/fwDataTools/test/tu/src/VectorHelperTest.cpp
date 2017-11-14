/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "VectorHelperTest.hpp"

#include <fwDataTools/helper/Vector.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>

#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::VectorHelperTest );

namespace fwDataTools
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
    ::fwData::Object::sptr nullobj;
    ::fwData::Vector::sptr vector     = ::fwData::Vector::New();
    ::fwData::Object::sptr vectorObj1 = ::fwData::String::New();
    ::fwData::Object::sptr vectorObj2 = ::fwData::String::New();
    ::fwData::Object::sptr vectorObj3 = ::fwData::String::New();

    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

    // Setup to test notifications
    unsigned int numAddedNotif = 0;
    std::mutex mutex;
    std::condition_variable condition;

    ::fwData::Vector::ContainerType addedVectors;
    std::function<void ( ::fwData::Vector::ContainerType)> fnAdd =
        [&](::fwData::Vector::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numAddedNotif;
                addedVectors = f;
            }
            condition.notify_one();
        };

    auto slotAdded = ::fwCom::newSlot( fnAdd );
    slotAdded->setWorker(worker);
    auto sigAdded = vector->signal< ::fwData::Vector::AddedObjectsSignalType>(::fwData::Vector::s_ADDED_OBJECTS_SIG);
    sigAdded->connect(slotAdded);

    unsigned int numRemovedNotif = 0;
    ::fwData::Vector::ContainerType removedVectors;

    std::function<void ( ::fwData::Vector::ContainerType)> fnRemove =
        [&](::fwData::Vector::ContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numRemovedNotif;
                removedVectors = f;
            }
            condition.notify_one();
        };
    auto slotRemoved = ::fwCom::newSlot( fnRemove );
    slotRemoved->setWorker(worker);
    auto sigRemoved = vector->signal< ::fwData::Vector::RemovedObjectsSignalType>(
        ::fwData::Vector::s_REMOVED_OBJECTS_SIG);
    sigRemoved->connect(slotRemoved);

    auto clearArrays = [&]() { addedVectors.clear(); removedVectors.clear(); };

    {
        // Test add()
        ::fwDataTools::helper::Vector vectorHelper(vector);
        vectorHelper.add(vectorObj1);
        vectorHelper.add(vectorObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(2), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj1);
        CPPUNIT_ASSERT((*vector)[1] == vectorObj2);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numAddedNotif == 1; });

        CPPUNIT_ASSERT_EQUAL(size_t(2), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), removedVectors.size());
        CPPUNIT_ASSERT(addedVectors[0] == vectorObj1);
        CPPUNIT_ASSERT(addedVectors[1] == vectorObj2);
        clearArrays();
    }

    {
        // Test add()
        ::fwDataTools::helper::Vector vectorHelper(vector);
        vectorHelper.add(vectorObj3);
        CPPUNIT_ASSERT_EQUAL(size_t(3), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj1);
        CPPUNIT_ASSERT((*vector)[1] == vectorObj2);
        CPPUNIT_ASSERT((*vector)[2] == vectorObj3);

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        vectorHelper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numAddedNotif == 2; });

        CPPUNIT_ASSERT_EQUAL(size_t(1), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), removedVectors.size());
        CPPUNIT_ASSERT(addedVectors[0] == vectorObj3);
        clearArrays();
    }

    {
        // Test remove()
        ::fwDataTools::helper::Vector vectorHelper(vector);
        vectorHelper.remove(vectorObj1);
        vectorHelper.remove(vectorObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(1), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj3);

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        vectorHelper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numRemovedNotif == 1; });

        CPPUNIT_ASSERT_EQUAL(size_t(0), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(size_t(2), removedVectors.size());
        CPPUNIT_ASSERT(removedVectors[0] == vectorObj1);
        CPPUNIT_ASSERT(removedVectors[1] == vectorObj2);
        clearArrays();
    }

    {
        // Test add(), and clear()
        ::fwDataTools::helper::Vector vectorHelper(vector);
        vectorHelper.add(vectorObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(2), vector->size());
        CPPUNIT_ASSERT((*vector)[0] == vectorObj3);
        CPPUNIT_ASSERT((*vector)[1] == vectorObj2);

        vectorHelper.clear();
        CPPUNIT_ASSERT_EQUAL(size_t(0), vector->size());
        CPPUNIT_ASSERT_EQUAL(true, vector->empty());
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numAddedNotif == 3 && numRemovedNotif == 2; });

        CPPUNIT_ASSERT_EQUAL(size_t(1), addedVectors.size());
        CPPUNIT_ASSERT_EQUAL(size_t(2), removedVectors.size());
        CPPUNIT_ASSERT(addedVectors[0] == vectorObj2);
        CPPUNIT_ASSERT(removedVectors[0] == vectorObj3);
        CPPUNIT_ASSERT(removedVectors[1] == vectorObj2);
        clearArrays();
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools
