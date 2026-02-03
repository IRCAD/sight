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

#include <data/array.hpp>
#include <data/boolean.hpp>
#include <data/color.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/line.hpp>
#include <data/map.hpp>
#include <data/material.hpp>
#include <data/matrix4.hpp>
#include <data/mesh.hpp>
#include <data/plane.hpp>
#include <data/plane_list.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>
#include <data/reconstruction.hpp>
#include <data/resection.hpp>
#include <data/resection_db.hpp>
#include <data/string.hpp>
#include <data/structure_traits.hpp>
#include <data/structure_traits_dictionary.hpp>
#include <data/transfer_function.hpp>
#include <data/vector.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::copy")
{
//-----------------------------------------------------------------------------

    template<typename T>
    static void field_deep_copy()
    {
        SIGHT_WARN("Testing : " << sight::core::demangler(typeid(T)).get_classname());
        typename T::sptr object = std::make_shared<T>();
        typename T::sptr deep_copy_object;

        typename T::sptr field1 = std::make_shared<T>();
        typename T::sptr field2 = std::make_shared<T>();
        typename T::sptr field3 = std::make_shared<T>();

        object->set_field("F1", field1);
        object->set_field("F2", field2);
        object->set_field("F3", field3);

        deep_copy_object = sight::data::object::copy(object);
        CHECK_EQ(object->get_classname(), deep_copy_object->get_classname());
        CHECK_EQ(object->get_fields().size(), deep_copy_object->get_fields().size());
        CHECK(object->get_field("F1") != deep_copy_object->get_field("F1"));
        CHECK(object->get_field("F2") != deep_copy_object->get_field("F2"));
        CHECK(object->get_field("F3") != deep_copy_object->get_field("F3"));

        CHECK_EQ(object->get_field("F1")->get_classname(), deep_copy_object->get_field("F1")->get_classname());
        CHECK_EQ(object->get_field("F2")->get_classname(), deep_copy_object->get_field("F2")->get_classname());
        CHECK_EQ(object->get_field("F3")->get_classname(), deep_copy_object->get_field("F3")->get_classname());
    }

//------------------------------------------------------------------------------

    template<typename T>
    static void field_copy()
    {
        SIGHT_WARN("Testing : " << sight::core::demangler(typeid(T)).get_classname());
        typename T::sptr object              = std::make_shared<T>();
        typename T::sptr deep_copy_object    = std::make_shared<T>();
        typename T::sptr shallow_copy_object = std::make_shared<T>();

        typename T::sptr field1 = std::make_shared<T>();
        typename T::sptr field2 = std::make_shared<T>();
        typename T::sptr field3 = std::make_shared<T>();

        object->set_field("F1", field1);
        object->set_field("F2", field2);
        object->set_field("F3", field3);

        deep_copy_object = sight::data::object::copy(object);
        CHECK_EQ(object->get_classname(), deep_copy_object->get_classname());
        CHECK_EQ(object->get_fields().size(), deep_copy_object->get_fields().size());
        CHECK(object->get_field("F1") != deep_copy_object->get_field("F1"));
        CHECK(object->get_field("F2") != deep_copy_object->get_field("F2"));
        CHECK(object->get_field("F3") != deep_copy_object->get_field("F3"));

        CHECK_EQ(object->get_field("F1")->get_classname(), deep_copy_object->get_field("F1")->get_classname());
        CHECK_EQ(object->get_field("F2")->get_classname(), deep_copy_object->get_field("F2")->get_classname());
        CHECK_EQ(object->get_field("F3")->get_classname(), deep_copy_object->get_field("F3")->get_classname());

        shallow_copy_object->shallow_copy(object);
        CHECK_EQ(object->get_classname(), shallow_copy_object->get_classname());
        CHECK_EQ(object->get_fields().size(), shallow_copy_object->get_fields().size());
        CHECK_EQ(object->get_field("F1"), shallow_copy_object->get_field("F1"));
        CHECK_EQ(object->get_field("F2"), shallow_copy_object->get_field("F2"));
        CHECK_EQ(object->get_field("F3"), shallow_copy_object->get_field("F3"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("field_copy")
    {
        field_deep_copy<sight::data::array>();
        field_copy<sight::data::boolean>();
        field_copy<sight::data::color>();
        field_copy<sight::data::map>();
        field_copy<sight::data::real>();
        field_copy<sight::data::image>();
        field_copy<sight::data::integer>();
        field_copy<sight::data::line>();
        field_copy<sight::data::material>();
        field_copy<sight::data::mesh>();
        field_copy<sight::data::plane>();
        field_copy<sight::data::plane_list>();
        field_copy<sight::data::point>();
        field_copy<sight::data::point_list>();
        field_copy<sight::data::reconstruction>();
        field_copy<sight::data::resection>();
        field_copy<sight::data::resection_db>();
        field_copy<sight::data::string>();
        field_copy<sight::data::transfer_function>();
        field_copy<sight::data::matrix4>();
        field_copy<sight::data::vector>();
        field_copy<sight::data::structure_traits>();
        field_copy<sight::data::structure_traits_dictionary>();
    }

//-----------------------------------------------------------------------------

    TEST_CASE("several_references_copy")
    {
        const std::int64_t value           = 42;
        sight::data::integer::sptr integer = std::make_shared<sight::data::integer>(value);
        sight::data::map::sptr map         = std::make_shared<sight::data::map>();

        (*map)["A"] = integer;
        (*map)["B"] = integer;
        map->set_field("F1", integer);
        map->set_field("F2", integer);

        sight::data::map::sptr map_copy = sight::data::object::copy(map);

        CHECK(integer != std::dynamic_pointer_cast<sight::data::integer>((*map_copy)["A"]));
        CHECK_EQ(value, std::dynamic_pointer_cast<sight::data::integer>((*map_copy)["A"])->get_value());
        CHECK_EQ((*map_copy)["A"], map_copy->get_field("F1"));
        CHECK_EQ((*map_copy)["A"], map_copy->get_field("F2"));
        CHECK_EQ((*map_copy)["A"], (*map_copy)["B"]);

        sight::data::vector::sptr vector = std::make_shared<sight::data::vector>();

        vector->push_back(map);
        vector->push_back(map);
        vector->set_field("F1", map);
        vector->set_field("F2", map);

        sight::data::vector::sptr vector_copy = sight::data::object::copy(vector);
        CHECK(map != (*vector_copy)[0]);
        CHECK_EQ((*vector_copy)[0], vector_copy->get_field("F1"));
        CHECK_EQ((*vector_copy)[0], vector_copy->get_field("F2"));
        CHECK_EQ((*vector_copy)[0], (*vector_copy)[1]);
    }
} // TEST_SUITE("sight::data::copy")
