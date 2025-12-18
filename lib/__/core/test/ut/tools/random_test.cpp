/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/tools/random/generator.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::tools::random")
{
//------------------------------------------------------------------------------

    TEST_CASE("generator")
    {
        float min_f  = 10.F;
        float max_f  = 20.F;
        float rand_f = sight::core::tools::random::get_value(min_f, max_f);
        CHECK_MESSAGE(rand_f >= min_f, "Generate random number not in good interval.");
        CHECK_MESSAGE(rand_f <= max_f, "Generate random number not in good interval.");

        int min_i  = 10;
        int max_i  = 12;
        int rand_i = sight::core::tools::random::get_value(min_i, max_i);
        CHECK_MESSAGE(rand_i >= min_i, "Generate random number not in good interval.");
        CHECK_MESSAGE(rand_i <= max_i, "Generate random number not in good interval.");
    }

//------------------------------------------------------------------------------

    TEST_CASE("random_list")
    {
        float min_f = 10.F;
        float max_f = 20.F;
        std::vector<float> rand_f(10);
        sight::core::tools::random::fill_container(min_f, max_f, rand_f);
        for(float i : rand_f)
        {
            CHECK_MESSAGE(i >= min_f, "Generate random number not in good interval.");
            CHECK_MESSAGE(i <= max_f, "Generate random number not in good interval.");
        }

        int min_i = 10;
        int max_i = 20;
        std::vector<int> rand_i(10);
        sight::core::tools::random::fill_container(min_i, max_i, rand_i);
        for(std::size_t i = 0 ; i < rand_f.size() ; ++i)
        {
            CHECK_MESSAGE(rand_i[i] >= min_i, "Generate random number not in good interval.");
            CHECK_MESSAGE(rand_i[i] <= max_i, "Generate random number not in good interval.");
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("seed")
    {
        std::uint32_t seed_val = 42;

        float min_f   = 10.F;
        float max_f   = 20.F;
        float rand_f1 = sight::core::tools::random::get_value(min_f, max_f, seed_val);
        CHECK_MESSAGE(rand_f1 >= min_f, "Generate random number not in good interval.");
        CHECK_MESSAGE(rand_f1 <= max_f, "Generate random number not in good interval.");
        float rand_f2 = sight::core::tools::random::get_value(min_f, max_f, seed_val);
        CHECK_EQ(rand_f1, rand_f2);

        int min_i = 10;
        int max_i = 20;
        std::vector<int> rand_i1(10);
        std::vector<int> rand_i2(10);
        sight::core::tools::random::fill_container(min_i, max_i, rand_i1, seed_val);
        sight::core::tools::random::fill_container(min_i, max_i, rand_i2, seed_val);
        for(std::size_t i = 0 ; i < rand_i1.size() ; ++i)
        {
            CHECK_EQ(rand_i1[i], rand_i2[i]);
        }
    }
} // TEST_SUITE
