/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/helper/Field.hpp>
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

void field_helper_test::testHelper()
{
    const std::string FIELD_ID1 = "FIELD_ID1";
    const std::string FIELD_ID2 = "FIELD_ID2";
    const std::string FIELD_ID3 = "FIELD_ID3";

    data::object::sptr nullobj;
    data::object::sptr obj       = std::make_shared<data::string>();
    data::object::sptr fieldObj1 = std::make_shared<data::string>();
    data::object::sptr fieldObj2 = std::make_shared<data::string>();
    data::object::sptr fieldObj3 = std::make_shared<data::string>();

    core::thread::worker::sptr worker = core::thread::worker::make();

    // Setup to test notifications
    unsigned int numAddedNotif = 0;
    std::mutex mutex;
    std::condition_variable condition;

    data::object::FieldsContainerType addedFields;
    std::function<void(data::object::FieldsContainerType)> fnAdd =
        [&](data::object::FieldsContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numAddedNotif;
                addedFields = f;
            }
            condition.notify_one();
        };

    auto slotAdded = core::com::new_slot(fnAdd);
    slotAdded->set_worker(worker);
    auto sigAdded = obj->signal<data::object::AddedFieldsSignalType>(data::object::ADDED_FIELDS_SIG);
    sigAdded->connect(slotAdded);

    unsigned int numRemovedNotif = 0;
    data::object::FieldsContainerType removedFields;

    std::function<void(data::object::FieldsContainerType)> fnRemove =
        [&](data::object::FieldsContainerType f)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numRemovedNotif;
                removedFields = f;
            }
            condition.notify_one();
        };
    auto slotRemoved = core::com::new_slot(fnRemove);
    slotRemoved->set_worker(worker);
    auto sigRemoved = obj->signal<data::object::RemovedFieldsSignalType>(data::object::REMOVED_FIELDS_SIG);
    sigRemoved->connect(slotRemoved);

    unsigned int numChangedNotif = 0;
    data::object::FieldsContainerType newFields;
    data::object::FieldsContainerType oldFields;

    std::function<void(data::object::FieldsContainerType, data::object::FieldsContainerType)> fnChange =
        [&](data::object::FieldsContainerType newF, data::object::FieldsContainerType oldF)
        {
            {
                std::unique_lock<std::mutex> lock(mutex);
                ++numChangedNotif;
                newFields = newF;
                oldFields = oldF;
            }
            condition.notify_one();
        };
    auto slotChanged = core::com::new_slot(fnChange);
    slotChanged->set_worker(worker);
    auto sigChanged = obj->signal<data::object::ChangedFieldsSignalType>(data::object::CHANGED_FIELDS_SIG);
    sigChanged->connect(slotChanged);

    auto clearArrays = [&](){addedFields.clear(); removedFields.clear(); newFields.clear(); oldFields.clear();};

    {
        // Test setField()
        data::helper::Field fieldHelper(obj);
        fieldHelper.setField(FIELD_ID1, fieldObj1);
        fieldHelper.setField(FIELD_ID2, fieldObj2);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj1);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numAddedNotif == 1;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(2), addedFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), removedFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), newFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), oldFields.size());
        CPPUNIT_ASSERT(addedFields[FIELD_ID1] == fieldObj1);
        CPPUNIT_ASSERT(addedFields[FIELD_ID2] == fieldObj2);
        clearArrays();
    }

    {
        // Test setFields()
        data::object::FieldMapType fieldsWithObj1 = {{FIELD_ID1, fieldObj3}};
        data::helper::Field fieldHelper(obj);
        fieldHelper.setFields(fieldsWithObj1);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj3);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numRemovedNotif == 1 && numChangedNotif == 1;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(0), addedFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), removedFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), newFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), oldFields.size());
        CPPUNIT_ASSERT(removedFields[FIELD_ID2] == fieldObj2);
        CPPUNIT_ASSERT(oldFields[FIELD_ID1] == fieldObj1);
        CPPUNIT_ASSERT(newFields[FIELD_ID1] == fieldObj3);
        clearArrays();
    }

    {
        // Test replacement with setField()
        data::helper::Field fieldHelper(obj);
        fieldHelper.setField(FIELD_ID1, fieldObj2);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);

        // Notify explicitly, this should change nothing since the destructor skip it in this case
        fieldHelper.notify();
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numChangedNotif == 2;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(0), addedFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), removedFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), newFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), oldFields.size());
        CPPUNIT_ASSERT(oldFields[FIELD_ID1] == fieldObj3);
        CPPUNIT_ASSERT(newFields[FIELD_ID1] == fieldObj2);
        clearArrays();
    }

    {
        // Test add(), addOrSwap() and remove()
        data::helper::Field fieldHelper(obj);
        fieldHelper.add(FIELD_ID2, fieldObj1);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj1);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);

        fieldHelper.addOrSwap(FIELD_ID2, fieldObj3);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj3);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);

        fieldHelper.remove(FIELD_ID1);
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj3);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);

        fieldHelper.add(FIELD_ID3, fieldObj1);
        CPPUNIT_ASSERT_EQUAL(std::size_t(2), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj3);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == fieldObj1);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numAddedNotif == 2 && numRemovedNotif == 2 && numChangedNotif == 3;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(2), addedFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), removedFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), newFields.size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(1), oldFields.size());
        CPPUNIT_ASSERT(addedFields[FIELD_ID2] == fieldObj1);
        CPPUNIT_ASSERT(addedFields[FIELD_ID3] == fieldObj1);
        CPPUNIT_ASSERT(oldFields[FIELD_ID2] == fieldObj1);
        CPPUNIT_ASSERT(newFields[FIELD_ID2] == fieldObj3);
        CPPUNIT_ASSERT(removedFields[FIELD_ID1] == fieldObj2);
        clearArrays();
    }

    {
        // Test clear()
        data::helper::Field fieldHelper(obj);
        fieldHelper.clear();
        CPPUNIT_ASSERT_EQUAL(std::size_t(0), obj->getFields().size());
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }
    {
        // Check notification
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&]{return numRemovedNotif == 3;});

        CPPUNIT_ASSERT_EQUAL(std::size_t(2), removedFields.size());
        CPPUNIT_ASSERT(removedFields[FIELD_ID2] == fieldObj3);
        CPPUNIT_ASSERT(removedFields[FIELD_ID3] == fieldObj1);
        clearArrays();
    }

    worker->stop();
}

//------------------------------------------------------------------------------

} // namespace sight::data::tools::ut
