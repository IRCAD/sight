/************************************************************************
 *
 * Copyright (C) 2016 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

#include "validator/ImagePropertiesTest.hpp"

#include <fwActivities/IObjectValidator.hpp>
#include <fwActivities/IValidator.hpp>
#include <fwActivities/registry/Activities.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Vector.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Extension.hpp>
#include <fwRuntime/io/BundleDescriptorReader.hpp>

#include <fwTest/generator/Image.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwActivities::ut::ImagePropertiesTest );

namespace fwActivities
{
namespace ut
{

//------------------------------------------------------------------------------

void ImagePropertiesTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ImagePropertiesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImagePropertiesTest::propertiesTest()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::fwActivities::validator::ImageProperties");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IObjectValidator::sptr objValidator = ::fwActivities::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    {
        ::fwData::Image::sptr img1 = ::fwData::Image::New();
        ::fwData::Image::sptr img2 = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(img1, ::fwTools::Type::create<std::uint8_t>());
        ::fwTest::generator::Image::generateRandomImage(img2, ::fwTools::Type::create<std::uint8_t>());

        ::fwData::Vector::sptr vector = ::fwData::Vector::New();
        vector->getContainer().push_back(img1);
        vector->getContainer().push_back(img2);

        ::fwActivities::IValidator::ValidationType validation;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);

        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        (*composite)["img1"]                = img1;
        (*composite)["img2"]                = img2;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);
    }


    {
        ::fwData::Image::SizeType size(3, 100);
        ::fwData::Image::sptr img1 = ::fwData::Image::New();
        ::fwData::Image::sptr img2 = ::fwData::Image::New();

        ::fwTest::generator::Image::generateRandomImage(img1, ::fwTools::Type::create<std::uint8_t>());

        ::fwTest::generator::Image::generateImage(img2, img1->getSize(), img1->getSpacing(),
                                                  img1->getOrigin(), img1->getType());

        ::fwData::Vector::sptr vector = ::fwData::Vector::New();
        vector->getContainer().push_back(img1);
        vector->getContainer().push_back(img2);

        ::fwActivities::IValidator::ValidationType validation;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);

        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        (*composite)["img1"]                = img1;
        (*composite)["img2"]                = img2;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwActivities
