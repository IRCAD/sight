/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWDATA_UT_COPYTEST_HPP__
#define __FWDATA_UT_COPYTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#define __FWDATA_UT_FIELD_DEEP_COPY_MACRO(ObjType)                                                                        \
    {                                                                                                                     \
        OSLM_WARN("Testing : " << #ObjType);                                                                              \
        ObjType::sptr object = ObjType::New();                                                                            \
        ObjType::sptr deepCopyObject;                                                                                     \
                                                                                                                          \
        ObjType::sptr field1 = ObjType::New();                                                                            \
        ObjType::sptr field2 = ObjType::New();                                                                            \
        ObjType::sptr field3 = ObjType::New();                                                                            \
                                                                                                                          \
        object->setField("F1", field1);                                                                                   \
        object->setField("F2", field2);                                                                                   \
        object->setField("F3", field3);                                                                                   \
                                                                                                                          \
        deepCopyObject = ::fwData::Object::copy(object);                                                                  \
        CPPUNIT_ASSERT_EQUAL( object->getClassname(), deepCopyObject->getClassname() );                                   \
        CPPUNIT_ASSERT_EQUAL( object->getFields().size(), deepCopyObject->getFields().size() );                           \
        CPPUNIT_ASSERT( object->getField("F1") != deepCopyObject->getField("F1") );                                       \
        CPPUNIT_ASSERT( object->getField("F2") != deepCopyObject->getField("F2") );                                       \
        CPPUNIT_ASSERT( object->getField("F3") != deepCopyObject->getField("F3") );                                       \
                                                                                                                          \
        CPPUNIT_ASSERT_EQUAL( object->getField("F1")->getClassname(), deepCopyObject->getField("F1")->getClassname() );   \
        CPPUNIT_ASSERT_EQUAL( object->getField("F2")->getClassname(), deepCopyObject->getField("F2")->getClassname() );   \
        CPPUNIT_ASSERT_EQUAL( object->getField("F3")->getClassname(), deepCopyObject->getField("F3")->getClassname() );   \
    }

#define __FWDATA_UT_FIELD_COPY_MACRO(ObjType)                                                                        \
    {                                                                                                                \
        OSLM_WARN("Testing : " << #ObjType);                                                                         \
        ObjType::sptr object            = ObjType::New();                                                            \
        ObjType::sptr deepCopyObject    = ObjType::New();                                                            \
        ObjType::sptr shallowCopyObject = ObjType::New();                                                            \
                                                                                                                     \
        ObjType::sptr field1 = ObjType::New();                                                                       \
        ObjType::sptr field2 = ObjType::New();                                                                       \
        ObjType::sptr field3 = ObjType::New();                                                                       \
                                                                                                                     \
        object->setField("F1", field1);                                                                              \
        object->setField("F2", field2);                                                                              \
        object->setField("F3", field3);                                                                              \
                                                                                                                     \
        deepCopyObject = ::fwData::Object::copy(object);                                                             \
        CPPUNIT_ASSERT_EQUAL( object->getClassname(), deepCopyObject->getClassname() );                              \
        CPPUNIT_ASSERT_EQUAL( object->getFields().size(), deepCopyObject->getFields().size() );                      \
        CPPUNIT_ASSERT( object->getField("F1") != deepCopyObject->getField("F1") );                                  \
        CPPUNIT_ASSERT( object->getField("F2") != deepCopyObject->getField("F2") );                                  \
        CPPUNIT_ASSERT( object->getField("F3") != deepCopyObject->getField("F3") );                                  \
                                                                                                                     \
        CPPUNIT_ASSERT_EQUAL( object->getField("F1")->getClassname(), deepCopyObject->getField("F1")->getClassname() );  \
        CPPUNIT_ASSERT_EQUAL( object->getField("F2")->getClassname(), deepCopyObject->getField("F2")->getClassname() );  \
        CPPUNIT_ASSERT_EQUAL( object->getField("F3")->getClassname(), deepCopyObject->getField("F3")->getClassname() );  \
                                                                                                                     \
        shallowCopyObject->shallowCopy(object);                                                                      \
        CPPUNIT_ASSERT_EQUAL( object->getClassname(), shallowCopyObject->getClassname() );                           \
        CPPUNIT_ASSERT_EQUAL( object->getFields().size(), shallowCopyObject->getFields().size() );                   \
        CPPUNIT_ASSERT_EQUAL( object->getField("F1"), shallowCopyObject->getField("F1") );                           \
        CPPUNIT_ASSERT_EQUAL( object->getField("F2"), shallowCopyObject->getField("F2") );                           \
        CPPUNIT_ASSERT_EQUAL( object->getField("F3"), shallowCopyObject->getField("F3") );                           \
    }


namespace fwData
{
namespace ut
{

class CopyTest : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( CopyTest );
    CPPUNIT_TEST( fieldCopyTest );
    CPPUNIT_TEST( severalReferencesCopyTest );
    CPPUNIT_TEST( recursiveCopyTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void fieldCopyTest();
    void severalReferencesCopyTest();
    void recursiveCopyTest();
};

} //namespace ut
} //namespace fwData

#endif //__FWDATA_UT_COPYTEST_HPP__
