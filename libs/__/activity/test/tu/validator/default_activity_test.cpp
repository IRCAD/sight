/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
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
#include <data/composite.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
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
    core::runtime::add_modules(core::runtime::get_resource_file_path("tu_exec_activity"));
    core::runtime::load_module("tu_validator");

    activity::extension::activity::getDefault()->parse_plugin_infos();
}

//------------------------------------------------------------------------------

void default_activity_test::tearDown()
{
    // Clean up after the test run.
    activity::extension::activity::getDefault()->clear_registry();
}

//------------------------------------------------------------------------------

void default_activity_test::requirementsTest()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::default_activity");
    CPPUNIT_ASSERT(validator);

    activity::validator::activity::sptr activityValidator =
        std::dynamic_pointer_cast<activity::validator::activity>(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::activity::sptr activity = std::make_shared<data::activity>();
    activity->setActivityConfigId("TestValidator0");

    activity::validator::return_t validation;

    data::image_series::sptr imageSeries  = std::make_shared<data::image_series>();
    data::image_series::sptr imageSeries2 = std::make_shared<data::image_series>();
    data::model_series::sptr modelSeries  = std::make_shared<data::model_series>();
    data::model_series::sptr modelSeries2 = std::make_shared<data::model_series>();
    data::model_series::sptr modelSeries3 = std::make_shared<data::model_series>();
    data::composite::sptr modelCompo      = std::make_shared<data::composite>();

    {
        // Empty data should NOT be valid
        validation = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty data should NOT be valid", false, validation.first);
    }
    {
        // data with only an image should NOT be valid
        (*activity)["imageSeries"] = imageSeries;

        validation = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with only an image should NOT be valid", false, validation.first);
    }
    {
        // data with an image and an empty model composite should be valid
        (*activity)["modelSeries"] = modelCompo;

        validation = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and an empty model composite should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and a model in the composite should be valid
        (*modelCompo)["key1"] = modelSeries;

        validation = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and a model in the composite should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and two models in the composite should be valid
        (*modelCompo)["key2"] = modelSeries2;

        validation = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and two models in the composite should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and 3 models in the composite should NOT be valid
        (*modelCompo)["key3"] = modelSeries2;

        validation = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and 3 models in the composite should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with an image and an image in the model composite should NOT be valid
        modelCompo->erase("key3");
        (*modelCompo)["key2"] = imageSeries;
        validation            = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and an image in the model composite should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a model in the composite should NOT be valid
        modelCompo->erase("key2");
        (*activity)["imageSeries"] = nullptr;
        validation                 = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a model in the composite should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a wrong key in the model composite should NOT be valid
        (*modelCompo)["myKey"]     = modelSeries2;
        (*activity)["imageSeries"] = imageSeries;
        validation                 = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a wrong key in the model composite should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void default_activity_test::objectTest()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::default_activity");
    CPPUNIT_ASSERT(validator);

    activity::validator::activity::sptr activityValidator =
        std::dynamic_pointer_cast<activity::validator::activity>(validator);
    CPPUNIT_ASSERT(activityValidator);

    activity::validator::return_t validation;

    data::image::sptr img1 = std::make_shared<data::image>();
    data::image::sptr img2 = std::make_shared<data::image>();
    data::image::sptr img3 = std::make_shared<data::image>();
    utest_data::generator::image::generateRandomImage(img1, core::type::UINT8);
    utest_data::generator::image::generateImage(
        img2,
        img1->size(),
        img1->getSpacing(),
        img1->getOrigin(),
        img1->getType(),
        data::image::PixelFormat::GRAY_SCALE
    );
    utest_data::generator::image::generateRandomImage(img3, core::type::UINT8);

    data::vector::sptr vector = std::make_shared<data::vector>();
    vector->push_back(img1);
    vector->push_back(img2);

    data::vector::sptr vector2 = std::make_shared<data::vector>();
    vector2->push_back(img1);
    vector2->push_back(img3);

    {
        // An empty validator implementation should be valid
        validation = activityValidator->checkObject(img1, "");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("An empty validator implementation should be valid", true, validation.first);
    }
    {
        // A non-existent validator implementation should NOT be valid
        validation = activityValidator->checkObject(img1, "::non-existent::validator");
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "A non-existent validator implementation should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // An existing validator implementation with valid data should be valid
        validation = activityValidator->checkObject(vector, "sight::activity::validator::image_properties");
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "An existing validator implementation with valid data should be valid",
            true,
            validation.first
        );
    }
    {
        // An existing validator implementation with invalid data should NOT be valid
        validation = activityValidator->checkObject(vector2, "sight::activity::validator::image_properties");
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "An existing validator implementation with invalid data should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void default_activity_test::validatorTest()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::default_activity");
    CPPUNIT_ASSERT(validator);

    activity::validator::activity::sptr activityValidator =
        std::dynamic_pointer_cast<activity::validator::activity>(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::activity::sptr activity = std::make_shared<data::activity>();
    activity->setActivityConfigId("TestValidator0");

    activity::validator::return_t validation;

    data::image_series::sptr imageSeries  = std::make_shared<data::image_series>();
    data::image_series::sptr imageSeries2 = std::make_shared<data::image_series>();
    data::model_series::sptr modelSeries  = std::make_shared<data::model_series>();
    data::model_series::sptr modelSeries2 = std::make_shared<data::model_series>();
    data::model_series::sptr modelSeries3 = std::make_shared<data::model_series>();
    data::composite::sptr modelCompo      = std::make_shared<data::composite>();

    utest_data::generator::image::generateRandomImage(imageSeries, core::type::UINT8);

    {
        // Empty data should NOT be valid
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty data should NOT be valid", false, validation.first);
    }
    {
        // data with only an image should NOT be valid
        (*activity)["imageSeries"] = imageSeries;

        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with only an image should NOT be valid", false, validation.first);
    }
    {
        // data with an image and an empty model composite should be valid
        (*activity)["modelSeries"] = modelCompo;

        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and an empty model composite should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and a model in the composite should be valid
        (*modelCompo)["key1"] = modelSeries;

        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and a model in the composite should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and two models in the composite should be valid
        (*modelCompo)["key2"] = modelSeries2;

        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and two models in the composite should be valid",
            true,
            validation.first
        );
    }
    {
        // data with an image and 3 models in the composite should NOT be valid
        (*modelCompo)["key3"] = modelSeries2;

        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and 3 models in the composite should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with an image and an image in the model composite should NOT be valid
        modelCompo->erase("key3");
        (*modelCompo)["key2"] = imageSeries;
        validation            = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with an image and an image in the model composite should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a model in the composite should NOT be valid
        modelCompo->erase("key2");
        (*activity)["imageSeries"] = nullptr;
        validation                 = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a model in the composite should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a wrong key in the model composite should NOT be valid
        (*modelCompo)["myKey"]     = modelSeries2;
        (*activity)["imageSeries"] = imageSeries;
        validation                 = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a wrong key in the model composite should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void default_activity_test::bigActivityTest()
{
    activity::validator::base::sptr validator =
        activity::validator::factory::make("sight::activity::validator::default_activity");
    CPPUNIT_ASSERT(validator);

    activity::validator::activity::sptr activityValidator =
        std::dynamic_pointer_cast<activity::validator::activity>(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::activity::sptr activity = std::make_shared<data::activity>();
    activity->setActivityConfigId("TestValidator1");

    activity::validator::return_t validation;

    data::image_series::sptr imageSeries1 = std::make_shared<data::image_series>();
    data::image_series::sptr imageSeries2 = std::make_shared<data::image_series>();
    data::image_series::sptr imageSeries3 = std::make_shared<data::image_series>();
    data::model_series::sptr modelSeries1 = std::make_shared<data::model_series>();
    data::model_series::sptr modelSeries2 = std::make_shared<data::model_series>();
    data::model_series::sptr modelSeries3 = std::make_shared<data::model_series>();

    data::matrix4::sptr matrix = std::make_shared<data::matrix4>();

    data::real::sptr floatObj   = std::make_shared<data::real>();
    data::integer::sptr intObj  = std::make_shared<data::integer>();
    data::boolean::sptr boolObj = std::make_shared<data::boolean>();

    data::composite::sptr compoStr = std::make_shared<data::composite>();

    (*compoStr)["Item1"] = std::make_shared<data::string>("myItem1");
    (*compoStr)["Item2"] = std::make_shared<data::string>("myItem3");
    (*compoStr)["Item3"] = std::make_shared<data::string>("myItem3");

    data::vector::sptr modelVector = std::make_shared<data::vector>();
    data::vector::sptr imageVector = std::make_shared<data::vector>();

    utest_data::generator::image::generateRandomImage(imageSeries1, core::type::UINT8);
    utest_data::generator::image::generateRandomImage(imageSeries2, core::type::UINT8);
    utest_data::generator::image::generateImage(
        imageSeries3,
        imageSeries2->size(),
        imageSeries2->getSpacing(),
        imageSeries2->getOrigin(),
        imageSeries2->getType(),
        data::image::PixelFormat::GRAY_SCALE
    );

    modelVector->push_back(modelSeries2);
    modelVector->push_back(modelSeries3);
    imageVector->push_back(imageSeries2);
    imageVector->push_back(imageSeries3);

    (*activity)["imageSeries"]          = imageSeries1;
    (*activity)["modelSeries"]          = modelSeries1;
    (*activity)["vectorImage"]          = imageVector;
    (*activity)["vectorModel"]          = modelVector;
    (*activity)["transformationMatrix"] = matrix;
    (*activity)["float"]                = floatObj;
    (*activity)["int"]                  = intObj;
    (*activity)["boolean"]              = boolObj;
    (*activity)["String"]               = compoStr;

    {
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with correct data should be valid", true, validation.first);
    }
    {
        activity->erase("int");
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Activity without an autogenerated data should NOT be valid",
            false,
            validation.first
        );
        (*activity)["int"] = intObj;
    }
    {
        activity->erase("transformationMatrix");
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Activity without an autocreated data should NOT be valid",
            false,
            validation.first
        );
        (*activity)["transformationMatrix"] = matrix;
    }
    {
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with correct data should be valid", true, validation.first);
    }
    {
        const data::image::Spacing spacing = {45., 45., 45.};
        imageSeries3->setSpacing(spacing);
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Activity with different image properties data should NOT be valid",
            false,
            validation.first
        );
        imageSeries3->setSpacing(imageSeries2->getSpacing());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::activity::ut
