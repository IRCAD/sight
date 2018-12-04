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


#include <fwTools/Object.hpp>

#include "ObjectTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::ObjectTest );

namespace fwTools
{
namespace ut
{

void ObjectTest::setUp()
{
    // Set up context before running a test.
}

void ObjectTest::tearDown()
{
    // Clean up after the test run.
}

void ObjectTest::idTest()
{
    const std::string UUID = "uid";

    // process
    ::fwTools::Object::sptr obj = std::make_shared< ::fwTools::Object >();

    obj->setID(UUID);

    // check
    CPPUNIT_ASSERT(obj->hasID());
    CPPUNIT_ASSERT_EQUAL(obj->getID(), UUID);
}

} // namespace ut
} // namespace fwTools
