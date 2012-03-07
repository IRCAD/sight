/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <utility>
#include <boost/foreach.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Boolean.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>

#include "CompositeTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::CompositeTest );

namespace fwData
{
namespace ut
{

void CompositeTest::setUp()
{
    // Set up context before running a test.

}
void CompositeTest::tearDown()
{
    // Clean up after the test run.
}

void CompositeTest::methode1()
{
    typedef ::fwData::Composite::value_type pair_type;
    const pair_type PAIRS[] = {
        std::make_pair( "object"       , ::fwData::Object::New()       ),
        std::make_pair( "boolean true" , ::fwData::Boolean::New(true)  ),
        std::make_pair( "boolean false", ::fwData::Boolean::New(false) ),
        std::make_pair( "float"        , ::fwData::Float::New(3.14f)    ),
        std::make_pair( "integer"      , ::fwData::Integer::New(404)   )
    };

    ::fwData::Composite::ContainerType stdmap;

    ::fwData::Composite::NewSptr composite;

    CPPUNIT_ASSERT( composite->size() == 0 );

    BOOST_FOREACH( pair_type p, PAIRS)
    {
        composite->getRefMap()[p.first] = p.second;
    }

    stdmap.insert(composite->begin(), composite->end());

    CPPUNIT_ASSERT( composite->size() == stdmap.size() );

    BOOST_FOREACH( pair_type p, *composite)
    {
        CPPUNIT_ASSERT( composite->getRefMap()[p.first] == (*composite)[p.first] );
        CPPUNIT_ASSERT(                 stdmap[p.first] == (*composite)[p.first] );
    }


    CPPUNIT_ASSERT_EQUAL( true ,  ::fwData::Boolean::dynamicCast((*composite)[ "boolean true" ])->value()  ) ;
    CPPUNIT_ASSERT_EQUAL( false,  ::fwData::Boolean::dynamicCast((*composite)[ "boolean false" ])->value() ) ;
    CPPUNIT_ASSERT_EQUAL( 3.14f ,   ::fwData::Float::dynamicCast((*composite)[ "float" ])->value()         ) ;
    CPPUNIT_ASSERT_EQUAL( 404  ,  ::fwData::Integer::dynamicCast((*composite)[ "integer" ])->value()       ) ;

    //-----------test values
    const std::string STR = "toto";
    ::fwData::Object::sptr obj = ::fwData::Object::New();

    composite->getRefMap()[STR] = obj;

    CPPUNIT_ASSERT( composite->begin() != composite->end() );

    CPPUNIT_ASSERT(composite->getRefMap().find(STR) != composite->getRefMap().end());
    CPPUNIT_ASSERT_EQUAL(composite->getRefMap()[STR], obj);
}

} //namespace ut
} //namespace fwData

