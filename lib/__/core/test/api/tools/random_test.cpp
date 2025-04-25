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

#include "random_test.hpp"

#include <core/tools/random/generator.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::tools::ut::random_test);

namespace sight::core::tools::ut
{

//------------------------------------------------------------------------------

void random_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void random_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void random_test::generator_test()
{
    float min_f  = 10.F;
    float max_f  = 20.F;
    float rand_f = core::tools::random::get_value(min_f, max_f);
    CPPUNIT_ASSERT_MESSAGE("Generate random number not in good interval.", rand_f >= min_f && rand_f <= max_f);

    int min_i  = 10;
    int max_i  = 12;
    int rand_i = core::tools::random::get_value(min_i, max_i);
    CPPUNIT_ASSERT_MESSAGE("Generate random number not in good interval.", rand_i >= min_i && rand_i <= max_i);
}

//------------------------------------------------------------------------------

void random_test::random_list_test()
{
    float min_f = 10.F;
    float max_f = 20.F;
    std::vector<float> rand_f(10);
    core::tools::random::fill_container(min_f, max_f, rand_f);
    for(float i : rand_f)
    {
        CPPUNIT_ASSERT_MESSAGE("Generate random number not in good interval.", i >= min_f && i <= max_f);
    }

    int min_i = 10;
    int max_i = 20;
    std::vector<int> rand_i(10);
    core::tools::random::fill_container(min_i, max_i, rand_i);
    for(std::size_t i = 0 ; i < rand_f.size() ; ++i)
    {
        CPPUNIT_ASSERT_MESSAGE(
            "Generate random number not in good interval.",
            rand_i[i] >= min_i && rand_i[i] <= max_i
        );
    }
}

//------------------------------------------------------------------------------

void random_test::seed_test()
{
    std::uint32_t seed_val = 42;

    float min_f   = 10.F;
    float max_f   = 20.F;
    float rand_f1 = core::tools::random::get_value(min_f, max_f, seed_val);
    CPPUNIT_ASSERT_MESSAGE("Generate random number not in good interval.", rand_f1 >= min_f && rand_f1 <= max_f);
    float rand_f2 = core::tools::random::get_value(min_f, max_f, seed_val);
    CPPUNIT_ASSERT_EQUAL(rand_f1, rand_f2);

    int min_i = 10;
    int max_i = 20;
    std::vector<int> rand_i1(10);
    std::vector<int> rand_i2(10);
    core::tools::random::fill_container(min_i, max_i, rand_i1, seed_val);
    core::tools::random::fill_container(min_i, max_i, rand_i2, seed_val);
    for(std::size_t i = 0 ; i < rand_i1.size() ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(rand_i1[i], rand_i2[i]);
    }
}

} // namespace sight::core::tools::ut
