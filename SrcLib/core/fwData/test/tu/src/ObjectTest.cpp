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

void ObjectTest::methode1()
{
    const int MIN   = 1 ;
    const int MAX   = 14 ;
    const int VALUE = 13 ;
    const std::string FIELDID1 = "fieldid1" ;

    ::fwData::Object::NewSptr  o1;
    ::fwTools::Field::sptr f1 (::fwTools::Field::New() );
    ::fwTools::Field::sptr f0 ; //shared_ptr NULL
    f1 = o1->setField(FIELDID1) ;

    // check
    CPPUNIT_ASSERT_EQUAL( o1->getField(FIELDID1),   f1 );
}

void ObjectTest::methode2()
{
    const int MIN   = 1 ;
    const int MAX   = 14 ;
    const int VALUE = 13 ;
    const std::string FIELDID1 = "fieldid1" ;

    ::fwData::Object::NewSptr  o1;
    ::fwTools::Field::sptr f1(::fwTools::Field::New());
    ::fwData::Object::NewSptr  o2;
    f1 = o1->setFieldSingleElement(FIELDID1,  o2) ;

    // check
    CPPUNIT_ASSERT_EQUAL( o1->getField(FIELDID1),   f1 );
}

void ObjectTest::methode3()
{
    const int MIN   = 1 ;
    const int MAX   = 14 ;
    const int VALUE = 13 ;
    const std::string FIELDID1 = "fieldid1" ;
    const std::string FIELDID2 = "fieldid2" ;

    ::fwData::Object::NewSptr o1;
    ::fwTools::Field::sptr f1(::fwTools::Field::New());
    ::fwTools::Field::sptr f0 ;
    ::fwData::Object::NewSptr  o2;
    f1 = o1->addFieldElement(FIELDID1, o2) ;

    // check
    CPPUNIT_ASSERT_EQUAL( o1->getField(FIELDID1),   f1 );
    CPPUNIT_ASSERT_EQUAL( o1->getField(FIELDID2),   f0 );
    CPPUNIT_ASSERT_EQUAL( o1->getField(FIELDID1),   f1 );
}

} //namespace ut
} //namespace fwData
