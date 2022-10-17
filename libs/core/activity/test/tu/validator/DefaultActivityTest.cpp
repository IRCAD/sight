/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "DefaultActivityTest.hpp"

#include <activity/extension/Activity.hpp>
#include <activity/IActivityValidator.hpp>
#include <activity/IValidator.hpp>

#include <core/runtime/Extension.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>

#include <data/Activity.hpp>
#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/Matrix4.hpp>
#include <data/ModelSeries.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <utestData/generator/Image.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::ut::DefaultActivityTest);

namespace sight::activity::ut
{

//------------------------------------------------------------------------------

void DefaultActivityTest::setUp()
{
    // Set up context before running a test.
    core::runtime::addModules(core::runtime::getResourceFilePath("tu_exec_activity"));
    core::runtime::loadModule("tu_validator");

    activity::extension::Activity::getDefault()->parseBundleInformation();
}

//------------------------------------------------------------------------------

void DefaultActivityTest::tearDown()
{
    // Clean up after the test run.
    activity::extension::Activity::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

void DefaultActivityTest::requirementsTest()
{
    activity::IValidator::sptr validator =
        activity::validator::factory::New("sight::activity::validator::DefaultActivity");
    CPPUNIT_ASSERT(validator);

    activity::IActivityValidator::sptr activityValidator =
        activity::IActivityValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::Activity::sptr activity = data::Activity::New();
    activity->setActivityConfigId("TestValidator0");

    activity::IValidator::ValidationType validation;

    data::Composite::sptr data = data::Composite::New();
    activity->setData(data);

    data::ImageSeries::sptr imageSeries  = data::ImageSeries::New();
    data::ImageSeries::sptr imageSeries2 = data::ImageSeries::New();
    data::ModelSeries::sptr modelSeries  = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries2 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries3 = data::ModelSeries::New();
    data::Composite::sptr modelCompo     = data::Composite::New();

    {
        // Empty data should NOT be valid
        validation = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty data should NOT be valid", false, validation.first);
    }
    {
        // data with only an image should NOT be valid
        (*data)["imageSeries"] = imageSeries;

        validation = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with only an image should NOT be valid", false, validation.first);
    }
    {
        // data with an image and an empty model composite should be valid
        (*data)["modelSeries"] = modelCompo;

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
        (*data)["imageSeries"] = nullptr;
        validation             = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a model in the composite should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a wrong key in the model composite should NOT be valid
        (*modelCompo)["myKey"] = modelSeries2;
        (*data)["imageSeries"] = imageSeries;
        validation             = activityValidator->checkRequirements(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a wrong key in the model composite should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void DefaultActivityTest::objectTest()
{
    activity::IValidator::sptr validator =
        activity::validator::factory::New("sight::activity::validator::DefaultActivity");
    CPPUNIT_ASSERT(validator);

    activity::IActivityValidator::sptr activityValidator =
        activity::IActivityValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(activityValidator);

    activity::IValidator::ValidationType validation;

    data::Image::sptr img1 = data::Image::New();
    data::Image::sptr img2 = data::Image::New();
    data::Image::sptr img3 = data::Image::New();
    utestData::generator::Image::generateRandomImage(img1, core::Type::UINT8);
    utestData::generator::Image::generateImage(
        img2,
        img1->getSize(),
        img1->getSpacing(),
        img1->getOrigin(),
        img1->getType(),
        data::Image::PixelFormat::GRAY_SCALE
    );
    utestData::generator::Image::generateRandomImage(img3, core::Type::UINT8);

    data::Vector::sptr vector = data::Vector::New();
    vector->push_back(img1);
    vector->push_back(img2);

    data::Vector::sptr vector2 = data::Vector::New();
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
        validation = activityValidator->checkObject(vector, "sight::activity::validator::ImageProperties");
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "An existing validator implementation with valid data should be valid",
            true,
            validation.first
        );
    }
    {
        // An existing validator implementation with invalid data should NOT be valid
        validation = activityValidator->checkObject(vector2, "sight::activity::validator::ImageProperties");
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "An existing validator implementation with invalid data should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void DefaultActivityTest::validatorTest()
{
    activity::IValidator::sptr validator =
        activity::validator::factory::New("sight::activity::validator::DefaultActivity");
    CPPUNIT_ASSERT(validator);

    activity::IActivityValidator::sptr activityValidator =
        activity::IActivityValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::Activity::sptr activity = data::Activity::New();
    activity->setActivityConfigId("TestValidator0");

    activity::IValidator::ValidationType validation;

    data::Composite::sptr data = data::Composite::New();
    activity->setData(data);

    data::ImageSeries::sptr imageSeries  = data::ImageSeries::New();
    data::ImageSeries::sptr imageSeries2 = data::ImageSeries::New();
    data::ModelSeries::sptr modelSeries  = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries2 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries3 = data::ModelSeries::New();
    data::Composite::sptr modelCompo     = data::Composite::New();

    utestData::generator::Image::generateRandomImage(imageSeries, core::Type::UINT8);

    {
        // Empty data should NOT be valid
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty data should NOT be valid", false, validation.first);
    }
    {
        // data with only an image should NOT be valid
        (*data)["imageSeries"] = imageSeries;

        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with only an image should NOT be valid", false, validation.first);
    }
    {
        // data with an image and an empty model composite should be valid
        (*data)["modelSeries"] = modelCompo;

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
        (*data)["imageSeries"] = nullptr;
        validation             = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a model in the composite should NOT be valid",
            false,
            validation.first
        );
    }
    {
        // data with a null image and a wrong key in the model composite should NOT be valid
        (*modelCompo)["myKey"] = modelSeries2;
        (*data)["imageSeries"] = imageSeries;
        validation             = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a wrong key in the model composite should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

void DefaultActivityTest::bigActivityTest()
{
    activity::IValidator::sptr validator =
        activity::validator::factory::New("sight::activity::validator::DefaultActivity");
    CPPUNIT_ASSERT(validator);

    activity::IActivityValidator::sptr activityValidator =
        activity::IActivityValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::Activity::sptr activity = data::Activity::New();
    activity->setActivityConfigId("TestValidator1");

    activity::IValidator::ValidationType validation;

    data::Composite::sptr data = data::Composite::New();
    activity->setData(data);

    data::ImageSeries::sptr imageSeries1 = data::ImageSeries::New();
    data::ImageSeries::sptr imageSeries2 = data::ImageSeries::New();
    data::ImageSeries::sptr imageSeries3 = data::ImageSeries::New();
    data::ModelSeries::sptr modelSeries1 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries2 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries3 = data::ModelSeries::New();

    data::Matrix4::sptr matrix = data::Matrix4::New();

    data::Float::sptr floatObj  = data::Float::New();
    data::Integer::sptr intObj  = data::Integer::New();
    data::Boolean::sptr boolObj = data::Boolean::New();

    data::Composite::sptr compoStr = data::Composite::New();

    (*compoStr)["Item1"] = data::String::New("myItem1");
    (*compoStr)["Item2"] = data::String::New("myItem3");
    (*compoStr)["Item3"] = data::String::New("myItem3");

    data::Vector::sptr modelVector = data::Vector::New();
    data::Vector::sptr imageVector = data::Vector::New();

    utestData::generator::Image::generateRandomImage(imageSeries1, core::Type::UINT8);
    utestData::generator::Image::generateRandomImage(imageSeries2, core::Type::UINT8);
    utestData::generator::Image::generateImage(
        imageSeries3,
        imageSeries2->getSize(),
        imageSeries2->getSpacing(),
        imageSeries2->getOrigin(),
        imageSeries2->getType(),
        data::Image::PixelFormat::GRAY_SCALE
    );

    modelVector->push_back(modelSeries2);
    modelVector->push_back(modelSeries3);
    imageVector->push_back(imageSeries2);
    imageVector->push_back(imageSeries3);

    (*data)["imageSeries"]          = imageSeries1;
    (*data)["modelSeries"]          = modelSeries1;
    (*data)["vectorImage"]          = imageVector;
    (*data)["vectorModel"]          = modelVector;
    (*data)["transformationMatrix"] = matrix;
    (*data)["float"]                = floatObj;
    (*data)["int"]                  = intObj;
    (*data)["boolean"]              = boolObj;
    (*data)["String"]               = compoStr;

    {
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with correct data should be valid", true, validation.first);
    }
    {
        data->erase("int");
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Activity without an autogenerated data should NOT be valid",
            false,
            validation.first
        );
        (*data)["int"] = intObj;
    }
    {
        data->erase("transformationMatrix");
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Activity without an autocreated data should NOT be valid",
            false,
            validation.first
        );
        (*data)["transformationMatrix"] = matrix;
    }
    {
        validation = activityValidator->validate(activity);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with correct data should be valid", true, validation.first);
    }
    {
        const data::Image::Spacing spacing = {45., 45., 45.};
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
