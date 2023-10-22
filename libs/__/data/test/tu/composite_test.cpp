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

#include "composite_test.hpp"

#include <data/boolean.hpp>
#include <data/composite.hpp>
#include <data/integer.hpp>
#include <data/real.hpp>
#include <data/string.hpp>

#include <utility>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::composite_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void composite_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void composite_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void composite_test::methode1()
{
    const std::int64_t value = 404;
    using pair_type = data::composite::value_type;
    const std::array<pair_type, 5> pairs = {
        std::make_pair("Composite", std::make_shared<data::composite>()),
        std::make_pair("boolean true", std::make_shared<data::boolean>(true)),
        std::make_pair("boolean false", std::make_shared<data::boolean>(false)),
        std::make_pair("float", std::make_shared<data::real>(3.14)),
        std::make_pair("integer", std::make_shared<data::integer>(value))
    };

    data::composite::container_type stdmap;

    data::composite::sptr composite = std::make_shared<data::composite>();

    CPPUNIT_ASSERT(composite->empty());

    for(const pair_type& p : pairs)
    {
        (*composite)[p.first] = p.second;
    }

    stdmap.insert(composite->begin(), composite->end());

    CPPUNIT_ASSERT(composite->size() == stdmap.size());

    for(const pair_type& p : *composite)
    {
        CPPUNIT_ASSERT(stdmap[p.first] == (*composite)[p.first]);
    }

    CPPUNIT_ASSERT_EQUAL(true, std::dynamic_pointer_cast<data::boolean>((*composite)["boolean true"])->value());
    CPPUNIT_ASSERT_EQUAL(false, std::dynamic_pointer_cast<data::boolean>((*composite)["boolean false"])->value());
    CPPUNIT_ASSERT_EQUAL(3.14, std::dynamic_pointer_cast<data::real>((*composite)["float"])->value());
    CPPUNIT_ASSERT_EQUAL(value, std::dynamic_pointer_cast<data::integer>((*composite)["integer"])->value());

    // test values
    const std::string str  = "string value";
    data::object::sptr obj = std::make_shared<data::real>();

    (*composite)[str] = obj;

    CPPUNIT_ASSERT(composite->begin() != composite->end());

    CPPUNIT_ASSERT(composite->find(str) != composite->end());
    CPPUNIT_ASSERT_EQUAL((*composite)[str], obj);
}

} // namespace sight::data::ut
