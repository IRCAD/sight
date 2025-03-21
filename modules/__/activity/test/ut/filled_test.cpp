/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "filled_test.hpp"

#include <activity/validator/object.hpp>

#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/point_list.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::activity::validator::ut::filled_test);

namespace sight::module::activity::validator::ut
{

namespace factory = sight::activity::validator::factory;

//------------------------------------------------------------------------------

void filled_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void filled_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void filled_test::image()
{
    auto validator = factory::make("sight::module::activity::validator::filled");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::activity::validator::return_t validation;
    data::image::sptr image = std::make_shared<data::image>();

    {
        validation = obj_validator->validate(image);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);
    }
    {
        image->resize({4, 4, 1}, core::type::UINT8, data::image::gray_scale);
        validation = obj_validator->validate(image);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);
    }
}

//------------------------------------------------------------------------------

void filled_test::model_series()
{
    auto validator = factory::make("sight::module::activity::validator::filled");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::activity::validator::return_t validation;
    data::model_series::sptr model_series = std::make_shared<data::model_series>();

    {
        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);
    }
    {
        data::model_series::reconstruction_vector_t recs = model_series->get_reconstruction_db();
        recs.push_back(std::make_shared<sight::data::reconstruction>());
        model_series->set_reconstruction_db(recs);

        validation = obj_validator->validate(model_series);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);
    }
}

//------------------------------------------------------------------------------

void filled_test::point_list()
{
    auto validator = factory::make("sight::module::activity::validator::filled");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::activity::validator::return_t validation;
    data::point_list::sptr point_list = std::make_shared<data::point_list>();

    {
        validation = obj_validator->validate(point_list);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);
    }
    {
        point_list->push_back(std::make_shared<sight::data::point>());
        validation = obj_validator->validate(point_list);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::activity::validator::ut
