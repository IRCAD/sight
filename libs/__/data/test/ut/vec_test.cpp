/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "vec_test.hpp"

#include <data/dvec2.hpp>
#include <data/dvec3.hpp>
#include <data/dvec4.hpp>
#include <data/integer.hpp>
#include <data/ivec2.hpp>
#include <data/ivec3.hpp>
#include <data/ivec4.hpp>

#include <utility>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::vec_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void vec_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void vec_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void vec_test::basic()
{
    {
        sight::data::dvec2 v2;
        CPPUNIT_ASSERT(v2.is_type_of("sight::data::dvec2"));
        CPPUNIT_ASSERT(v2.is_type_of("sight::data::string_serializable"));
        sight::data::dvec3 v3;
        CPPUNIT_ASSERT(v3.is_type_of("sight::data::dvec3"));
        CPPUNIT_ASSERT(v3.is_type_of("sight::data::string_serializable"));
        sight::data::dvec4 v4;
        CPPUNIT_ASSERT(v4.is_type_of("sight::data::dvec4"));
        CPPUNIT_ASSERT(v4.is_type_of("sight::data::string_serializable"));

        sight::data::ivec2 i2;
        CPPUNIT_ASSERT(i2.is_type_of("sight::data::ivec2"));
        CPPUNIT_ASSERT(i2.is_type_of("sight::data::string_serializable"));
        sight::data::ivec3 i3;
        CPPUNIT_ASSERT(i3.is_type_of("sight::data::ivec3"));
        CPPUNIT_ASSERT(i3.is_type_of("sight::data::string_serializable"));
        sight::data::ivec4 i4;
        CPPUNIT_ASSERT(i4.is_type_of("sight::data::ivec4"));
        CPPUNIT_ASSERT(i4.is_type_of("sight::data::string_serializable"));
    }

    sight::data::dvec2 v1;
    v1 = sight::vec2d_t({0.2, 0.8});
    CPPUNIT_ASSERT(sight::vec2d_t({0.2, 0.8}) == v1.value());

    sight::data::dvec2 v2 = sight::vec2d_t({0.2, 0.8});
    CPPUNIT_ASSERT(v1 == v2);
    CPPUNIT_ASSERT(!(v1 != v2));

    v2 = sight::vec2d_t({0.2, 0.1});
    CPPUNIT_ASSERT(v1 != v2);

    sight::data::ivec4 v3({1, 2, 3, -4});
    CPPUNIT_ASSERT_EQUAL(std::int64_t(1), v3[0]);
    CPPUNIT_ASSERT_EQUAL(std::int64_t(2), v3[1]);
    CPPUNIT_ASSERT_EQUAL(std::int64_t(3), v3[2]);
    CPPUNIT_ASSERT_EQUAL(std::int64_t(-4), v3[3]);

    v3[1] = -12;
    CPPUNIT_ASSERT_EQUAL(std::int64_t(-12), v3[1]);
}

//------------------------------------------------------------------------------

void vec_test::copy()
{
    auto p3 = std::make_shared<sight::data::ivec3>();
    auto p4 = std::make_shared<sight::data::ivec3>();

    *p3 = sight::ivec3_t({255, 0, -4});
    p4->shallow_copy(p3);

    CPPUNIT_ASSERT(*p3 == *p4);
    CPPUNIT_ASSERT(sight::ivec3_t({255, 0, -4}) == p4->value());

    // We start the inception
    auto p5 = std::make_shared<sight::data::integer>();
    p3->set_field("test_field", p5);

    CPPUNIT_ASSERT(*p3 != *p4);
    p4->shallow_copy(p3);

    CPPUNIT_ASSERT(*p3 == *p4);

    *p5 = 1234;
    CPPUNIT_ASSERT(*p3 == *p4);

    p4->deep_copy(p3);
    CPPUNIT_ASSERT(*p3 == *p4);

    // TODO: When two objects compare properly their content
    // *p5 = 4321;
    // CPPUNIT_ASSERT(*p3 != *p4);
}

//------------------------------------------------------------------------------

void vec_test::string_conversion()
{
    sight::data::dvec2 v1;
    v1 = sight::vec2d_t({0.2, 0.8});
    CPPUNIT_ASSERT_EQUAL(std::string("0.200000;0.800000"), v1.to_string());

    sight::data::dvec3 v2;
    v2.from_string("-0.76;0.2;0.8");
    CPPUNIT_ASSERT(sight::vec3d_t({-0.76, 0.2, 0.8}) == v2.value());

    v2.from_string(" -0.8; 0.5; 0.2  ");
    CPPUNIT_ASSERT(sight::vec3d_t({-0.8, 0.5, 0.2}) == v2.value());

    sight::data::ivec3 v3;
    CPPUNIT_ASSERT_THROW(v3.from_string("-7894;-1557.2;48"), boost::bad_lexical_cast);
    CPPUNIT_ASSERT_THROW(v3.from_string("-7894;1557;foo"), boost::bad_lexical_cast);
    CPPUNIT_ASSERT_THROW(v3.from_string("7894;45;48;12"), sight::data::exception);
}

//------------------------------------------------------------------------------

void vec_test::reset()
{
    sight::data::dvec2 v1;
    v1 = sight::vec2d_t({0.2, 0.8});

    CPPUNIT_ASSERT(sight::vec2d_t({0.2, 0.8}) == v1.value());
    CPPUNIT_ASSERT(sight::vec2d_t({0., 0.}) == v1.default_value());

    v1.set_default_value();
    CPPUNIT_ASSERT(sight::vec2d_t({0.2, 0.8}) == v1.value());
    CPPUNIT_ASSERT(sight::vec2d_t({0.2, 0.8}) == v1.default_value());

    v1 = sight::vec2d_t({-0.76, 0.2});
    CPPUNIT_ASSERT(sight::vec2d_t({-0.76, 0.2}) == v1.value());
    CPPUNIT_ASSERT(sight::vec2d_t({0.2, 0.8}) == v1.default_value());

    v1.reset();
    CPPUNIT_ASSERT(sight::vec2d_t({0.2, 0.8}) == v1.value());
    CPPUNIT_ASSERT(sight::vec2d_t({0.2, 0.8}) == v1.default_value());
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
