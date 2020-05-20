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

#include "BooleanTest.hpp"

#include <fwAtoms/Boolean.hpp>

#include <limits>



CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtoms::ut::BooleanTest );

namespace fwAtoms
{
namespace ut
{

void BooleanTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void BooleanTest::tearDown()
{
    // Clean up after the test run.
}


void BooleanTest::conversion()
{
    const bool VALUES[] = {true, false};

    ::fwAtoms::Boolean::sptr meta;

    for ( bool BOOL : VALUES )
    {
        std::string res = BOOL ? "true" : "false";

        meta = ::fwAtoms::Boolean::New(BOOL);

        CPPUNIT_ASSERT_EQUAL(res, meta->getString());
    }
}


}  // namespace ut
}  // namespace fwAtoms

