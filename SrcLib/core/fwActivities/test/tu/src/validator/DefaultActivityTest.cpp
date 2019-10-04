/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Float.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwDataCamp/Version.hpp>

#include <fwMedData/ActivitySeries.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwMedDataCamp/Version.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/io/BundleDescriptorReader.hpp>
#include <fwRuntime/operations.hpp>

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
    //Force link with fwDataCamp, it is used by the validator to parse the objects
    m_fwDataCampVersion = ::fwDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(m_fwDataCampVersion);

    //Force link with fwMedDataCamp, it is used by the validator to parse the objects
    m_fwMedDataCampVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(m_fwMedDataCampVersion);

    // Set up context before running a test.
    std::filesystem::path plugin = ::fwRuntime::getResourceFilePath("tu_exec_fwActivities-0.0/tu_validator");
    m_bundle = ::fwRuntime::io::BundleDescriptorReader::createBundle(plugin);

    ::fwRuntime::Bundle::ExtensionContainer extensionsSet( m_bundle->extensionsBegin(), m_bundle->extensionsEnd());
    std::vector< SPTR( ::fwRuntime::Extension ) > extensions(extensionsSet.begin(), extensionsSet.end());
    ::fwActivities::registry::Activities::getDefault()->parseBundleInformation(extensions);

    CPPUNIT_ASSERT_EQUAL( size_t(2), extensions.size());
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

    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::New();
    activitySeries->setActivityConfigId("Test0");

    ::fwActivities::IValidator::ValidationType validation;

    ::fwData::Composite::sptr data = ::fwData::Composite::New();
    activitySeries->setData(data);

    ::fwMedData::ImageSeries::sptr imageSeries  = ::fwMedData::ImageSeries::New();
    ::fwMedData::ImageSeries::sptr imageSeries2 = ::fwMedData::ImageSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries  = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries2 = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries3 = ::fwMedData::ModelSeries::New();
    ::fwData::Composite::sptr modelCompo        = ::fwData::Composite::New();

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

    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::New();
    activitySeries->setActivityConfigId("Test0");

    ::fwActivities::IValidator::ValidationType validation;

    ::fwData::Composite::sptr data = ::fwData::Composite::New();
    activitySeries->setData(data);

    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::New();
    ::fwData::Image::sptr img                  = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, ::fwTools::Type::create<std::uint8_t>());
    imageSeries->setImage(img);

    ::fwMedData::ModelSeries::sptr modelSeries  = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries2 = ::fwMedData::ModelSeries::New();
    ::fwData::Composite::sptr modelCompo        = ::fwData::Composite::New();

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

    ::fwData::Image::sptr img1 = ::fwData::Image::New();
    ::fwData::Image::sptr img2 = ::fwData::Image::New();
    ::fwData::Image::sptr img3 = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img1, ::fwTools::Type::create<std::uint8_t>());
    ::fwTest::generator::Image::generateImage(img2, img1->getSize(), img1->getSpacing(),
                                              img1->getOrigin(), img1->getType());
    ::fwTest::generator::Image::generateRandomImage(img3, ::fwTools::Type::create<std::uint8_t>());

    ::fwData::Vector::sptr vector = ::fwData::Vector::New();
    vector->getContainer().push_back(img1);
    vector->getContainer().push_back(img2);

    ::fwData::Vector::sptr vector2 = ::fwData::Vector::New();
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

    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::New();
    activitySeries->setActivityConfigId("Test0");

    ::fwActivities::IValidator::ValidationType validation;

    ::fwData::Composite::sptr data = ::fwData::Composite::New();
    activitySeries->setData(data);

    ::fwMedData::ImageSeries::sptr imageSeries  = ::fwMedData::ImageSeries::New();
    ::fwMedData::ImageSeries::sptr imageSeries2 = ::fwMedData::ImageSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries  = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries2 = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries3 = ::fwMedData::ModelSeries::New();
    ::fwData::Composite::sptr modelCompo        = ::fwData::Composite::New();

    ::fwData::Image::sptr img = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, ::fwTools::Type::create<std::uint8_t>());
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

    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::New();
    activitySeries->setActivityConfigId("Test1");

    ::fwActivities::IValidator::ValidationType validation;

    ::fwData::Composite::sptr data = ::fwData::Composite::New();
    activitySeries->setData(data);

    ::fwMedData::ImageSeries::sptr imageSeries1 = ::fwMedData::ImageSeries::New();
    ::fwMedData::ImageSeries::sptr imageSeries2 = ::fwMedData::ImageSeries::New();
    ::fwMedData::ImageSeries::sptr imageSeries3 = ::fwMedData::ImageSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries1 = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries2 = ::fwMedData::ModelSeries::New();
    ::fwMedData::ModelSeries::sptr modelSeries3 = ::fwMedData::ModelSeries::New();

    ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::New();

    ::fwData::Float::sptr floatObj  = ::fwData::Float::New();
    ::fwData::Integer::sptr intObj  = ::fwData::Integer::New();
    ::fwData::Boolean::sptr boolObj = ::fwData::Boolean::New();

    ::fwData::Composite::sptr compoStr = ::fwData::Composite::New();

    (*compoStr)["Item1"] = ::fwData::String::New("myItem1");
    (*compoStr)["Item2"] = ::fwData::String::New("myItem3");
    (*compoStr)["Item3"] = ::fwData::String::New("myItem3");

    ::fwData::Vector::sptr modelVector = ::fwData::Vector::New();
    ::fwData::Vector::sptr imageVector = ::fwData::Vector::New();

    ::fwData::Image::sptr img1 = ::fwData::Image::New();
    ::fwData::Image::sptr img2 = ::fwData::Image::New();
    ::fwData::Image::sptr img3 = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img1, ::fwTools::Type::create<std::uint8_t>());
    ::fwTest::generator::Image::generateRandomImage(img2, ::fwTools::Type::create<std::uint8_t>());
    ::fwTest::generator::Image::generateImage(img3, img2->getSize(), img2->getSpacing(),
                                              img2->getOrigin(), img2->getType());
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
        ::fwData::Image::SpacingType spacing(3, 45);
        img3->setSpacing(spacing);
        validation = activityValidator->validate(activitySeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Activity with different image properties data should NOT be valid",
                                     false, validation.first);
        img3->setSpacing(img2->getSpacing());
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwActivities
