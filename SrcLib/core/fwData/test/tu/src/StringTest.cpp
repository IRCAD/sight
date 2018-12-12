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

#include "StringTest.hpp"

#include <fwData/String.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::StringTest );

namespace fwData
{
namespace ut
{

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
    const std::string VALUES[] = { "", "chaine_caractere" };

    for( std::string VALUE : VALUES )
    {
        ::fwData::String::sptr s0 = ::fwData::String::New();
        s0->value()               = VALUE;
        ::fwData::String::sptr s1 = ::fwData::String::New( VALUE );

        CPPUNIT_ASSERT_EQUAL( VALUE, s0->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, s1->value() );
        CPPUNIT_ASSERT_EQUAL( VALUE, ::fwData::String::New( VALUE )->value() );
    }
}

} //namespace ut
} //namespace fwData
