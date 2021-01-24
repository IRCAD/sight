/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include "validator/DefaultActivityTest.hpp"

#include <fwActivities/IActivityValidator.hpp>
#include <fwActivities/IValidator.hpp>
#include <fwActivities/registry/Activities.hpp>

#include <core/runtime/Extension.hpp>
#include <core/runtime/Module.hpp>
#include <core/runtime/operations.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Boolean.hpp>
#include <data/Composite.hpp>
#include <data/Float.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Integer.hpp>
#include <data/ModelSeries.hpp>
#include <data/String.hpp>
#include <data/TransformationMatrix3D.hpp>
#include <data/Vector.hpp>

#include <fwTest/generator/Image.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwActivities::ut::DefaultActivityTest );

namespace fwActivities
{
namespace ut
{

//------------------------------------------------------------------------------

void DefaultActivityTest::setUp()
{
    // Set up context before running a test.
    core::runtime::addModules(core::runtime::getResourceFilePath("tu_exec_fwActivities-0.0"));
    core::runtime::loadModule("tu_validator");

    ::fwActivities::registry::Activities::getDefault()->parseBundleInformation();
}

//------------------------------------------------------------------------------

void DefaultActivityTest::tearDown()
{
    // Clean up after the test run.
    ::fwActivities::registry::Activities::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

void DefaultActivityTest::requirementsTest()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::fwActivities::validator::DefaultActivity");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IActivityValidator::sptr activityValidator =
        ::fwActivities::IActivityValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::ActivitySeries::sptr activitySeries = data::ActivitySeries::New();
    activitySeries->setActivityConfigId("TestValidator0");

    ::fwActivities::IValidator::ValidationType validation;

    data::Composite::sptr data = data::Composite::New();
    activitySeries->setData(data);

    data::ImageSeries::sptr imageSeries  = data::ImageSeries::New();
    data::ImageSeries::sptr imageSeries2 = data::ImageSeries::New();
    data::ModelSeries::sptr modelSeries  = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries2 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries3 = data::ModelSeries::New();
    data::Composite::sptr modelCompo     = data::Composite::New();

    {
        // Empty data should NOT be valid
        validation = activityValidator->checkRequirements(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty data should NOT be valid", false, validation.first);
    }
    {
        // data with only an image should NOT be valid
        (*data)["imageSeries"] = imageSeries;

        validation = activityValidator->checkRequirements(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with only an image should NOT be valid", false, validation.first);
    }
    {
        // data with an image and an empty model composite should be valid
        (*data)["modelSeries"] = modelCompo;

        validation = activityValidator->checkRequirements(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and an empty model composite should be valid",
                                     true, validation.first);
    }
    {
        // data with an image and a model in the composite should be valid
        (*modelCompo)["key1"] = modelSeries;

        validation = activityValidator->checkRequirements(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and a model in the composite should be valid",
                                     true, validation.first);
    }
    {
        // data with an image and two models in the composite should be valid
        (*modelCompo)["key2"] = modelSeries2;

        validation = activityValidator->checkRequirements(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and two models in the composite should be valid",
                                     true, validation.first);
    }
    {
        // data with an image and 3 models in the composite should NOT be valid
        (*modelCompo)["key3"] = modelSeries2;

        validation = activityValidator->checkRequirements(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and 3 models in the composite should NOT be valid",
                                     false, validation.first);
    }
    {
        // data with an image and an image in the model composite should NOT be valid
        modelCompo->getContainer().erase("key3");
        (*modelCompo)["key2"] = imageSeries;
        validation            = activityValidator->checkRequirements(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and an image in the model composite should NOT be valid",
                                     false, validation.first);
    }
    {
        // data with a null image and a model in the composite should NOT be valid
        modelCompo->getContainer().erase("key2");
        (*data)["imageSeries"] = nullptr;
        validation             = activityValidator->checkRequirements(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with a null image and a model in the composite should NOT be valid",
                                     false, validation.first);
    }
    {
        // data with a null image and a wrong key in the model composite should NOT be valid
        (*modelCompo)["myKey"] = modelSeries2;
        (*data)["imageSeries"] = imageSeries;
        validation             = activityValidator->checkRequirements(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a wrong key in the model composite should NOT be valid",
            false, validation.first);
    }
}

//------------------------------------------------------------------------------

void DefaultActivityTest::parametersTest()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::fwActivities::validator::DefaultActivity");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IActivityValidator::sptr activityValidator =
        ::fwActivities::IActivityValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::ActivitySeries::sptr activitySeries = data::ActivitySeries::New();
    activitySeries->setActivityConfigId("TestValidator0");

    ::fwActivities::IValidator::ValidationType validation;

    data::Composite::sptr data = data::Composite::New();
    activitySeries->setData(data);

    data::ImageSeries::sptr imageSeries = data::ImageSeries::New();
    data::Image::sptr img               = data::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, core::tools::Type::create<std::uint8_t>());
    imageSeries->setImage(img);

    data::ModelSeries::sptr modelSeries  = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries2 = data::ModelSeries::New();
    data::Composite::sptr modelCompo     = data::Composite::New();

    {
        // Empty data should NOT be valid
        validation = activityValidator->checkParameters(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty data should NOT be valid", false, validation.first);
    }
    {
        // data with only an image should NOT be valid
        (*data)["imageSeries"] = imageSeries;

        validation = activityValidator->checkParameters(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with only an image should NOT be valid", false, validation.first);
    }
    {
        // data with an image and an empty model composite should be valid
        (*data)["modelSeries"] = modelCompo;

        validation = activityValidator->checkParameters(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and an empty model composite should be valid",
                                     true, validation.first);
    }
    {
        // data with an image and a model in the composite should be valid
        (*modelCompo)["key1"] = modelSeries;

        validation = activityValidator->checkParameters(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and a model in the composite should be valid",
                                     true, validation.first);
    }
    {
        // data with an image and two models in the composite should be valid
        (*modelCompo)["key2"] = modelSeries2;

        validation = activityValidator->checkParameters(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and two models in the composite should be valid",
                                     true, validation.first);
    }
    {
        // data with a null image and a model in the composite should NOT be valid
        (*data)["imageSeries"] = nullptr;
        validation             = activityValidator->checkParameters(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with a null image and a model in the composite should NOT be valid",
                                     false, validation.first);
    }
}

//------------------------------------------------------------------------------

void DefaultActivityTest::objectTest()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::fwActivities::validator::DefaultActivity");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IActivityValidator::sptr activityValidator =
        ::fwActivities::IActivityValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(activityValidator);

    ::fwActivities::IValidator::ValidationType validation;

    data::Image::sptr img1 = data::Image::New();
    data::Image::sptr img2 = data::Image::New();
    data::Image::sptr img3 = data::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img1, core::tools::Type::create<std::uint8_t>());
    ::fwTest::generator::Image::generateImage(img2, img1->getSize2(), img1->getSpacing2(),
                                              img1->getOrigin2(), img1->getType(),
                                              data::Image::PixelFormat::GRAY_SCALE);
    ::fwTest::generator::Image::generateRandomImage(img3, core::tools::Type::create<std::uint8_t>());

    data::Vector::sptr vector = data::Vector::New();
    vector->getContainer().push_back(img1);
    vector->getContainer().push_back(img2);

    data::Vector::sptr vector2 = data::Vector::New();
    vector2->getContainer().push_back(img1);
    vector2->getContainer().push_back(img3);

    {
        // An empty validator implementation should be valid
        validation = activityValidator->checkObject(img1, "");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("An empty validator implementation should be valid", true, validation.first);
    }
    {
        // An inexisting validator implementation should NOT be valid
        validation = activityValidator->checkObject(img1, "::inexisting::validator");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("An inexisting validator implementation should NOT be valid",
                                     false, validation.first);
    }
    {
        // An existing validator implementation with valid data should be valid
        validation = activityValidator->checkObject(vector, "::fwActivities::validator::ImageProperties");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("An existing validator implementation with valid data should be valid",
                                     true, validation.first);
    }
    {
        // An existing validator implementation with invalid data should NOT be valid
        validation = activityValidator->checkObject(vector2, "::fwActivities::validator::ImageProperties");
        CPPUNIT_ASSERT_EQUAL_MESSAGE("An existing validator implementation with invalid data should NOT be valid",
                                     false, validation.first);
    }
}

//------------------------------------------------------------------------------

void DefaultActivityTest::validatorTest()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::fwActivities::validator::DefaultActivity");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IActivityValidator::sptr activityValidator =
        ::fwActivities::IActivityValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::ActivitySeries::sptr activitySeries = data::ActivitySeries::New();
    activitySeries->setActivityConfigId("TestValidator0");

    ::fwActivities::IValidator::ValidationType validation;

    data::Composite::sptr data = data::Composite::New();
    activitySeries->setData(data);

    data::ImageSeries::sptr imageSeries  = data::ImageSeries::New();
    data::ImageSeries::sptr imageSeries2 = data::ImageSeries::New();
    data::ModelSeries::sptr modelSeries  = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries2 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries3 = data::ModelSeries::New();
    data::Composite::sptr modelCompo     = data::Composite::New();

    data::Image::sptr img = data::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, core::tools::Type::create<std::uint8_t>());
    imageSeries->setImage(img);

    {
        // Empty data should NOT be valid
        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty data should NOT be valid", false, validation.first);
    }
    {
        // data with only an image should NOT be valid
        (*data)["imageSeries"] = imageSeries;

        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with only an image should NOT be valid", false, validation.first);
    }
    {
        // data with an image and an empty model composite should be valid
        (*data)["modelSeries"] = modelCompo;

        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and an empty model composite should be valid",
                                     true, validation.first);
    }
    {
        // data with an image and a model in the composite should be valid
        (*modelCompo)["key1"] = modelSeries;

        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and a model in the composite should be valid",
                                     true, validation.first);
    }
    {
        // data with an image and two models in the composite should be valid
        (*modelCompo)["key2"] = modelSeries2;

        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and two models in the composite should be valid",
                                     true, validation.first);
    }
    {
        // data with an image and 3 models in the composite should NOT be valid
        (*modelCompo)["key3"] = modelSeries2;

        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and 3 models in the composite should NOT be valid",
                                     false, validation.first);
    }
    {
        // data with an image and an image in the model composite should NOT be valid
        modelCompo->getContainer().erase("key3");
        (*modelCompo)["key2"] = imageSeries;
        validation            = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with an image and an image in the model composite should NOT be valid",
                                     false, validation.first);
    }
    {
        // data with a null image and a model in the composite should NOT be valid
        modelCompo->getContainer().erase("key2");
        (*data)["imageSeries"] = nullptr;
        validation             = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("data with a null image and a model in the composite should NOT be valid",
                                     false, validation.first);
    }
    {
        // data with a null image and a wrong key in the model composite should NOT be valid
        (*modelCompo)["myKey"] = modelSeries2;
        (*data)["imageSeries"] = imageSeries;
        validation             = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "data with a null image and a wrong key in the model composite should NOT be valid",
            false, validation.first);
    }
}

//------------------------------------------------------------------------------

void DefaultActivityTest::bigActivityTest()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::fwActivities::validator::DefaultActivity");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IActivityValidator::sptr activityValidator =
        ::fwActivities::IActivityValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(activityValidator);

    data::ActivitySeries::sptr activitySeries = data::ActivitySeries::New();
    activitySeries->setActivityConfigId("TestValidator1");

    ::fwActivities::IValidator::ValidationType validation;

    data::Composite::sptr data = data::Composite::New();
    activitySeries->setData(data);

    data::ImageSeries::sptr imageSeries1 = data::ImageSeries::New();
    data::ImageSeries::sptr imageSeries2 = data::ImageSeries::New();
    data::ImageSeries::sptr imageSeries3 = data::ImageSeries::New();
    data::ModelSeries::sptr modelSeries1 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries2 = data::ModelSeries::New();
    data::ModelSeries::sptr modelSeries3 = data::ModelSeries::New();

    data::TransformationMatrix3D::sptr matrix = data::TransformationMatrix3D::New();

    data::Float::sptr floatObj  = data::Float::New();
    data::Integer::sptr intObj  = data::Integer::New();
    data::Boolean::sptr boolObj = data::Boolean::New();

    data::Composite::sptr compoStr = data::Composite::New();

    (*compoStr)["Item1"] = data::String::New("myItem1");
    (*compoStr)["Item2"] = data::String::New("myItem3");
    (*compoStr)["Item3"] = data::String::New("myItem3");

    data::Vector::sptr modelVector = data::Vector::New();
    data::Vector::sptr imageVector = data::Vector::New();

    data::Image::sptr img1 = data::Image::New();
    data::Image::sptr img2 = data::Image::New();
    data::Image::sptr img3 = data::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img1, core::tools::Type::create<std::uint8_t>());
    ::fwTest::generator::Image::generateRandomImage(img2, core::tools::Type::create<std::uint8_t>());
    ::fwTest::generator::Image::generateImage(img3, img2->getSize2(), img2->getSpacing2(),
                                              img2->getOrigin2(), img2->getType(),
                                              data::Image::PixelFormat::GRAY_SCALE);
    imageSeries1->setImage(img1);
    imageSeries2->setImage(img2);
    imageSeries3->setImage(img3);

    modelVector->getContainer().push_back(modelSeries2);
    modelVector->getContainer().push_back(modelSeries3);
    imageVector->getContainer().push_back(imageSeries2);
    imageVector->getContainer().push_back(imageSeries3);

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
        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with correct data should be valid", true, validation.first);
    }
    {
        data->getContainer().erase("int");
        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity without an autogenerated data should NOT be valid",
                                     false, validation.first);
        (*data)["int"] = intObj;
    }
    {
        data->getContainer().erase("transformationMatrix");
        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity without an autocreated data should NOT be valid",
                                     false, validation.first);
        (*data)["transformationMatrix"] = matrix;
    }
    {
        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with correct data should be valid", true, validation.first);
    }
    {
        const data::Image::Spacing spacing = {45., 45., 45.};
        img3->setSpacing2(spacing);
        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with different image properties data should NOT be valid",
                                     false, validation.first);
        img3->setSpacing2(img2->getSpacing2());
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwActivities
