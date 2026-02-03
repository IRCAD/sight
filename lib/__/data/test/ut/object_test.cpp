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

#include <data/mt/locked_ptr.hpp>
#include <data/real.hpp>

#include <doctest/doctest.h>

#include <algorithm>

TEST_SUITE("sight::data::object")
{
//------------------------------------------------------------------------------

    TEST_CASE("field")
    {
        const std::string field_i_d1 = "FIELD_ID1";
        const std::string field_i_d2 = "FIELD_ID2";
        const std::string field_i_d3 = "FIELD_ID3";

        sight::data::object::sptr nullobj;
        sight::data::object::sptr obj        = std::make_shared<sight::data::real>();
        sight::data::object::sptr field_obj1 = std::make_shared<sight::data::real>();
        sight::data::object::sptr field_obj2 = std::make_shared<sight::data::real>();

        CHECK(obj->get_fields().empty());

        obj->set_field(field_i_d1, field_obj1);
        CHECK_EQ(obj->get_fields().size(), std::size_t(1));
        CHECK_EQ(obj->get_field(field_i_d1), field_obj1);
        CHECK_EQ(obj->get_field(field_i_d2), nullobj);
        CHECK_EQ(obj->get_field(field_i_d3), nullobj);

        sight::data::object::field_map_t local_fields_backup = obj->get_fields();

        obj->set_field(field_i_d2, field_obj2);
        CHECK_EQ(obj->get_fields().size(), std::size_t(2));
        CHECK_EQ(obj->get_field(field_i_d1), field_obj1);
        CHECK_EQ(obj->get_field(field_i_d2), field_obj2);
        CHECK_EQ(obj->get_field(field_i_d3), nullobj);

        sight::data::object::field_name_vector_t ref_field_names;
        ref_field_names.push_back(field_i_d1);
        ref_field_names.push_back(field_i_d2);
        sight::data::object::field_name_vector_t field_names = obj->get_field_names();
        std::ranges::sort(field_names);
        std::ranges::sort(ref_field_names);
        CHECK(ref_field_names == field_names);

        obj->set_fields(local_fields_backup);
        CHECK_EQ(obj->get_fields().size(), std::size_t(1));
        CHECK_EQ(obj->get_field(field_i_d1), field_obj1);
        CHECK_EQ(obj->get_field(field_i_d2), nullobj);
        CHECK_EQ(obj->get_field(field_i_d3), nullobj);

        field_names = obj->get_field_names();
        ref_field_names.clear();
        ref_field_names.push_back(field_i_d1);
        CHECK(ref_field_names == field_names);

        obj->set_field(field_i_d1, field_obj2);
        CHECK_EQ(obj->get_fields().size(), std::size_t(1));
        CHECK_EQ(obj->get_field(field_i_d1), field_obj2);
        CHECK_EQ(obj->get_field(field_i_d2), nullobj);
        CHECK_EQ(obj->get_field(field_i_d3), nullobj);

        obj->remove_field(field_i_d1);
        CHECK(obj->get_fields().empty());

        sight::data::object::sptr default_field = obj->set_default_field(field_i_d1, field_obj1);
        CHECK(default_field == field_obj1);

        default_field = obj->set_default_field(field_i_d1, field_obj2);
        CHECK(default_field != field_obj2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("last_modify")
    {
        sight::data::object::sptr object = std::make_shared<sight::data::real>();

        CHECK_EQ(std::uint64_t(0), object->last_modified());

        {
            sight::data::mt::locked_ptr lock(object);
            CHECK_EQ(std::uint64_t(1), object->last_modified());
        }

        for(std::uint32_t i = 0 ; i < 199 ; ++i)
        {
            sight::data::mt::locked_ptr lock(object);
            CHECK_EQ(std::uint64_t(i + 2), object->last_modified());
        }

        CHECK_EQ(std::uint64_t(200), object->last_modified());
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto object1 = std::make_shared<sight::data::real>();
        auto object2 = std::make_shared<sight::data::real>();

        CHECK(*object1 == *object2);
        CHECK(!(*object1 != *object2));

        object1->set_description("1");
        CHECK(*object1 != *object2);
        CHECK(!(*object1 == *object2));
        object2->set_description(object1->get_description());
        CHECK(*object1 == *object2);
        CHECK(!(*object1 != *object2));
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::object")
