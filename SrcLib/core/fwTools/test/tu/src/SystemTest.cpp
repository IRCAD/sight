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

#include "SystemTest.hpp"

#include <fwTools/System.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::SystemTest );

namespace fwTools
{
namespace ut
{

//------------------------------------------------------------------------------

void SystemTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SystemTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SystemTest::genTempFilenameTest()
{
    {
        const std::string filename = ::fwTools::System::genTempFileName();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(64), filename.size());

        for(auto c : filename)
        {
            CPPUNIT_ASSERT( std::isalnum(c) != 0);
        }
    }

    const std::vector<size_t> size = { 234, 0, 1, 36, 98, 2034 };

    for(auto s : size)
    {
        const std::string filename = ::fwTools::System::genTempFileName(s);
        CPPUNIT_ASSERT_EQUAL(s, filename.size());
        for(auto c : filename)
        {
            CPPUNIT_ASSERT( std::isalnum(c) != 0);
        }
    }
}

} // namespace ut
} // namespace fwTools
