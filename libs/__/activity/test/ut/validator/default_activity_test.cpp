/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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

#include "default_activity_test.hpp"

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

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::ut::default_activity_test);

namespace sight::activity::ut
{

//------------------------------------------------------------------------------

void default_activity_test::setUp()
{
    // Set up context before running a test.
    core::runtime::add_modules(core::runtime::get_resource_file_path("activity_ut"));
    core::runtime::load_module("ut_validator");

    activity::extension::activity::get_default()->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void default_activity_test::tearDown()
{
    // Clean up after the test run.
    activity::extension::activity::get_default()->clear_registry();
}

//------------------------------------------------------------------------------

void default_activity_test::requirements_test()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::default_activity");
    CPPUNIT_ASSERT(validator);

    activity::validator::activity::sptr activity_validator =
        std::dynamic_pointer_cast<activity::validator::activity>(validator);
    CPPUNIT_ASSERT(activity_validator);

    data::activity::sptr activity = std::make_shared<data::activity>();
    activity->set_activity_config_id("TestValidator0");

    activity::validator::return_t validation;

    data::image_series::sptr image_series  = std::make_shared<data::image_series>();
    data::image_series::sptr image_series2 = std::make_shared<data::image_series>();
    data::model_series::sptr model_series  = std::make_shared<data::model_series>();
    data::model_series::sptr model_series2 = std::make_shared<data::model_series>();
    data::model_series::sptr model_series3 = std::make_shared<data::model_series>();
    data::map::sptr model_compo            = std::make_shared<data::map>();

    {
        // Empty data should NOT be valid
        validation = activity_validator->check_requirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty data should NOT be valid", false, validation.first);
    }
    {
        // data with only an image should NOT be valid
        (*activity)["imageSeries"] = image_series;

        validation = activity_validator->check_requirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with only an image should NOT be valid", false, validation.first);
    }
    {
        // data with an image and an empty model map should be valid
        (*activity)["modelSeries"] = model_compo;

        validation = activity_validator->check_requirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and an empty model map should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and a model in the map should be valid
        (*model_compo)["key1"] = model_series;

        validation = activity_validator->check_requirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and a model in the map should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and two models in the map should be valid
        (*model_compo)["key2"] = model_series2;

        validation = activity_validator->check_requirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and two models in the map should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and 3 models in the map should NOT be valid
        (*model_compo)["key3"] = model_series2;

        validation = activity_validator->check_requirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and 3 models in the map should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with an image and an image in the model map should NOT be valid
        model_compo->erase("key3");
        (*model_compo)["key2"] = image_series;
        validation             = activity_validator->check_requirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and an image in the model map should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a model in the map should NOT be valid
        model_compo->erase("key2");
        (*activity)["imageSeries"] = nullptr;
        validation                 = activity_validator->check_requirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a model in the map should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a wrong key in the model map should NOT be valid
        (*model_compo)["myKey"]    = model_series2;
        (*activity)["imageSeries"] = image_series;
        validation                 = activity_validator->check_requirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a wrong key in the model map should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void default_activity_test::object_test()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::default_activity");
    CPPUNIT_ASSERT(validator);

    activity::validator::activity::sptr activity_validator =
        std::dynamic_pointer_cast<activity::validator::activity>(validator);
    CPPUNIT_ASSERT(activity_validator);

    activity::validator::return_t validation;

    data::image::sptr img1 = std::make_shared<data::image>();
    data::image::sptr img2 = std::make_shared<data::image>();
    data::image::sptr img3 = std::make_shared<data::image>();
    utest_data::generator::image::generate_random_image(img1, core::type::UINT8);
    utest_data::generator::image::generate_image(
        img2,
        img1->size(),
        img1->spacing(),
        img1->origin(),
        img1->orientation(),
        img1->type(),
        data::image::pixel_format_t::gray_scale
    );
    utest_data::generator::image::generate_random_image(img3, core::type::UINT8);

    data::vector::sptr vector = std::make_shared<data::vector>();
    vector->push_back(img1);
    vector->push_back(img2);

    data::vector::sptr vector2 = std::make_shared<data::vector>();
    vector2->push_back(img1);
    vector2->push_back(img3);

    {
        // An empty validator implementation should be valid
        validation = activity_validator->check_object(img1, "");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("An empty validator implementation should be valid", true, validation.first);
    }
    {
        // A non-existent validator implementation should NOT be valid
        validation = activity_validator->check_object(img1, "::non-existent::validator");
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "A non-existent validator implementation should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // An existing validator implementation with valid data should be valid
        validation = activity_validator->check_object(vector, "sight::activity::validator::image_properties");
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "An existing validator implementation with valid data should be valid",
            true,
            validation.first
        );
    }
    {
        // An existing validator implementation with invalid data should NOT be valid
        validation = activity_validator->check_object(vector2, "sight::activity::validator::image_properties");
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "An existing validator implementation with invalid data should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void default_activity_test::validator_test()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::default_activity");
    CPPUNIT_ASSERT(validator);

    activity::validator::activity::sptr activity_validator =
        std::dynamic_pointer_cast<activity::validator::activity>(validator);
    CPPUNIT_ASSERT(activity_validator);

    data::activity::sptr activity = std::make_shared<data::activity>();
    activity->set_activity_config_id("TestValidator0");

    activity::validator::return_t validation;

    data::image_series::sptr image_series  = std::make_shared<data::image_series>();
    data::image_series::sptr image_series2 = std::make_shared<data::image_series>();
    data::model_series::sptr model_series  = std::make_shared<data::model_series>();
    data::model_series::sptr model_series2 = std::make_shared<data::model_series>();
    data::model_series::sptr model_series3 = std::make_shared<data::model_series>();
    data::map::sptr model_compo            = std::make_shared<data::map>();

    utest_data::generator::image::generate_random_image(image_series, core::type::UINT8);

    {
        // Empty data should NOT be valid
        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty data should NOT be valid", false, validation.first);
    }
    {
        // data with only an image should NOT be valid
        (*activity)["imageSeries"] = image_series;

        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with only an image should NOT be valid", false, validation.first);
    }
    {
        // data with an image and an empty model map should be valid
        (*activity)["modelSeries"] = model_compo;

        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and an empty model map should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and a model in the map should be valid
        (*model_compo)["key1"] = model_series;

        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and a model in the map should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and two models in the map should be valid
        (*model_compo)["key2"] = model_series2;

        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and two models in the map should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and 3 models in the map should NOT be valid
        (*model_compo)["key3"] = model_series2;

        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and 3 models in the map should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with an image and an image in the model map should NOT be valid
        model_compo->erase("key3");
        (*model_compo)["key2"] = image_series;
        validation             = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and an image in the model map should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a model in the map should NOT be valid
        model_compo->erase("key2");
        (*activity)["imageSeries"] = nullptr;
        validation                 = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a model in the map should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a wrong key in the model map should NOT be valid
        (*model_compo)["myKey"]    = model_series2;
        (*activity)["imageSeries"] = image_series;
        validation                 = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a wrong key in the model map should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void default_activity_test::big_activity_test()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::default_activity");
    CPPUNIT_ASSERT(validator);

    activity::validator::activity::sptr activity_validator =
        std::dynamic_pointer_cast<activity::validator::activity>(validator);
    CPPUNIT_ASSERT(activity_validator);

    data::activity::sptr activity = std::make_shared<data::activity>();
    activity->set_activity_config_id("TestValidator1");

    activity::validator::return_t validation;

    data::image_series::sptr image_series1 = std::make_shared<data::image_series>();
    data::image_series::sptr image_series2 = std::make_shared<data::image_series>();
    data::image_series::sptr image_series3 = std::make_shared<data::image_series>();
    data::model_series::sptr model_series1 = std::make_shared<data::model_series>();
    data::model_series::sptr model_series2 = std::make_shared<data::model_series>();
    data::model_series::sptr model_series3 = std::make_shared<data::model_series>();

    data::matrix4::sptr matrix = std::make_shared<data::matrix4>();

    data::real::sptr float_obj   = std::make_shared<data::real>();
    data::integer::sptr int_obj  = std::make_shared<data::integer>();
    data::boolean::sptr bool_obj = std::make_shared<data::boolean>();

    data::map::sptr compo_str = std::make_shared<data::map>();

    (*compo_str)["Item1"] = std::make_shared<data::string>("myItem1");
    (*compo_str)["Item2"] = std::make_shared<data::string>("myItem3");
    (*compo_str)["Item3"] = std::make_shared<data::string>("myItem3");

    data::vector::sptr model_vector = std::make_shared<data::vector>();
    data::vector::sptr image_vector = std::make_shared<data::vector>();

    utest_data::generator::image::generate_random_image(image_series1, core::type::UINT8);
    utest_data::generator::image::generate_random_image(image_series2, core::type::UINT8);
    utest_data::generator::image::generate_image(
        image_series3,
        image_series2->size(),
        image_series2->spacing(),
        image_series2->origin(),
        image_series2->orientation(),
        image_series2->type(),
        data::image::pixel_format_t::gray_scale
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
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with correct data should be valid", true, validation.first);
    }
    {
        activity->erase("int");
        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Activity without an autogenerated data should NOT be valid",
            false,
            validation.first
        );
        (*activity)["int"] = int_obj;
    }
    {
        activity->erase("transformationMatrix");
        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Activity without an autocreated data should NOT be valid",
            false,
            validation.first
        );
        (*activity)["transformationMatrix"] = matrix;
    }
    {
        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with correct data should be valid", true, validation.first);
    }
    {
        const data::image::spacing_t spacing = {45., 45., 45.};
        image_series3->set_spacing(spacing);
        validation = activity_validator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Activity with different image properties data should NOT be valid",
            false,
            validation.first
        );
        image_series3->set_spacing(image_series2->spacing());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::activity::ut
