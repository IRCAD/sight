/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "EdgeTest.hpp"

#include <data/Edge.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::data::ut::EdgeTest );

namespace sight::data
{
namespace ut
{

//------------------------------------------------------------------------------

void EdgeTest::setUp()
{
    // Set up context before running a test.

}
//------------------------------------------------------------------------------

void EdgeTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void EdgeTest::methode1()
{
    //-----------test values
    const std::string STR1 = "foo";
    const std::string STR2 = "bar";
    const std::string STR3 = data::Edge::NATURE_FLOW;
    data::Edge::sptr edge  = data::Edge::New();

    edge->setIdentifiers(STR1, STR2);
    edge->setNature(STR3);

    CPPUNIT_ASSERT_EQUAL(edge->getFromPortID(), STR1);
    CPPUNIT_ASSERT_EQUAL(edge->getToPortID(), STR2);
    CPPUNIT_ASSERT_EQUAL(edge->getNature(), STR3);
}

} //namespace ut
} //namespace sight::data
