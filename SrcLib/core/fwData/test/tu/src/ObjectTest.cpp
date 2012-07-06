/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Object.hpp>

#include "ObjectTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::ObjectTest );


namespace fwData
{
namespace ut
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

    ::fwData::Object::sptr nullobj;
    ::fwData::Object::sptr obj = ::fwData::Object::New();
    ::fwData::Object::sptr fieldObj1 = ::fwData::Object::New();
    ::fwData::Object::sptr fieldObj2 = ::fwData::Object::New();

    CPPUNIT_ASSERT(obj->getFields().empty());

    obj->setField(FIELD_ID1, fieldObj1);
    CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);

    ::fwData::Object::FieldMapType localFields = obj->getFields();
    ::fwData::Object::FieldMapType localFieldsBackup = obj->getFields();
    localFields.insert( ::fwData::Object::FieldMapType::value_type(FIELD_ID2,fieldObj2));

    CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);

    obj->updateFields(localFields);
    CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(2));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), fieldObj2);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);

    ::fwData::Object::FieldNameVectorType refFieldNames;
    refFieldNames.push_back(FIELD_ID1);
    refFieldNames.push_back(FIELD_ID2);
    ::fwData::Object::FieldNameVectorType fieldNames = obj->getFieldNames();
    CPPUNIT_ASSERT(refFieldNames == fieldNames);

    obj->setFields(localFieldsBackup);
    CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);

    fieldNames = obj->getFieldNames();
    refFieldNames.clear();
    refFieldNames.push_back(FIELD_ID1);
    CPPUNIT_ASSERT(refFieldNames == fieldNames);

    obj->setField(FIELD_ID1, fieldObj2);
    CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj2);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);

    obj->removeField(FIELD_ID1);
    CPPUNIT_ASSERT(obj->getFields().empty());

    ::fwData::Object::sptr defaultField = obj->setDefaultField(FIELD_ID1, fieldObj1);
    CPPUNIT_ASSERT(defaultField == fieldObj1);

    defaultField = obj->setDefaultField(FIELD_ID1, fieldObj2);
    CPPUNIT_ASSERT(defaultField != fieldObj2);
}

} //namespace ut
} //namespace fwData
