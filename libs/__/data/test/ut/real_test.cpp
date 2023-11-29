/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "real_test.hpp"

#include <data/real.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::real_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void real_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void real_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void real_test::basic()
{
    constexpr std::array values =
    {-3.14159265358979323851, 0.0, 2.04, 10., std::numeric_limits<double>::infinity()};
    constexpr std::array nan_values = {
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::signaling_NaN()
    };

    for(double value : values)
    {
        auto d0 = std::make_shared<data::real>();
        d0->value() = value;
        auto d1 = std::make_shared<data::real>(value);
        auto d2 = std::make_shared<data::real>(value + 0.1);
        auto d3 = std::make_shared<data::real>();
        d3->shallow_copy(d1);
        auto d4 = std::make_shared<data::real>();
        d4->deep_copy(d1);

        CPPUNIT_ASSERT(*d0 == *d1);

        if(std::isinf(value))
        {
            CPPUNIT_ASSERT(*d0 == *d2);
        }
        else
        {
            CPPUNIT_ASSERT(*d0 != *d2);
        }

        CPPUNIT_ASSERT_EQUAL(value, d0->value());
        CPPUNIT_ASSERT_EQUAL(value, d1->value());
        CPPUNIT_ASSERT_EQUAL(value, std::make_shared<data::real>(value)->value());
        CPPUNIT_ASSERT_EQUAL(value, d3->value());
        CPPUNIT_ASSERT_EQUAL(value, d4->value());
    }

    for(double value : nan_values)
    {
        auto d0 = std::make_shared<data::real>();
        d0->value() = value;
        auto d1 = std::make_shared<data::real>(value);
        auto d2 = std::make_shared<data::real>();
        d2->shallow_copy(d1);
        auto d3 = std::make_shared<data::real>();
        d3->deep_copy(d1);

        // Our implementation of "==" operator for NaN is not the same as the one of the C++ standard
        CPPUNIT_ASSERT(*d0 == *d1);
        CPPUNIT_ASSERT(*d0 == *d2);
        CPPUNIT_ASSERT(*d0 == *d3);

        CPPUNIT_ASSERT(!(value == d0->value()));
        CPPUNIT_ASSERT(!(value < d0->value()));
        CPPUNIT_ASSERT(!(value > d0->value()));

        CPPUNIT_ASSERT(!(value == d1->value()));
        CPPUNIT_ASSERT(!(value < d1->value()));
        CPPUNIT_ASSERT(!(value > d1->value()));

        CPPUNIT_ASSERT(!(value == std::make_shared<data::real>(value)->value()));
        CPPUNIT_ASSERT(!(value < std::make_shared<data::real>(value)->value()));
        CPPUNIT_ASSERT(!(value > std::make_shared<data::real>(value)->value()));
    }
}

} // namespace sight::data::ut
