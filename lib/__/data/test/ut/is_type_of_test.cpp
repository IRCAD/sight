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
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/map.hpp>
#include <data/real.hpp>
#include <data/transfer_function.hpp>

#include <doctest/doctest.h>

#include <utility>

TEST_SUITE("sight::data::is_type_of")
{
//------------------------------------------------------------------------------

    TEST_CASE("check_type")
    {
        sight::data::map::sptr data_map              = std::make_shared<sight::data::map>();
        sight::data::boolean::sptr data_boolean      = std::make_shared<sight::data::boolean>();
        sight::data::real::sptr data_float           = std::make_shared<sight::data::real>();
        sight::data::integer::sptr data_integer      = std::make_shared<sight::data::integer>();
        sight::data::image::sptr data_image          = std::make_shared<sight::data::image>();
        sight::data::transfer_function::sptr data_tf = std::make_shared<sight::data::transfer_function>();

        sight::data::map::is_type_of("sight::data::object");
        sight::data::boolean::is_type_of("sight::data::object");
        sight::data::real::is_type_of("sight::data::object");
        sight::data::integer::is_type_of("sight::data::object");
        sight::data::image::is_type_of("sight::data::object");
        sight::data::transfer_function::is_type_of("sight::data::object");

        sight::data::map::is_type_of("sight::data::map");
        sight::data::boolean::is_type_of("sight::data::boolean");
        sight::data::real::is_type_of("sight::data::real");
        sight::data::integer::is_type_of("sight::data::integer");
        sight::data::image::is_type_of("sight::data::image");
        sight::data::transfer_function::is_type_of("sight::data::transfer_function");

        CHECK(data_map->is_a("sight::data::object"));
        CHECK(data_boolean->is_a("sight::data::object"));
        CHECK(data_float->is_a("sight::data::object"));
        CHECK(data_integer->is_a("sight::data::object"));
        CHECK(data_image->is_a("sight::data::object"));
        CHECK(data_tf->is_a("sight::data::object"));

        CHECK(data_map->is_a("sight::data::map"));
        CHECK(data_boolean->is_a("sight::data::boolean"));
        CHECK(data_float->is_a("sight::data::real"));
        CHECK(data_integer->is_a("sight::data::integer"));
        CHECK(data_image->is_a("sight::data::image"));
        CHECK(data_tf->is_a("sight::data::transfer_function"));

        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_map)->is_a("sight::data::object"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_boolean)->is_a("sight::data::object"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_float)->is_a("sight::data::object"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_integer)->is_a("sight::data::object"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_image)->is_a("sight::data::object"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_tf)->is_a("sight::data::object"));

        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_map)->is_a("sight::data::map"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_boolean)->is_a("sight::data::boolean"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_float)->is_a("sight::data::real"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_integer)->is_a("sight::data::integer"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_image)->is_a("sight::data::image"));
        CHECK(std::dynamic_pointer_cast<sight::data::object>(data_tf)->is_a("sight::data::transfer_function"));

        CHECK(!data_map->is_a("sight::data::image"));
        CHECK(!data_boolean->is_a("sight::data::image"));
        CHECK(!data_float->is_a("sight::data::image"));
        CHECK(!data_integer->is_a("sight::data::image"));
        CHECK(!data_image->is_a("sight::data::boolean"));
        CHECK(!data_tf->is_a("sight::data::image"));
    }
} // TEST_SUITE("sight::data::is_type_of")
