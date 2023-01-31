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

#include "PlaneTest.hpp"

#include <data/Plane.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::PlaneTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void PlaneTest::equalityTest()
{
    auto plane1 = data::Plane::New();
    auto plane2 = data::Plane::New();

    CPPUNIT_ASSERT(*plane1 == *plane2 && !(*plane1 != *plane2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
    plane1->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Planes should be different when " #op " of the first plane is different", \
        *plane1 != *plane2 && !(*plane1 == *plane2) \
    ); \
    plane2->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Planes should be equal when " #op " of both plane are equal", \
        *plane1 == *plane2 && !(*plane1 != *plane2) \
    );

    TEST(setPoints({data::Point::New(1., 2., 3.), data::Point::New(4., 5., 6.), data::Point::New(7., 8., 9.)}));
    TEST(setIsIntersection(false));

    #undef TEST
}

} // namespace sight::data::ut
