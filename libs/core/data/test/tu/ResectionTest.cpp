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

#include "ResectionTest.hpp"

#include <data/Reconstruction.hpp>
#include <data/Resection.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ResectionTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void ResectionTest::equalityTest()
{
    auto resection1 = data::Resection::New();
    auto resection2 = data::Resection::New();

    CPPUNIT_ASSERT(*resection1 == *resection2 && !(*resection1 != *resection2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
    resection1->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Resections should be different when using " #op " on the first one", \
        *resection1 != *resection2 && !(*resection1 == *resection2) \
    ); \
    resection2->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Resections should be equal when using " #op " on both", \
        *resection1 == *resection2 && !(*resection1 != *resection2) \
    );

    auto planeList = data::PlaneList::New();
    planeList->setPlanes({data::Plane::New()});
    TEST(setPlaneList(planeList));
    TEST(setInputs({data::Reconstruction::New()}));
    TEST(setOutputs({data::Reconstruction::New()}));
    TEST(setIsSafePart(false));
    TEST(setName("1"));
    TEST(setIsVisible(false));
    TEST(setIsValid(true));

    #undef TEST
}

} // namespace sight::data::ut
