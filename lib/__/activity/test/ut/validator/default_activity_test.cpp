/************************************************************************
 *
 * Copyright (C) 2016-2026 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include <activity/extension/activity.hpp>
#include <activity/validator/activity.hpp>
#include <activity/validator/base.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/activity.hpp>
#include <data/boolean.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/map.hpp>
#include <data/matrix4.hpp>
#include <data/model_series.hpp>
#include <data/real.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <utest_data/generator/image.hpp>

#include <doctest/doctest.h>

namespace
{

struct fixture
{
    fixture()
    {
        // Set up context before running a test.
        sight::core::runtime::add_modules(sight::core::runtime::get_resource_file_path("activity_ut"));
        sight::core::runtime::load_module("ut_validator");

        sight::activity::extension::activity::get_default()->parse_plugin_infos();
    }

    ~fixture()
    {
        // Clean up after the test run.
        sight::activity::extension::activity::get_default()->clear_registry();
    }
};

} // namespace

//------------------------------------------------------------------------------

TEST_SUITE("sight::activity::default_activity")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "requirements")
    {
        auto validator = sight::activity::validator::factory::make("sight::activity::validator::default_activity");
        CHECK(validator);

        auto activity_validator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);
        CHECK(activity_validator);

        sight::data::activity::sptr activity = std::make_shared<sight::data::activity>();
        activity->set_activity_config_id("TestValidator0");

        sight::activity::validator::return_t validation;

        sight::data::image_series::sptr image_series  = std::make_shared<sight::data::image_series>();
        sight::data::image_series::sptr image_series2 = std::make_shared<sight::data::image_series>();
        sight::data::model_series::sptr model_series  = std::make_shared<sight::data::model_series>();
        sight::data::model_series::sptr model_series2 = std::make_shared<sight::data::model_series>();
        sight::data::model_series::sptr model_series3 = std::make_shared<sight::data::model_series>();
        sight::data::map::sptr model_compo            = std::make_shared<sight::data::map>();

        {
            // Empty data should NOT be valid
            validation = sight::activity::validator::activity::check_requirements(activity);
            CHECK_MESSAGE(validation.first == false, "Empty data should NOT be valid");
        }
        {
            // data with only an image should NOT be valid
            (*activity)["imageSeries"] = image_series;

            validation = sight::activity::validator::activity::check_requirements(activity);
            CHECK_MESSAGE(validation.first == false, "data with only an image should NOT be valid");
        }
        {
            // data with an image and an empty model map should be valid
            (*activity)["modelSeries"] = model_compo;

            validation = sight::activity::validator::activity::check_requirements(activity);
            CHECK_MESSAGE(
                validation.first == true,
                "data with an image and an empty model map should be valid"
            );
        }
        {
            // data with an image and a model in the map should be valid
            (*model_compo)["key1"] = model_series;

            validation = sight::activity::validator::activity::check_requirements(activity);
            CHECK_MESSAGE(
                validation.first == true,
                "data with an image and a model in the map should be valid"
            );
        }
        {
            // data with an image and two models in the map should be valid
            (*model_compo)["key2"] = model_series2;

            validation = sight::activity::validator::activity::check_requirements(activity);
            CHECK_MESSAGE(
                validation.first == true,
                "data with an image and two models in the map should be valid"
            );
        }
        {
            // data with an image and 3 models in the map should NOT be valid
            (*model_compo)["key3"] = model_series2;

            validation = sight::activity::validator::activity::check_requirements(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "data with an image and 3 models in the map should NOT be valid"
            );
        }
        {
            // data with an image and an image in the model map should NOT be valid
            model_compo->erase("key3");
            (*model_compo)["key2"] = image_series;
            validation             = sight::activity::validator::activity::check_requirements(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "data with an image and an image in the model map should NOT be valid"
            );
        }
        {
            // data with a null image and a model in the map should NOT be valid
            model_compo->erase("key2");
            (*activity)["imageSeries"] = nullptr;
            validation                 = sight::activity::validator::activity::check_requirements(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "data with a null image and a model in the map should NOT be valid"
            );
        }
        {
            // data with a null image and a wrong key in the model map should NOT be valid
            (*model_compo)["myKey"]    = model_series2;
            (*activity)["imageSeries"] = image_series;
            validation                 = sight::activity::validator::activity::check_requirements(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "data with a null image and a wrong key in the model map should NOT be valid"
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "object")
    {
        auto validator = sight::activity::validator::factory::make("sight::activity::validator::default_activity");
        CHECK(validator);

        auto activity_validator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);
        CHECK(activity_validator);

        sight::activity::validator::return_t validation;

        sight::data::image::sptr img1 = std::make_shared<sight::data::image>();
        sight::data::image::sptr img2 = std::make_shared<sight::data::image>();
        sight::data::image::sptr img3 = std::make_shared<sight::data::image>();
        sight::utest_data::generator::image::generate_random_image(img1, sight::core::type::UINT8);
        sight::utest_data::generator::image::generate_image(
            img2,
            img1->size(),
            img1->spacing(),
            img1->origin(),
            img1->orientation(),
            img1->type(),
            sight::data::image::pixel_format_t::gray_scale
        );
        sight::utest_data::generator::image::generate_random_image(img3, sight::core::type::UINT8);

        sight::data::vector::sptr vector = std::make_shared<sight::data::vector>();
        vector->push_back(img1);
        vector->push_back(img2);

        sight::data::vector::sptr vector2 = std::make_shared<sight::data::vector>();
        vector2->push_back(img1);
        vector2->push_back(img3);

        {
            // An empty validator implementation should be valid
            validation = sight::activity::validator::activity::check_object(img1, "");
            CHECK_MESSAGE(validation.first == true, "An empty validator implementation should be valid");
        }
        {
            // A non-existent validator implementation should NOT be valid
            validation = sight::activity::validator::activity::check_object(img1, "::non-existent::validator");
            CHECK_MESSAGE(
                validation.first == false,
                "A non-existent validator implementation should NOT be valid"
            );
        }
        {
            // An existing validator implementation with valid data should be valid
            validation = sight::activity::validator::activity::check_object(
                vector,
                "sight::activity::validator::image_properties"
            );
            CHECK_MESSAGE(
                validation.first == true,
                "An existing validator implementation with valid data should be valid"
            );
        }
        {
            // An existing validator implementation with invalid data should NOT be valid
            validation = sight::activity::validator::activity::check_object(
                vector2,
                "sight::activity::validator::image_properties"
            );
            CHECK_MESSAGE(
                validation.first == false,
                "An existing validator implementation with invalid data should NOT be valid"
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "validator")
    {
        auto validator = sight::activity::validator::factory::make("sight::activity::validator::default_activity");
        CHECK(validator);

        auto activity_validator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);
        CHECK(activity_validator);

        sight::data::activity::sptr activity = std::make_shared<sight::data::activity>();
        activity->set_activity_config_id("TestValidator0");

        sight::activity::validator::return_t validation;

        sight::data::image_series::sptr image_series  = std::make_shared<sight::data::image_series>();
        sight::data::image_series::sptr image_series2 = std::make_shared<sight::data::image_series>();
        sight::data::model_series::sptr model_series  = std::make_shared<sight::data::model_series>();
        sight::data::model_series::sptr model_series2 = std::make_shared<sight::data::model_series>();
        sight::data::model_series::sptr model_series3 = std::make_shared<sight::data::model_series>();
        sight::data::map::sptr model_compo            = std::make_shared<sight::data::map>();

        sight::utest_data::generator::image::generate_random_image(image_series, sight::core::type::UINT8);

        {
            // Empty data should NOT be valid
            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(validation.first == false, "Empty data should NOT be valid");
        }
        {
            // data with only an image should NOT be valid
            (*activity)["imageSeries"] = image_series;

            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(validation.first == false, "data with only an image should NOT be valid");
        }
        {
            // data with an image and an empty model map should be valid
            (*activity)["modelSeries"] = model_compo;

            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == true,
                "data with an image and an empty model map should be valid"
            );
        }
        {
            // data with an image and a model in the map should be valid
            (*model_compo)["key1"] = model_series;

            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == true,
                "data with an image and a model in the map should be valid"
            );
        }
        {
            // data with an image and two models in the map should be valid
            (*model_compo)["key2"] = model_series2;

            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == true,
                "data with an image and two models in the map should be valid"
            );
        }
        {
            // data with an image and 3 models in the map should NOT be valid
            (*model_compo)["key3"] = model_series2;

            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "data with an image and 3 models in the map should NOT be valid"
            );
        }
        {
            // data with an image and an image in the model map should NOT be valid
            model_compo->erase("key3");
            (*model_compo)["key2"] = image_series;
            validation             = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "data with an image and an image in the model map should NOT be valid"
            );
        }
        {
            // data with a null image and a model in the map should NOT be valid
            model_compo->erase("key2");
            (*activity)["imageSeries"] = nullptr;
            validation                 = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "data with a null image and a model in the map should NOT be valid"
            );
        }
        {
            // data with a null image and a wrong key in the model map should NOT be valid
            (*model_compo)["myKey"]    = model_series2;
            (*activity)["imageSeries"] = image_series;
            validation                 = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "data with a null image and a wrong key in the model map should NOT be valid"
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "big_activity")
    {
        auto validator = sight::activity::validator::factory::make("sight::activity::validator::default_activity");
        CHECK(validator);

        auto activity_validator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);
        CHECK(activity_validator);

        sight::data::activity::sptr activity = std::make_shared<sight::data::activity>();
        activity->set_activity_config_id("TestValidator1");

        sight::activity::validator::return_t validation;

        sight::data::image_series::sptr image_series1 = std::make_shared<sight::data::image_series>();
        sight::data::image_series::sptr image_series2 = std::make_shared<sight::data::image_series>();
        sight::data::image_series::sptr image_series3 = std::make_shared<sight::data::image_series>();
        sight::data::model_series::sptr model_series1 = std::make_shared<sight::data::model_series>();
        sight::data::model_series::sptr model_series2 = std::make_shared<sight::data::model_series>();
        sight::data::model_series::sptr model_series3 = std::make_shared<sight::data::model_series>();

        sight::data::matrix4::sptr matrix = std::make_shared<sight::data::matrix4>();

        sight::data::real::sptr float_obj   = std::make_shared<sight::data::real>();
        sight::data::integer::sptr int_obj  = std::make_shared<sight::data::integer>();
        sight::data::boolean::sptr bool_obj = std::make_shared<sight::data::boolean>();

        sight::data::map::sptr compo_str = std::make_shared<sight::data::map>();

        (*compo_str)["Item1"] = std::make_shared<sight::data::string>("myItem1");
        (*compo_str)["Item2"] = std::make_shared<sight::data::string>("myItem3");
        (*compo_str)["Item3"] = std::make_shared<sight::data::string>("myItem3");

        sight::data::vector::sptr model_vector = std::make_shared<sight::data::vector>();
        sight::data::vector::sptr image_vector = std::make_shared<sight::data::vector>();

        sight::utest_data::generator::image::generate_random_image(image_series1, sight::core::type::UINT8);
        sight::utest_data::generator::image::generate_random_image(image_series2, sight::core::type::UINT8);
        sight::utest_data::generator::image::generate_image(
            image_series3,
            image_series2->size(),
            image_series2->spacing(),
            image_series2->origin(),
            image_series2->orientation(),
            image_series2->type(),
            sight::data::image::pixel_format_t::gray_scale
        );

        model_vector->push_back(model_series2);
        model_vector->push_back(model_series3);
        image_vector->push_back(image_series2);
        image_vector->push_back(image_series3);

        (*activity)["imageSeries"]          = image_series1;
        (*activity)["modelSeries"]          = model_series1;
        (*activity)["vectorImage"]          = image_vector;
        (*activity)["vectorModel"]          = model_vector;
        (*activity)["transformationMatrix"] = matrix;
        (*activity)["float"]                = float_obj;
        (*activity)["int"]                  = int_obj;
        (*activity)["boolean"]              = bool_obj;
        (*activity)["String"]               = compo_str;

        {
            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(validation.first == true, "Activity with correct data should be valid");
        }
        {
            activity->erase("int");
            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "Activity without an autogenerated data should NOT be valid"
            );
            (*activity)["int"] = int_obj;
        }
        {
            activity->erase("transformationMatrix");
            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "Activity without an autocreated data should NOT be valid"
            );
            (*activity)["transformationMatrix"] = matrix;
        }
        {
            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(validation.first == true, "Activity with correct data should be valid");
        }
        {
            const sight::data::image::spacing_t spacing = {45., 45., 45.};
            image_series3->set_spacing(spacing);
            validation = activity_validator->validate(activity);
            CHECK_MESSAGE(
                validation.first == false,
                "Activity with different image properties data should NOT be valid"
            );
            image_series3->set_spacing(image_series2->spacing());
        }
    }
}
