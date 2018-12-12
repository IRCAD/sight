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

#include <fwAtoms/String.hpp>

#include <limits>



CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtoms::ut::StringTest );

namespace fwAtoms
{
namespace ut
{

void StringTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void StringTest::tearDown()
{
    // Clean up after the test run.
}


void StringTest::conversion()
{
    const std::string VALUES[] = {"", "azerty"};
    fwAtoms::String::sptr metaString;

    for ( std::string str: VALUES )
    {
        metaString = ::fwAtoms::String::New(str);

        CPPUNIT_ASSERT_EQUAL(str, metaString->getString());
    }
}

}  // namespace ut
}  // namespace fwAtoms

