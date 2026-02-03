/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <data/real.hpp>

#include <doctest/doctest.h>

#include <limits>
#include <numbers>

TEST_SUITE("sight::data::real")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        {
            sight::data::real r;
            CHECK(r.is_type_of("sight::data::real"));
            CHECK(r.is_type_of("sight::data::string_serializable"));
        }

        constexpr std::array values     = {std::numbers::pi, 0.0, 2.04, 10., std::numeric_limits<double>::infinity()};
        constexpr std::array nan_values = {
            std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::signaling_NaN()
        };

        for(double value : values)
        {
            auto d0 = std::make_shared<sight::data::real>();
            d0->value() = value;
            auto d1 = std::make_shared<sight::data::real>(value);
            auto d2 = std::make_shared<sight::data::real>(value + 0.1);
            auto d3 = std::make_shared<sight::data::real>();
            d3->shallow_copy(d1);
            auto d4 = std::make_shared<sight::data::real>();
            d4->deep_copy(d1);

            CHECK(*d0 == *d1);

            if(std::isinf(value))
            {
                CHECK(*d0 == *d2);
            }
            else
            {
                CHECK(*d0 != *d2);
            }

            CHECK_EQ(value, d0->value());
            CHECK_EQ(value, d1->value());
            CHECK_EQ(value, std::make_shared<sight::data::real>(value)->value());
            CHECK_EQ(value, d3->value());
            CHECK_EQ(value, d4->value());
        }

        for(double value : nan_values)
        {
            auto d0 = std::make_shared<sight::data::real>();
            d0->value() = value;
            auto d1 = std::make_shared<sight::data::real>(value);
            auto d2 = std::make_shared<sight::data::real>();
            d2->shallow_copy(d1);
            auto d3 = std::make_shared<sight::data::real>();
            d3->deep_copy(d1);

            // Our implementation of "==" operator for NaN is not the same as the one of the C++ standard
            CHECK(*d0 == *d1);
            CHECK(*d0 == *d2);
            CHECK(*d0 == *d3);

            CHECK(!(value == d0->value()));
            CHECK(!(value < d0->value()));
            CHECK(!(value > d0->value()));

            CHECK(!(value == d1->value()));
            CHECK(!(value < d1->value()));
            CHECK(!(value > d1->value()));

            CHECK(!(value == std::make_shared<sight::data::real>(value)->value()));
            CHECK(!(value < std::make_shared<sight::data::real>(value)->value()));
            CHECK(!(value > std::make_shared<sight::data::real>(value)->value()));
        }
    }
} // TEST_SUITE("sight::data::real")
