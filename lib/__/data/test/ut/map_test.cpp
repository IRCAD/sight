/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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
#include <data/integer.hpp>
#include <data/map.hpp>
#include <data/real.hpp>
#include <data/string.hpp>

#include <doctest/doctest.h>

#include <utility>

TEST_SUITE("sight::data::map")
{
//------------------------------------------------------------------------------

    TEST_CASE("methode1")
    {
        const std::int64_t value = 404;
        using pair_type = sight::data::map::value_type;
        const std::array<pair_type, 5> pairs = {
            std::make_pair("map", std::make_shared<sight::data::map>()),
            std::make_pair("boolean true", std::make_shared<sight::data::boolean>(true)),
            std::make_pair("boolean false", std::make_shared<sight::data::boolean>(false)),
            std::make_pair("float", std::make_shared<sight::data::real>(3.14)),
            std::make_pair("integer", std::make_shared<sight::data::integer>(value))
        };

        sight::data::map::container_t stdmap;

        sight::data::map::sptr map = std::make_shared<sight::data::map>();

        CHECK(map->empty());

        for(const pair_type& p : pairs)
        {
            (*map)[p.first] = p.second;
        }

        stdmap.insert(map->begin(), map->end());

        CHECK(map->size() == stdmap.size());

        for(const pair_type& p : *map)
        {
            CHECK(stdmap[p.first] == (*map)[p.first]);
        }

        CHECK_EQ(true, std::dynamic_pointer_cast<sight::data::boolean>((*map)["boolean true"])->value());
        CHECK_EQ(false, std::dynamic_pointer_cast<sight::data::boolean>((*map)["boolean false"])->value());
        CHECK_EQ(3.14, std::dynamic_pointer_cast<sight::data::real>((*map)["float"])->value());
        CHECK_EQ(value, std::dynamic_pointer_cast<sight::data::integer>((*map)["integer"])->value());

        // test values
        const std::string str         = "string value";
        sight::data::object::sptr obj = std::make_shared<sight::data::real>();

        (*map)[str] = obj;

        CHECK(map->begin() != map->end());

        CHECK(map->find(str) != map->end());
        CHECK_EQ((*map)[str], obj);
    }
} // TEST_SUITE("sight::data::map")
