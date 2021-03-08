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

#include "ObjectTest.hpp"

#include <core/tools/Object.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( sight::core::tools::ut::ObjectTest );

namespace sight::core::tools
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

void ObjectTest::idTest()
{
    const std::string UUID = "uid";

    // process
    core::tools::Object::sptr obj = std::make_shared< core::tools::Object >();

    obj->setID(UUID);

    // check
    CPPUNIT_ASSERT(obj->hasID());
    CPPUNIT_ASSERT_EQUAL(obj->getID(), UUID);
}

} // namespace ut
} // namespace sight::core::tools
