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

#include "FieldHelperTest.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>

#include <data/String.hpp>
#include <data/tools/helper/Field.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::data::tools::ut::FieldHelperTest );

namespace sight::data::tools
{
namespace ut
{

//------------------------------------------------------------------------------

void FieldHelperTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void FieldHelperTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void FieldHelperTest::testHelper()
{
    const std::string FIELD_ID1 = "FIELD_ID1";
    const std::string FIELD_ID2 = "FIELD_ID2";
    const std::string FIELD_ID3 = "FIELD_ID3";

    data::Object::sptr nullobj;
    data::Object::sptr obj       = data::String::New();
    data::Object::sptr fieldObj1 = data::String::New();
    data::Object::sptr fieldObj2 = data::String::New();
    data::Object::sptr fieldObj3 = data::String::New();

    core::thread::Worker::sptr worker = core::thread::Worker::New();

    // Setup to test notifications
    unsigned int numAddedNotif = 0;
    std::mutex mutex;
    std::condition_variable condition;

    data::Object::FieldsContainerType addedFields;
    std::function<void ( data::Object::FieldsContainerType)> fnAdd =
        [&](data::Object::FieldsContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numAddedNotif;
                addedFields = f;
            }
            condition.notify_one();
        };

    auto slotAdded = core::com::newSlot( fnAdd );
    slotAdded->setWorker(worker);
    auto sigAdded = obj->signal< data::Object::AddedFieldsSignalType>(data::Object::s_ADDED_FIELDS_SIG);
    sigAdded->connect(slotAdded);

    unsigned int numRemovedNotif = 0;
    data::Object::FieldsContainerType removedFields;

    std::function<void ( data::Object::FieldsContainerType)> fnRemove =
        [&](data::Object::FieldsContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numRemovedNotif;
                removedFields = f;
            }
            condition.notify_one();
        };
    auto slotRemoved = core::com::newSlot( fnRemove );
    slotRemoved->setWorker(worker);
    auto sigRemoved = obj->signal< data::Object::RemovedFieldsSignalType>(data::Object::s_REMOVED_FIELDS_SIG);
    sigRemoved->connect(slotRemoved);

    unsigned int numChangedNotif = 0;
    data::Object::FieldsContainerType newFields;
    data::Object::FieldsContainerType oldFields;

    std::function<void ( data::Object::FieldsContainerType, data::Object::FieldsContainerType)> fnChange =
        [&](data::Object::FieldsContainerType newF, data::Object::FieldsContainerType oldF)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numChangedNotif;
                newFields = newF;
                oldFields = oldF;
            }
            condition.notify_one();
        };
    auto slotChanged = core::com::newSlot( fnChange );
    slotChanged->setWorker(worker);
    auto sigChanged = obj->signal< data::Object::ChangedFieldsSignalType>(data::Object::s_CHANGED_FIELDS_SIG);
    sigChanged->connect(slotChanged);

    auto clearArrays = [&]() { addedFields.clear(); removedFields.clear(); newFields.clear(); oldFields.clear(); };

    {
        // Test setField()
        data::tools::helper::Field fieldHelper(obj);
        fieldHelper.setField(FIELD_ID1, fieldObj1);
        fieldHelper.setField(FIELD_ID2, fieldObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(2), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj1);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numAddedNotif == 1; });

        CPPUNIT_ASSERT_EQUAL(size_t(2), addedFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), removedFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), newFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), oldFields.size());
        CPPUNIT_ASSERT(addedFields[FIELD_ID1] == fieldObj1);
        CPPUNIT_ASSERT(addedFields[FIELD_ID2] == fieldObj2);
        clearArrays();
    }

    {
        // Test setFields()
        data::Object::FieldMapType fieldsWithObj1 = { { FIELD_ID1, fieldObj3} };
        data::tools::helper::Field fieldHelper(obj);
        fieldHelper.setFields(fieldsWithObj1);
        CPPUNIT_ASSERT_EQUAL(size_t(1), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj3);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numRemovedNotif == 1 && numChangedNotif == 1; });

        CPPUNIT_ASSERT_EQUAL(size_t(0), addedFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), removedFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), newFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), oldFields.size());
        CPPUNIT_ASSERT(removedFields[FIELD_ID2] == fieldObj2);
        CPPUNIT_ASSERT(oldFields[FIELD_ID1] == fieldObj1);
        CPPUNIT_ASSERT(newFields[FIELD_ID1] == fieldObj3);
        clearArrays();
    }

    {
        // Test replacement with setField()
        data::tools::helper::Field fieldHelper(obj);
        fieldHelper.setField(FIELD_ID1, fieldObj2);
        CPPUNIT_ASSERT_EQUAL(size_t(1), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        fieldHelper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numChangedNotif == 2; });

        CPPUNIT_ASSERT_EQUAL(size_t(0), addedFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(0), removedFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), newFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), oldFields.size());
        CPPUNIT_ASSERT(oldFields[FIELD_ID1] == fieldObj3);
        CPPUNIT_ASSERT(newFields[FIELD_ID1] == fieldObj2);
        clearArrays();
    }

    {
        // Test add(), addOrSwap() and remove()
        data::tools::helper::Field fieldHelper(obj);
        fieldHelper.add(FIELD_ID2, fieldObj1);
        CPPUNIT_ASSERT_EQUAL(size_t(2), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj1);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);

        fieldHelper.addOrSwap(FIELD_ID2, fieldObj3);
        CPPUNIT_ASSERT_EQUAL(size_t(2), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj3);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);

        fieldHelper.remove(FIELD_ID1);
        CPPUNIT_ASSERT_EQUAL(size_t(1), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj3);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);

        fieldHelper.add(FIELD_ID3, fieldObj1);
        CPPUNIT_ASSERT_EQUAL(size_t(2), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj3);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == fieldObj1);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numAddedNotif == 2 && numRemovedNotif == 2 && numChangedNotif == 3; });

        CPPUNIT_ASSERT_EQUAL(size_t(2), addedFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), removedFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), newFields.size());
        CPPUNIT_ASSERT_EQUAL(size_t(1), oldFields.size());
        CPPUNIT_ASSERT(addedFields[FIELD_ID2] == fieldObj1);
        CPPUNIT_ASSERT(addedFields[FIELD_ID3] == fieldObj1);
        CPPUNIT_ASSERT(oldFields[FIELD_ID2] == fieldObj1);
        CPPUNIT_ASSERT(newFields[FIELD_ID2] == fieldObj3);
        CPPUNIT_ASSERT(removedFields[FIELD_ID1] == fieldObj2);
        clearArrays();
    }

    {
        // Test clear()
        data::tools::helper::Field fieldHelper(obj);
        fieldHelper.clear();
        CPPUNIT_ASSERT_EQUAL(size_t(0), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] { return numRemovedNotif == 3; });

        CPPUNIT_ASSERT_EQUAL(size_t(2), removedFields.size());
        CPPUNIT_ASSERT(removedFields[FIELD_ID2] == fieldObj3);
        CPPUNIT_ASSERT(removedFields[FIELD_ID3] == fieldObj1);
        clearArrays();
    }

    worker->stop();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::data::tools
