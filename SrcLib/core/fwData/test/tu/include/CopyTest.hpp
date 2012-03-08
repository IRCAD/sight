/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_UT_COPYTEST_HPP__
#define __FWDATA_UT_COPYTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#define __FWDATA_UT_FIELD_COPY_MACRO(ObjType)                                                                                     \
    {                                                                                                                             \
    OSLM_WARN("Testing : " << #ObjType);                                                                                          \
    ObjType::sptr object = ObjType::New();                                                                                        \
    ObjType::sptr deepCopyObject = ObjType::New();                                                                                \
    ObjType::sptr shallowCopyObject = ObjType::New();                                                                             \
                                                                                                                                  \
    ObjType::sptr field1 = ObjType::New();                                                                                        \
    ObjType::sptr field2 = ObjType::New();                                                                                        \
    ObjType::sptr field3 = ObjType::New();                                                                                        \
                                                                                                                                  \
    object->setField_NEWAPI("F1", field1);                                                                                        \
    object->setField_NEWAPI("F2", field2);                                                                                        \
    object->setField_NEWAPI("F3", field3);                                                                                        \
                                                                                                                                  \
    deepCopyObject->deepCopy(object);                                                                                             \
    CPPUNIT_ASSERT_EQUAL( object->getClassname(), deepCopyObject->getClassname() );                                               \
    CPPUNIT_ASSERT_EQUAL( object->getFields_NEWAPI().size(), deepCopyObject->getFields_NEWAPI().size() );                         \
    CPPUNIT_ASSERT( object->getField_NEWAPI("F1") != deepCopyObject->getField_NEWAPI("F1") );                                     \
    CPPUNIT_ASSERT( object->getField_NEWAPI("F2") != deepCopyObject->getField_NEWAPI("F2") );                                     \
    CPPUNIT_ASSERT( object->getField_NEWAPI("F3") != deepCopyObject->getField_NEWAPI("F3") );                                     \
                                                                                                                                  \
    CPPUNIT_ASSERT_EQUAL( object->getField_NEWAPI("F1")->getClassname(), deepCopyObject->getField_NEWAPI("F1")->getClassname() ); \
    CPPUNIT_ASSERT_EQUAL( object->getField_NEWAPI("F2")->getClassname(), deepCopyObject->getField_NEWAPI("F2")->getClassname() ); \
    CPPUNIT_ASSERT_EQUAL( object->getField_NEWAPI("F3")->getClassname(), deepCopyObject->getField_NEWAPI("F3")->getClassname() ); \
                                                                                                                                  \
    shallowCopyObject->shallowCopy(object);                                                                                       \
    CPPUNIT_ASSERT_EQUAL( object->getClassname(), shallowCopyObject->getClassname() );                                            \
    CPPUNIT_ASSERT_EQUAL( object->getFields_NEWAPI().size(), shallowCopyObject->getFields_NEWAPI().size() );                      \
    CPPUNIT_ASSERT_EQUAL( object->getField_NEWAPI("F1"), shallowCopyObject->getField_NEWAPI("F1") );                              \
    CPPUNIT_ASSERT_EQUAL( object->getField_NEWAPI("F2"), shallowCopyObject->getField_NEWAPI("F2") );                              \
    CPPUNIT_ASSERT_EQUAL( object->getField_NEWAPI("F3"), shallowCopyObject->getField_NEWAPI("F3") );                              \
    }


namespace fwData
{
namespace ut
{

class CopyTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( CopyTest );
    CPPUNIT_TEST( fieldCopyTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void fieldCopyTest();
};

} //namespace ut
} //namespace fwData

#endif //__FWDATA_UT_COPYTEST_HPP__
