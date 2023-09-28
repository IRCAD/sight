/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ImageTest.hpp"

#include <core/type.hpp>

#include <data/Image.hpp>

#include <utestData/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::utestData::ut::ImageTest);

namespace sight::utestData::ut
{

//------------------------------------------------------------------------------

void ImageTest::setUp()
{
}

//------------------------------------------------------------------------------

void ImageTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ImageTest::generator3DTest()
{
    data::Image::sptr image            = std::make_shared<data::Image>();
    const data::Image::Size size       = {10, 15, 23};
    const data::Image::Spacing spacing = {0.85, 2.6, 1.87};
    const data::Image::Origin origin   = {-45.6, 25.97, -53.9};
    core::type type                    = core::type::INT32;

    utestData::generator::Image::generateImage(
        image,
        size,
        spacing,
        origin,
        type,
        data::Image::PixelFormat::GRAY_SCALE
    );

    CPPUNIT_ASSERT(image->size() == size);
    CPPUNIT_ASSERT(image->getSpacing() == spacing);
    CPPUNIT_ASSERT(image->getOrigin() == origin);
    CPPUNIT_ASSERT_EQUAL(type, image->getType());
}

//------------------------------------------------------------------------------

void ImageTest::generator2DTest()
{
    data::Image::sptr image            = std::make_shared<data::Image>();
    const data::Image::Size size       = {10, 15, 0};
    const data::Image::Spacing spacing = {0.85, 2.6, 0.};
    const data::Image::Origin origin   = {-45.6, 25.97, 0.};
    core::type type                    = core::type::INT16;

    utestData::generator::Image::generateImage(
        image,
        size,
        spacing,
        origin,
        type,
        data::Image::PixelFormat::GRAY_SCALE
    );
    CPPUNIT_ASSERT(image->size() == size);
    CPPUNIT_ASSERT(image->getSpacing() == spacing);
    CPPUNIT_ASSERT(image->getOrigin() == origin);
    CPPUNIT_ASSERT_EQUAL(type, image->getType());
}

//------------------------------------------------------------------------------

void ImageTest::deep_copyTest()
{
    data::Image::sptr image = std::make_shared<data::Image>();
    data::Image::sptr image2;
    core::type type = core::type::INT32;
    utestData::generator::Image::generateRandomImage(image, type);

    image2 = data::Object::copy(image);
    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

inline static void stressTestWithType(core::type type, int nbTest)
{
    for(int nb = 0 ; nb < nbTest ; ++nb)
    {
        data::Image::sptr image = std::make_shared<data::Image>();
        utestData::generator::Image::generateRandomImage(image, type);

        data::Image::sptr image2;
        image2 = data::Object::copy(image);
        CPPUNIT_ASSERT(*image == *image2);
    }
}

//------------------------------------------------------------------------------

void ImageTest::stressTest()
{
    core::type type = core::type::INT8;
    stressTestWithType(type, 10);

    type = core::type::UINT8;
    stressTestWithType(type, 10);

    type = core::type::INT16;
    stressTestWithType(type, 10);

    type = core::type::UINT16;
    stressTestWithType(type, 10);

    type = core::type::INT32;
    stressTestWithType(type, 10);

    type = core::type::UINT32;
    stressTestWithType(type, 10);

    type = core::type::INT64;
    stressTestWithType(type, 10);

    type = core::type::UINT64;
    stressTestWithType(type, 10);

    type = core::type::FLOAT;
    stressTestWithType(type, 10);

    type = core::type::DOUBLE;
    stressTestWithType(type, 10);
}

//------------------------------------------------------------------------------

} // namespace sight::utestData::ut
