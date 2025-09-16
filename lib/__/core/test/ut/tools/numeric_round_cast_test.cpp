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

#include <core/tools/numeric_round_cast.hxx>

#include <doctest/doctest.h>

#include <cstdint>

TEST_SUITE("sight::core::tools::numeric_round_cast")
{
//------------------------------------------------------------------------------

    TEST_CASE("cast_float_to_int")
    {
        float val       = 1.32F;
        int casted_val1 = sight::core::tools::numeric_round_cast<int>(val);
        CHECK_EQ(static_cast<int>(1), casted_val1);

        val         = 1.82F;
        casted_val1 = sight::core::tools::numeric_round_cast<int>(val);
        CHECK_EQ(static_cast<int>(2), casted_val1);

        val         = -16.62F;
        casted_val1 = sight::core::tools::numeric_round_cast<int>(val);
        CHECK_EQ(static_cast<int>(-17), casted_val1);

        val = 4826.2F;
        const auto casted_val2 = sight::core::tools::numeric_round_cast<std::uint16_t>(val);
        CHECK_EQ(static_cast<std::uint16_t>(4826), casted_val2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("cast_double_to_int")
    {
        double val1      = 895.33;
        auto casted_val1 = sight::core::tools::numeric_round_cast<unsigned int>(val1);
        CHECK_EQ(static_cast<unsigned int>(895), casted_val1);

        double val2     = -56655.58;
        int casted_val2 = sight::core::tools::numeric_round_cast<int>(val2);
        CHECK_EQ(static_cast<int>(-56656), casted_val2);

        double val3      = 88.9005;
        auto casted_val3 = sight::core::tools::numeric_round_cast<std::uint16_t>(val3);
        CHECK_EQ(static_cast<std::uint16_t>(89), casted_val3);
    }

//------------------------------------------------------------------------------

    TEST_CASE("cast_int_to_float")
    {
        int val1         = 55;
        auto casted_val1 = sight::core::tools::numeric_round_cast<float>(val1);
        CHECK_EQ(doctest::Approx(55.0F), casted_val1);

        int val2         = -4564;
        auto casted_val2 = sight::core::tools::numeric_round_cast<float>(val2);
        CHECK_EQ(doctest::Approx(-4564.0F), casted_val2);

        std::uint16_t val3 = 5394;
        auto casted_val3   = sight::core::tools::numeric_round_cast<double>(val3);
        CHECK_EQ(doctest::Approx(5394.0), casted_val3);
    }

//------------------------------------------------------------------------------

    TEST_CASE("cast_double_to_float")
    {
        double val      = 55.678;
        auto casted_val = sight::core::tools::numeric_round_cast<float>(val);
        CHECK_EQ(doctest::Approx(55.678F), casted_val);

        val        = -4564.851;
        casted_val = sight::core::tools::numeric_round_cast<float>(val);
        CHECK_EQ(doctest::Approx(-4564.851F), casted_val);

        val        = -84.851 / 3.0;
        casted_val = sight::core::tools::numeric_round_cast<float>(val);
        CHECK_EQ(doctest::Approx(-84.851F / 3.0F), casted_val);
    }

//------------------------------------------------------------------------------

    TEST_CASE("cast_int_to_int")
    {
        unsigned int val1 = 153;
        auto casted_val1  = sight::core::tools::numeric_round_cast<unsigned char>(val1);
        CHECK_EQ(static_cast<unsigned char>(153), casted_val1);

        unsigned char val2 = 25;
        auto casted_val2   = sight::core::tools::numeric_round_cast<unsigned char>(val2);
        CHECK_EQ(static_cast<unsigned char>(25), casted_val2);

        char val3                 = 121;
        std::uint16_t casted_val3 = sight::core::tools::numeric_round_cast<unsigned char>(val3);
        CHECK_EQ(static_cast<std::uint16_t>(121), casted_val3);
    }
} // TEST_SUITE
