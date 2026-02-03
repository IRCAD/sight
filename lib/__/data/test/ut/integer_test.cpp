/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/integer.hpp>

#include <doctest/doctest.h>

#include <limits>

TEST_SUITE("sight::data::integer")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        {
            sight::data::integer i;
            CHECK(i.is_type_of("sight::data::integer"));
            CHECK(i.is_type_of("sight::data::string_serializable"));
        }

        const std::array values {
            std::numeric_limits<std::int64_t>::min(),
            std::int64_t(-1654), std::int64_t(0), std::int64_t(123456),
            std::numeric_limits<std::int64_t>::max()
        };

        for(std::int64_t value : values)
        {
            sight::data::integer::sptr i0 = std::make_shared<sight::data::integer>();
            i0->value() = value;
            sight::data::integer::sptr i1 = std::make_shared<sight::data::integer>(value);

            CHECK_EQ(value, i0->value());
            CHECK_EQ(value, i1->value());
            CHECK_EQ(value, std::make_shared<sight::data::integer>(value)->value());

            CHECK(*i0 == *i1);
        }

        for(std::int64_t value : values)
        {
            sight::data::integer i0;
            i0.set_value(value);
            sight::data::integer i1 = value;

            CHECK_EQ(value, i0.value());
            CHECK_EQ(value, i1.value());
            CHECK(i0 == i1);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("string_conversion")
    {
        sight::data::integer i1 = 42;
        CHECK_EQ(std::string("42"), i1.to_string());

        i1 = -26972;
        CHECK_EQ(std::string("-26972"), i1.to_string());

        i1.from_string("45693");
        CHECK(45693 == i1.value());

        CHECK_THROWS_AS(i1.from_string("-7894;-1557.2;48"), boost::bad_lexical_cast);
        CHECK_THROWS_AS(i1.from_string("-7894.489"), boost::bad_lexical_cast);
        CHECK_THROWS_AS(i1.from_string("74vcx7aaa"), boost::bad_lexical_cast);
    }

//------------------------------------------------------------------------------

    TEST_CASE("reset")
    {
        sight::data::integer i1 = 42;
        i1.set_default_value();

        CHECK_EQ(std::int64_t(42), i1.value());
        CHECK_EQ(std::int64_t(42), i1.default_value());

        i1 = 788;
        CHECK_EQ(std::int64_t(788), i1.value());
        CHECK_EQ(std::int64_t(42), i1.default_value());

        i1.reset();
        CHECK_EQ(std::int64_t(42), i1.value());
        CHECK_EQ(std::int64_t(42), i1.default_value());

        i1 = -4788;
        CHECK_EQ(std::int64_t(-4788), i1.value());
        CHECK_EQ(std::int64_t(42), i1.default_value());

        i1.set_value(i1.default_value());
        CHECK_EQ(std::int64_t(42), i1.value());
        CHECK_EQ(std::int64_t(42), i1.default_value());
    }
} // TEST_SUITE("sight::data::integer")
