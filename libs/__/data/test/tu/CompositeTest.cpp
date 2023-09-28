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

#include "CompositeTest.hpp"

#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/Integer.hpp>
#include <data/String.hpp>

#include <utility>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::CompositeTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void CompositeTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void CompositeTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void CompositeTest::methode1()
{
    const std::int64_t value = 404;
    using pair_type = data::Composite::value_type;
    const std::array<pair_type, 5> PAIRS = {
        std::make_pair("Composite", std::make_shared<data::Composite>()),
        std::make_pair("boolean true", std::make_shared<data::Boolean>(true)),
        std::make_pair("boolean false", std::make_shared<data::Boolean>(false)),
        std::make_pair("float", std::make_shared<data::Float>(3.14F)),
        std::make_pair("integer", std::make_shared<data::Integer>(value))
    };

    data::Composite::container_type stdmap;

    data::Composite::sptr composite = std::make_shared<data::Composite>();

    CPPUNIT_ASSERT(composite->empty());

    for(const pair_type& p : PAIRS)
    {
        (*composite)[p.first] = p.second;
    }

    stdmap.insert(composite->begin(), composite->end());

    CPPUNIT_ASSERT(composite->size() == stdmap.size());

    for(const pair_type& p : *composite)
    {
        CPPUNIT_ASSERT(stdmap[p.first] == (*composite)[p.first]);
    }

    CPPUNIT_ASSERT_EQUAL(true, std::dynamic_pointer_cast<data::Boolean>((*composite)["boolean true"])->value());
    CPPUNIT_ASSERT_EQUAL(false, std::dynamic_pointer_cast<data::Boolean>((*composite)["boolean false"])->value());
    CPPUNIT_ASSERT_EQUAL(3.14F, std::dynamic_pointer_cast<data::Float>((*composite)["float"])->value());
    CPPUNIT_ASSERT_EQUAL(value, std::dynamic_pointer_cast<data::Integer>((*composite)["integer"])->value());

    // test values
    const std::string STR  = "string value";
    data::Object::sptr obj = std::make_shared<data::Float>();

    (*composite)[STR] = obj;

    CPPUNIT_ASSERT(composite->begin() != composite->end());

    CPPUNIT_ASSERT(composite->find(STR) != composite->end());
    CPPUNIT_ASSERT_EQUAL((*composite)[STR], obj);
}

} // namespace sight::data::ut
