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

#include <fwTools/Stringizer.hpp>

#include "StringizerTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::StringizerTest );

namespace fwTools
{
namespace ut
{

void StringizerTest::setUp()
{
    // Set up context before running a test.
}

void StringizerTest::tearDown()
{
    // Clean up after the test run.
}

void StringizerTest::convertTest()
{
    const std::string STR1 = "[1,2,3]";
    const std::string STR2 = "6";
    const double D         = 6.0;
    std::vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);

    std::string res1 = ::fwTools::getString(vec.begin(), vec.end());
    std::string res2 = ::fwTools::getString<double>(D);

    // check
    CPPUNIT_ASSERT_EQUAL(res1, STR1);
    CPPUNIT_ASSERT_EQUAL(res2, STR2);
}

} // namespace ut
} // namespace fwTools

