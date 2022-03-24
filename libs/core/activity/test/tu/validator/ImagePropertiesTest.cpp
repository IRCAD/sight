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

#include "ImagePropertiesTest.hpp"

#include <activity/extension/Activity.hpp>
#include <activity/IObjectValidator.hpp>
#include <activity/IValidator.hpp>

#include <core/runtime/Extension.hpp>
#include <core/runtime/Module.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Vector.hpp>

#include <utestData/generator/Image.hpp>

#include <cstdint>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::activity::ut::ImagePropertiesTest);

namespace sight::activity
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
    activity::IValidator::sptr validator =
        activity::validator::factory::New("sight::activity::validator::ImageProperties");
    CPPUNIT_ASSERT(validator);

    activity::IObjectValidator::sptr objValidator = activity::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    {
        data::Image::sptr img1 = data::Image::New();
        data::Image::sptr img2 = data::Image::New();
        utestData::generator::Image::generateRandomImage(img1, core::Type::UINT8);
        utestData::generator::Image::generateRandomImage(img2, core::Type::UINT8);

        data::Vector::sptr vector = data::Vector::New();
        vector->getContainer().push_back(img1);
        vector->getContainer().push_back(img2);

        activity::IValidator::ValidationType validation;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);

        data::Composite::sptr composite = data::Composite::New();
        (*composite)["img1"] = img1;
        (*composite)["img2"] = img2;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(false, validation.first);
    }

    {
        data::Image::sptr img1 = data::Image::New();
        data::Image::sptr img2 = data::Image::New();

        utestData::generator::Image::generateRandomImage(img1, core::Type::UINT8);

        utestData::generator::Image::generateImage(
            img2,
            img1->getSize(),
            img1->getSpacing(),
            img1->getOrigin(),
            img1->getType(),
            data::Image::PixelFormat::GRAY_SCALE
        );

        data::Vector::sptr vector = data::Vector::New();
        vector->getContainer().push_back(img1);
        vector->getContainer().push_back(img2);

        activity::IValidator::ValidationType validation;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);

        data::Composite::sptr composite = data::Composite::New();
        (*composite)["img1"] = img1;
        (*composite)["img2"] = img2;

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL(true, validation.first);
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::activity
