/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "DoubleTest.hpp"

#include <data/Double.hpp>

#include <limits>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::DoubleTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void DoubleTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DoubleTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DoubleTest::methode1()
{
    constexpr std::array VALUES =
    {-3.14159265358979323851, 0.0, 2.04, 10., std::numeric_limits<double>::infinity()};
    constexpr std::array NAN_VALUES = {
        std::numeric_limits<double>::quiet_NaN(),
        std::numeric_limits<double>::signaling_NaN()
    };

    for(double VALUE : VALUES)
    {
        auto d0 = data::Double::New();
        d0->value() = VALUE;
        auto d1 = data::Double::New(VALUE);
        auto d2 = data::Double::New(VALUE + 0.1);
        auto d3 = data::Double::New();
        d3->shallowCopy(d1);
        auto d4 = data::Double::New();
        d4->deepCopy(d1);

        CPPUNIT_ASSERT(*d0 == *d1);

        if(std::isinf(VALUE))
        {
            CPPUNIT_ASSERT(*d0 == *d2);
        }
        else
        {
            CPPUNIT_ASSERT(*d0 != *d2);
        }

        CPPUNIT_ASSERT_EQUAL(VALUE, d0->value());
        CPPUNIT_ASSERT_EQUAL(VALUE, d1->value());
        CPPUNIT_ASSERT_EQUAL(VALUE, data::Double::New(VALUE)->value());
        CPPUNIT_ASSERT_EQUAL(VALUE, d3->value());
        CPPUNIT_ASSERT_EQUAL(VALUE, d4->value());
    }

    for(double VALUE : NAN_VALUES)
    {
        auto d0 = data::Double::New();
        d0->value() = VALUE;
        auto d1 = data::Double::New(VALUE);
        auto d2 = data::Double::New();
        d2->shallowCopy(d1);
        auto d3 = data::Double::New();
        d3->deepCopy(d1);

        // Our implementation of "==" operator for NaN is not the same as the one of the C++ standard
        CPPUNIT_ASSERT(*d0 == *d1);
        CPPUNIT_ASSERT(*d0 == *d2);
        CPPUNIT_ASSERT(*d0 == *d3);

        CPPUNIT_ASSERT(!(VALUE == d0->value()));
        CPPUNIT_ASSERT(!(VALUE < d0->value()));
        CPPUNIT_ASSERT(!(VALUE > d0->value()));

        CPPUNIT_ASSERT(!(VALUE == d1->value()));
        CPPUNIT_ASSERT(!(VALUE < d1->value()));
        CPPUNIT_ASSERT(!(VALUE > d1->value()));

        CPPUNIT_ASSERT(!(VALUE == data::Double::New(VALUE)->value()));
        CPPUNIT_ASSERT(!(VALUE < data::Double::New(VALUE)->value()));
        CPPUNIT_ASSERT(!(VALUE > data::Double::New(VALUE)->value()));
    }
}

} // namespace sight::data::ut
