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

#include <data/boolean.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::boolean")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        sight::data::boolean b1(true);
        CHECK_EQ(true, b1.value());
        CHECK(b1.is_type_of("sight::data::boolean"));
        CHECK(b1.is_type_of("sight::data::string_serializable"));

        sight::data::boolean b2;
        CHECK_EQ(false, b2.value());
        b2 = true;
        CHECK_EQ(true, b2.value());

        CHECK(b1 == b2);

        const bool true_value  = true;
        const bool false_value = !true_value;

        auto p1 = std::make_shared<sight::data::boolean>(true_value);
        auto p2 = std::make_shared<sight::data::boolean>(false_value);

        // check
        CHECK_EQ(true_value, std::make_shared<sight::data::boolean>(true_value)->value());
        CHECK_EQ(false_value, std::make_shared<sight::data::boolean>(false_value)->value());

        CHECK_EQ(true_value, p1->value());
        CHECK_EQ(false_value, p2->value());

        CHECK(*p1 != *p2);

        p2->set_value(true_value);

        CHECK(*p1 == *p2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("string_conversion")
    {
        sight::data::boolean b1;
        CHECK_EQ(false, b1.value());
        CHECK_EQ(std::string("false"), b1.to_string());

        b1 = true;
        CHECK_EQ(std::string("true"), b1.to_string());

        sight::data::boolean b2;
        CHECK_EQ(false, b2.value());
        b2.from_string("true");
        CHECK_EQ(true, b2.value());

        b2.from_string("false");
        CHECK_EQ(false, b2.value());

        CHECK_THROWS_AS(b2.from_string("test"), sight::data::exception);

        std::stringstream stream;
        stream << b2;
        CHECK_EQ(std::string("false"), stream.str());
    }
} // TEST_SUITE("sight::data::boolean")
