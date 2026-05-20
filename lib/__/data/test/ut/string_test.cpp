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

#include <data/string.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::string")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        {
            sight::data::string s;
            CHECK(s.is_type_of("sight::data::string"));
            CHECK(s.is_type_of("sight::data::string_serializable"));
        }

        const std::array<std::string, 2> values = {"", "example_string"};

        for(const std::string& value : values)
        {
            sight::data::string::sptr s0 = std::make_shared<sight::data::string>();
            s0->value() = value;
            sight::data::string::sptr s1 = std::make_shared<sight::data::string>(value);
            sight::data::string::sptr s2 = std::make_shared<sight::data::string>(value + "other");

            CHECK(*s0 == *s1);
            CHECK(*s0 != *s2);
            CHECK_EQ(value, s0->value());
            CHECK_EQ(value, s1->value());
            CHECK_EQ(value, std::make_shared<sight::data::string>(value)->value());
        }
    }
} // TEST_SUITE("sight::data::string")
