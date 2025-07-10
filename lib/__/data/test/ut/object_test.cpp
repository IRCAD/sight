/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "object_test.hpp"

#include <data/mt/locked_ptr.hpp>
#include <data/real.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::object_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void object_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void object_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void object_test::field_test()
{
    const std::string field_i_d1 = "FIELD_ID1";
    const std::string field_i_d2 = "FIELD_ID2";
    const std::string field_i_d3 = "FIELD_ID3";

    data::object::sptr nullobj;
    data::object::sptr obj        = std::make_shared<data::real>();
    data::object::sptr field_obj1 = std::make_shared<data::real>();
    data::object::sptr field_obj2 = std::make_shared<data::real>();

    CPPUNIT_ASSERT(obj->get_fields().empty());

    obj->set_field(field_i_d1, field_obj1);
    CPPUNIT_ASSERT_EQUAL(obj->get_fields().size(), std::size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d1), field_obj1);
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d3), nullobj);

    data::object::field_map_t local_fields_backup = obj->get_fields();

    obj->set_field(field_i_d2, field_obj2);
    CPPUNIT_ASSERT_EQUAL(obj->get_fields().size(), std::size_t(2));
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d1), field_obj1);
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d2), field_obj2);
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d3), nullobj);

    data::object::field_name_vector_t ref_field_names;
    ref_field_names.push_back(field_i_d1);
    ref_field_names.push_back(field_i_d2);
    data::object::field_name_vector_t field_names = obj->get_field_names();
    std::sort(field_names.begin(), field_names.end());
    std::sort(ref_field_names.begin(), ref_field_names.end());
    CPPUNIT_ASSERT(ref_field_names == field_names);

    obj->set_fields(local_fields_backup);
    CPPUNIT_ASSERT_EQUAL(obj->get_fields().size(), std::size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d1), field_obj1);
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d3), nullobj);

    field_names = obj->get_field_names();
    ref_field_names.clear();
    ref_field_names.push_back(field_i_d1);
    CPPUNIT_ASSERT(ref_field_names == field_names);

    obj->set_field(field_i_d1, field_obj2);
    CPPUNIT_ASSERT_EQUAL(obj->get_fields().size(), std::size_t(1));
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d1), field_obj2);
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d2), nullobj);
    CPPUNIT_ASSERT_EQUAL(obj->get_field(field_i_d3), nullobj);

    obj->remove_field(field_i_d1);
    CPPUNIT_ASSERT(obj->get_fields().empty());

    data::object::sptr default_field = obj->set_default_field(field_i_d1, field_obj1);
    CPPUNIT_ASSERT(default_field == field_obj1);

    default_field = obj->set_default_field(field_i_d1, field_obj2);
    CPPUNIT_ASSERT(default_field != field_obj2);
}

//------------------------------------------------------------------------------

void object_test::last_modify_test()
{
    data::object::sptr object = std::make_shared<data::real>();

    CPPUNIT_ASSERT_EQUAL(std::uint64_t(0), object->last_modified());

    {
        data::mt::locked_ptr lock(object);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(1), object->last_modified());
    }

    for(std::uint32_t i = 0 ; i < 199 ; ++i)
    {
        data::mt::locked_ptr lock(object);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(i + 2), object->last_modified());
    }

    CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), object->last_modified());
}

//------------------------------------------------------------------------------

void object_test::equality_test()
{
    auto object1 = std::make_shared<data::real>();
    auto object2 = std::make_shared<data::real>();

    CPPUNIT_ASSERT(*object1 == *object2 && !(*object1 != *object2));

    object1->set_description("1");
    CPPUNIT_ASSERT(*object1 != *object2 && !(*object1 == *object2));
    object2->set_description(object1->get_description());
    CPPUNIT_ASSERT(*object1 == *object2 && !(*object1 != *object2));
}

//------------------------------------------------------------------------------

} // namespace sight::data::ut
