/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "CompositeTest.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>

#include <utility>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::CompositeTest );

namespace fwData
{
namespace ut
{

//------------------------------------------------------------------------------

void CompositeTest::setUp()
{
    // Set up context before running a test.

}
//------------------------------------------------------------------------------

void CompositeTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void CompositeTest::methode1()
{
    const std::int64_t value = 404;
    typedef ::fwData::Composite::value_type pair_type;
    const pair_type PAIRS[] = {
        std::make_pair( "Composite", ::fwData::Composite::New()    ),
        std::make_pair( "boolean true", ::fwData::Boolean::New(true)  ),
        std::make_pair( "boolean false", ::fwData::Boolean::New(false) ),
        std::make_pair( "float", ::fwData::Float::New(3.14f)   ),
        std::make_pair( "integer", ::fwData::Integer::New(value)   )
    };

    ::fwData::Composite::ContainerType stdmap;

    ::fwData::Composite::sptr composite = ::fwData::Composite::New();

    CPPUNIT_ASSERT( composite->size() == 0 );

    for( pair_type p : PAIRS)
    {
        composite->getContainer()[p.first] = p.second;
    }

    stdmap.insert(composite->begin(), composite->end());

    CPPUNIT_ASSERT( composite->size() == stdmap.size() );

    for( pair_type p : *composite)
    {
        CPPUNIT_ASSERT( composite->getContainer()[p.first] == (*composite)[p.first] );
        CPPUNIT_ASSERT( stdmap[p.first] == (*composite)[p.first] );
    }

    CPPUNIT_ASSERT_EQUAL( true, ::fwData::Boolean::dynamicCast((*composite)[ "boolean true" ])->value());
    CPPUNIT_ASSERT_EQUAL( false, ::fwData::Boolean::dynamicCast((*composite)[ "boolean false" ])->value());
    CPPUNIT_ASSERT_EQUAL( 3.14f, ::fwData::Float::dynamicCast((*composite)[ "float" ])->value());
    CPPUNIT_ASSERT_EQUAL( value, ::fwData::Integer::dynamicCast((*composite)[ "integer" ])->value());

    // test values
    const std::string STR = "string value";
    ::fwData::Object::sptr obj = ::fwData::Float::New();

    composite->getContainer()[STR] = obj;

    CPPUNIT_ASSERT( composite->begin() != composite->end() );

    CPPUNIT_ASSERT(composite->find(STR) != composite->end());
    CPPUNIT_ASSERT_EQUAL(composite->getContainer()[STR], obj);
}

//------------------------------------------------------------------------------

void CompositeTest::setGetContainerTest()
{
    std::map< std::string, ::fwData::String::sptr > myStdMap;
    const std::string key1 = "string value 1";
    const std::string key2 = "string value 2";
    myStdMap[ key1 ] = ::fwData::String::New("lolo");
    myStdMap[ key2 ] = ::fwData::String::New("lulu");

    ::fwData::Composite::sptr myDataMap = ::fwData::Composite::New();
    myDataMap->setDataContainer( myStdMap );
    CPPUNIT_ASSERT_EQUAL( static_cast< size_t >(2), myDataMap->size() );

    std::map< std::string, ::fwData::String::sptr > myStdMap2;
    myStdMap2 = myDataMap->getDataContainer< ::fwData::String >();
    CPPUNIT_ASSERT( myStdMap2[key1] == myStdMap[key1] );
    CPPUNIT_ASSERT( myStdMap2[key2] == myStdMap[key2] );
}

} //namespace ut
} //namespace fwData
