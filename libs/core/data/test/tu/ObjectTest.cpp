/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "ObjectTest.hpp"

#include <data/Float.hpp>
#include <data/mt/locked_ptr.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ObjectTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void ObjectTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ObjectTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ObjectTest::fieldTest()
{
    const std::string FIELD_ID1 = "FIELD_ID1";
    const std::string FIELD_ID2 = "FIELD_ID2";
    const std::string FIELD_ID3 = "FIELD_ID3";

    data::Object::sptr nullobj;
    data::Object::sptr obj       = data::Float::New();
    data::Object::sptr fieldObj1 = data::Float::New();
    data::Object::sptr fieldObj2 = data::Float::New();

    CPPUNIT_ASSERT(obj->getFields().empty());

    obj->setField(FIELD_ID1, fieldObj1);
    CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), std::size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);

    data::Object::FieldMapType localFieldsBackup = obj->getFields();

    obj->setField(FIELD_ID2, fieldObj2);
    CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), std::size_t(2));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), fieldObj2);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);

    data::Object::FieldNameVectorType refFieldNames;
    refFieldNames.push_back(FIELD_ID1);
    refFieldNames.push_back(FIELD_ID2);
    data::Object::FieldNameVectorType fieldNames = obj->getFieldNames();
    std::sort(fieldNames.begin(), fieldNames.end());
    std::sort(refFieldNames.begin(), refFieldNames.end());
    CPPUNIT_ASSERT(refFieldNames == fieldNames);

    obj->setFields(localFieldsBackup);
    CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), std::size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);

    fieldNames = obj->getFieldNames();
    refFieldNames.clear();
    refFieldNames.push_back(FIELD_ID1);
    CPPUNIT_ASSERT(refFieldNames == fieldNames);

    obj->setField(FIELD_ID1, fieldObj2);
    CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), std::size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj2);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);

    obj->removeField(FIELD_ID1);
    CPPUNIT_ASSERT(obj->getFields().empty());

    data::Object::sptr defaultField = obj->setDefaultField(FIELD_ID1, fieldObj1);
    CPPUNIT_ASSERT(defaultField == fieldObj1);

    defaultField = obj->setDefaultField(FIELD_ID1, fieldObj2);
    CPPUNIT_ASSERT(defaultField != fieldObj2);
}

//------------------------------------------------------------------------------

void ObjectTest::lastModifyTest()
{
    data::Object::sptr object = data::Float::New();

    CPPUNIT_ASSERT_EQUAL(std::uint64_t(0), object->lastModified());

    {
        data::mt::locked_ptr lock(object);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(1), object->lastModified());
    }

    for(std::uint32_t i = 0 ; i < 199 ; ++i)
    {
        data::mt::locked_ptr lock(object);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(i + 2), object->lastModified());
    }

    CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), object->lastModified());
}

//------------------------------------------------------------------------------

void ObjectTest::equalityTest()
{
    auto object1 = data::Float::New();
    auto object2 = data::Float::New();

    CPPUNIT_ASSERT(*object1 == *object2 && !(*object1 != *object2));

    object1->setDescription("1");
    CPPUNIT_ASSERT(*object1 != *object2 && !(*object1 == *object2));
    object2->setDescription(object1->getDescription());
    CPPUNIT_ASSERT(*object1 == *object2 && !(*object1 != *object2));
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
