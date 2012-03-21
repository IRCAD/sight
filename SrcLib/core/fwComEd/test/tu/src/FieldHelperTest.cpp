/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Object.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Video.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwComEd/helper/Field.hpp>

#include "ConfigParserTest.hpp"
#include "FieldHelperTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FieldHelperTest );

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
    ::fwData::Object::sptr obj = ::fwData::Object::New();
    ::fwData::Object::sptr fieldObj1 = ::fwData::Object::New();
    ::fwData::Object::sptr fieldObj2 = ::fwData::Object::New();

    {
        ::fwComEd::helper::Field fieldHelper(obj);
        fieldHelper.setField(FIELD_ID1, fieldObj1);
        CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);
    }

    ::fwData::Object::FieldMapType localFields = obj->getFields();
    ::fwData::Object::FieldMapType localFieldsBackup = obj->getFields();
    localFields.insert( ::fwData::Object::FieldMapType::value_type(FIELD_ID2,fieldObj2));

    {
        ::fwComEd::helper::Field fieldHelper(obj);
        fieldHelper.updateFields(localFields);
        CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(2));
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), fieldObj2);
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);
    }

    {
        ::fwComEd::helper::Field fieldHelper(obj);
        fieldHelper.setFields(localFieldsBackup);
        CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj1);
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);
    }

    {
        ::fwComEd::helper::Field fieldHelper(obj);
        fieldHelper.setField(FIELD_ID1, fieldObj2);
        CPPUNIT_ASSERT_EQUAL(obj->getFields().size(), size_t(1));
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID1), fieldObj2);
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID2), nullobj);
        CPPUNIT_ASSERT_EQUAL(obj->getField(FIELD_ID3), nullobj);
    }
}

//------------------------------------------------------------------------------
