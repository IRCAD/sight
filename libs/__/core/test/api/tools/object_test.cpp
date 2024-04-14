/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "object_test.hpp"

#include <core/tools/object.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::object_test);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void object_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void object_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void object_test::id_test()
{
    const std::string uuid = "uid";

    // process
    core::tools::object::sptr obj = std::make_shared<core::tools::object>();

    obj->set_id(uuid);

    // check
    CPPUNIT_ASSERT(obj->has_id());
    CPPUNIT_ASSERT_EQUAL(obj->get_id(), uuid);
}

} // namespace sight::core::tools::ut
