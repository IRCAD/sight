/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwData/String.hpp"
#include "StringTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( StringTest );

void StringTest::setUp()
{
    // Set up context before running a test.
}

void StringTest::tearDown()
{
    // Clean up after the test run.
}

void StringTest::methode1()
{
    const std::string VALUES[]  = { "", "chaine_caractere" };

    BOOST_FOREACH ( std::string VALUE, VALUES )
    {
        ::fwData::String::sptr s0 = ::fwData::String::New();
        s0->value() = VALUE;
        ::fwData::String::NewSptr s1( VALUE );

        CPPUNIT_ASSERT_EQUAL( VALUE , s0->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE , s1->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE , ::fwData::String::New( VALUE )->value() );
    }
}
