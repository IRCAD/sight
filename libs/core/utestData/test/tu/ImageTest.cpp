/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/tools/Type.hpp>

#include <data/reflection/visitor/CompareObjects.hpp>

#include <utestData/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::utestData::ut::ImageTest);

namespace sight::utestData
{

namespace ut
{

//-----------------------------------------------------------------------------

void compare(data::Object::sptr objRef, data::Object::sptr objComp)
{
    data::reflection::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(data::reflection::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for(data::reflection::visitor::CompareObjects::PropsMapType::value_type prop : (*props))
    {
        SIGHT_ERROR("new object difference found : " << prop.first << " '" << prop.second << "'");
    }

    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0);
}

//------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageTest::generator3DTest()
{
    data::Image::sptr image            = data::Image::New();
    const data::Image::Size size       = {10, 15, 23};
    const data::Image::Spacing spacing = {0.85, 2.6, 1.87};
    const data::Image::Origin origin   = {-45.6, 25.97, -53.9};
    core::tools::Type type             = core::tools::Type::create<std::int32_t>();

    utestData::generator::Image::generateImage(
        image,
        size,
        spacing,
        origin,
        type,
        data::Image::PixelFormat::GRAY_SCALE
    );

    CPPUNIT_ASSERT(image->getSize2() == size);
    CPPUNIT_ASSERT(image->getSpacing2() == spacing);
    CPPUNIT_ASSERT(image->getOrigin2() == origin);
    CPPUNIT_ASSERT_EQUAL(type.string(), image->getType().string());
}

//------------------------------------------------------------------------------

void ImageTest::generator2DTest()
{
    data::Image::sptr image            = data::Image::New();
    const data::Image::Size size       = {10, 15, 0};
    const data::Image::Spacing spacing = {0.85, 2.6, 0.};
    const data::Image::Origin origin   = {-45.6, 25.97, 0.};
    core::tools::Type type             = core::tools::Type::create<std::int16_t>();

    utestData::generator::Image::generateImage(
        image,
        size,
        spacing,
        origin,
        type,
        data::Image::PixelFormat::GRAY_SCALE
    );
    CPPUNIT_ASSERT(image->getSize2() == size);
    CPPUNIT_ASSERT(image->getSpacing2() == spacing);
    CPPUNIT_ASSERT(image->getOrigin2() == origin);
    CPPUNIT_ASSERT_EQUAL(type.string(), image->getType().string());
}

//------------------------------------------------------------------------------

void ImageTest::deepCopyTest()
{
    data::Image::sptr image = data::Image::New();
    data::Image::sptr image2;
    core::tools::Type type = core::tools::Type::create<std::int32_t>();
    utestData::generator::Image::generateRandomImage(image, type);

    image2 = data::Object::copy(image);
    compare(image, image2);
}

//------------------------------------------------------------------------------

void ImageTest::stressTest()
{
    core::tools::Type type = core::tools::Type::create<std::int8_t>();
    this->stressTestWithType(type, 10);

    type = core::tools::Type::create<std::uint8_t>();
    this->stressTestWithType(type, 10);

    type = core::tools::Type::create<std::int16_t>();
    this->stressTestWithType(type, 10);

    type = core::tools::Type::create<std::uint16_t>();
    this->stressTestWithType(type, 10);

    type = core::tools::Type::create<std::int32_t>();
    this->stressTestWithType(type, 10);

    type = core::tools::Type::create<std::uint32_t>();
    this->stressTestWithType(type, 10);

    type = core::tools::Type::create<std::int64_t>();
    this->stressTestWithType(type, 10);

    type = core::tools::Type::create<std::uint64_t>();
    this->stressTestWithType(type, 10);

    type = core::tools::Type::create<float>();
    this->stressTestWithType(type, 10);

    type = core::tools::Type::create<double>();
    this->stressTestWithType(type, 10);
}

//------------------------------------------------------------------------------

void ImageTest::stressTestWithType(core::tools::Type type, int nbTest)
{
    for(int nb = 0 ; nb < nbTest ; ++nb)
    {
        data::Image::sptr image = data::Image::New();
        utestData::generator::Image::generateRandomImage(image, type);

        data::Image::sptr image2;
        image2 = data::Object::copy(image);
        compare(image, image2);
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::utestData
