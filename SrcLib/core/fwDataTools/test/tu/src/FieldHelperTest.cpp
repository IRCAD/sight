/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "FieldHelperTest.hpp"

#include <fwData/String.hpp>

#include <fwDataTools/helper/Field.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataTools::ut::FieldHelperTest );

namespace fwDataTools
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

    ::fwData::Object::sptr nullobj;
    ::fwData::Object::sptr obj       = ::fwData::String::New();
    ::fwData::Object::sptr fieldObj1 = ::fwData::String::New();
    ::fwData::Object::sptr fieldObj2 = ::fwData::String::New();

    {
        ::fwDataTools::helper::Field fieldHelper(obj);
        fieldHelper.setField(FIELD_ID1, fieldObj1);
        CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj1);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }

    ::fwData::Object::FieldMapType localFields       = obj->getFields();
    ::fwData::Object::FieldMapType localFieldsBackup = obj->getFields();
    localFields.insert( ::fwData::Object::FieldMapType::value_type(FIELD_ID2,fieldObj2));

    {
        ::fwDataTools::helper::Field fieldHelper(obj);
        fieldHelper.updateFields(localFields);
        CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(2));
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj1);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }

    {
        ::fwDataTools::helper::Field fieldHelper(obj);
        fieldHelper.setFields(localFieldsBackup);
        CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj1);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }

    {
        ::fwDataTools::helper::Field fieldHelper(obj);
        fieldHelper.setField(FIELD_ID1, fieldObj2);
        CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
        CPPUNIT_ASSERT(obj->getField(FIELD_ID1) == fieldObj2);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID2) == nullobj);
        CPPUNIT_ASSERT(obj->getField(FIELD_ID3) == nullobj);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDataTools
