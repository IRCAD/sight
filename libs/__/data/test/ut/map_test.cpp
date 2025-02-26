/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "map_test.hpp"

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/map.hpp>
#include <data/real.hpp>
#include <data/string.hpp>

#include <utility>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::map_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void map_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void map_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void map_test::methode1()
{
    const std::int64_t value = 404;
    using pair_type = data::map::value_type;
    const std::array<pair_type, 5> pairs = {
        std::make_pair("map", std::make_shared<data::map>()),
        std::make_pair("boolean true", std::make_shared<data::boolean>(true)),
        std::make_pair("boolean false", std::make_shared<data::boolean>(false)),
        std::make_pair("float", std::make_shared<data::real>(3.14)),
        std::make_pair("integer", std::make_shared<data::integer>(value))
    };

    data::map::container_t stdmap;

    data::map::sptr map = std::make_shared<data::map>();

    CPPUNIT_ASSERT(map->empty());

    for(const pair_type& p : pairs)
    {
        (*map)[p.first] = p.second;
    }

    stdmap.insert(map->begin(), map->end());

    CPPUNIT_ASSERT(map->size() == stdmap.size());

    for(const pair_type& p : *map)
    {
        CPPUNIT_ASSERT(stdmap[p.first] == (*map)[p.first]);
    }

    CPPUNIT_ASSERT_EQUAL(true, std::dynamic_pointer_cast<data::boolean>((*map)["boolean true"])->value());
    CPPUNIT_ASSERT_EQUAL(false, std::dynamic_pointer_cast<data::boolean>((*map)["boolean false"])->value());
    CPPUNIT_ASSERT_EQUAL(3.14, std::dynamic_pointer_cast<data::real>((*map)["float"])->value());
    CPPUNIT_ASSERT_EQUAL(value, std::dynamic_pointer_cast<data::integer>((*map)["integer"])->value());

    // test values
    const std::string str  = "string value";
    data::object::sptr obj = std::make_shared<data::real>();

    (*map)[str] = obj;

    CPPUNIT_ASSERT(map->begin() != map->end());

    CPPUNIT_ASSERT(map->find(str) != map->end());
    CPPUNIT_ASSERT_EQUAL((*map)[str], obj);
}

} // namespace sight::data::ut
