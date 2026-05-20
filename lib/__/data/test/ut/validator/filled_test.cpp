/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <data/image_series.hpp>
#include <data/model_series.hpp>
#include <data/point_list.hpp>
#include <data/string.hpp>
#include <data/validator/base.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::validator")
{
    namespace factory = sight::data::validator::factory;

//------------------------------------------------------------------------------

    TEST_CASE("image")
    {
        auto validator = factory::make("sight::data::validator::filled");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;
        sight::data::image::sptr image = std::make_shared<sight::data::image>();

        {
            validation = obj_validator->validate(image);
            CHECK_EQ(false, validation.first);
        }
        {
            image->resize({4, 4, 1}, sight::core::type::UINT8, sight::data::image::gray_scale);
            validation = obj_validator->validate(image);
            CHECK_EQ(true, validation.first);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("model_series")
    {
        auto validator = factory::make("sight::data::validator::filled");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;
        sight::data::model_series::sptr model_series = std::make_shared<sight::data::model_series>();

        {
            validation = obj_validator->validate(model_series);
            CHECK_EQ(false, validation.first);
        }
        {
            sight::data::model_series::reconstruction_vector_t recs = model_series->get_reconstruction_db();
            recs.push_back(std::make_shared<sight::data::reconstruction>());
            model_series->set_reconstruction_db(recs);

            validation = obj_validator->validate(model_series);
            CHECK_EQ(true, validation.first);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("point_list")
    {
        auto validator = factory::make("sight::data::validator::filled");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;
        sight::data::point_list::sptr point_list = std::make_shared<sight::data::point_list>();

        {
            validation = obj_validator->validate(point_list);
            CHECK_EQ(false, validation.first);
        }
        {
            point_list->push_back(std::make_shared<sight::data::point>());
            validation = obj_validator->validate(point_list);
            CHECK_EQ(true, validation.first);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("string_serializable")
    {
        auto validator = factory::make("sight::data::validator::filled");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;
        sight::data::string_serializable::sptr string_serializable = std::make_shared<sight::data::string>();

        {
            validation = obj_validator->validate(string_serializable);
            CHECK_EQ(false, validation.first);
        }
        {
            string_serializable->from_string("Non-empty string");
            validation = obj_validator->validate(string_serializable);
            CHECK_EQ(true, validation.first);
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::validator")
