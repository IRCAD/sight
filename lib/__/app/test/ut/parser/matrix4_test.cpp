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

#include <data/matrix4.hpp>

#include <app/parser/matrix4.hpp>

#include <boost/property_tree/ptree.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::app::parser::Matrix4")
{
//------------------------------------------------------------------------------

    TEST_CASE("basic")
    {
        boost::property_tree::ptree ptree;
        ptree.put("matrix", R"(
        0 1 2 3
        10 11 12 13
        20 21 22 23
        30 31 32 33
    )");
        auto matrix = std::make_shared<sight::data::matrix4>();
        sight::app::parser::matrix4 matrix_parser;
        CHECK(matrix_parser.is_a("sight::app::parser::matrix4"));
        sight::service::object_parser::objects_t sub_objects;
        matrix_parser.parse(ptree, matrix, sub_objects);
        for(std::uint8_t i = 0 ; i < 4 ; i++)
        {
            for(std::uint8_t j = 0 ; j < 4 ; j++)
            {
                CHECK_EQ(j + 10. * i, (*matrix)(i, j));
            }
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
