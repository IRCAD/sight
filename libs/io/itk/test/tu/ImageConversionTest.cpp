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

#include "ImageConversionTest.hpp"

#include "helper.hpp"

#include <core/base.hpp>
#include <core/tools/random/generator.hpp>

#include <data/Object.hpp>

#include <io/itk/itk.hpp>

#include <utestData/generator/Image.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::itk::ut::ImageConversionTest);

namespace sight::io::itk::ut
{

using core::tools::random::safe_rand;

//------------------------------------------------------------------------------

void ImageConversionTest::setUp()
{
}

//------------------------------------------------------------------------------

void ImageConversionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageConversionTest::testConversion()
{
    // create Image
    data::Image::sptr image = std::make_shared<data::Image>();
    utestData::generator::Image::generateRandomImage(image, core::type::INT16);

    using ImageType = ::itk::Image<std::int16_t, 3>;
    ImageType::Pointer itkImage = io::itk::moveToItk<ImageType>(image);

    data::Image::sptr image2 = std::make_shared<data::Image>();
    io::itk::moveFromItk<ImageType>(itkImage, image2, false);

    io::itk::ut::helper::roundSpacing(image);
    io::itk::ut::helper::roundSpacing(image2);

    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    CPPUNIT_ASSERT(*image == *image2);

    data::Image::sptr image3 = io::itk::moveFromItk<ImageType>(itkImage, false);
    image3->setWindowCenter(image->getWindowCenter());
    image3->setWindowWidth(image->getWindowWidth());

    CPPUNIT_ASSERT(*image == *image3);
}

//------------------------------------------------------------------------------

void ImageConversionTest::stressTest()
{
    stressTestForAType<std::int8_t>();
    stressTestForAType<std::uint8_t>();

    stressTestForAType<std::int16_t>();
    stressTestForAType<std::uint16_t>();

    stressTestForAType<std::int32_t>();
    stressTestForAType<std::uint32_t>();

    stressTestForAType<std::int64_t>();
    stressTestForAType<std::uint64_t>();

    stressTestForAType<float>();
    stressTestForAType<double>();
}

//------------------------------------------------------------------------------

void ImageConversionTest::testConversion2D()
{
    // create Image
    data::Image::sptr image = std::make_shared<data::Image>();
    data::Image::Size size  =
    {static_cast<std::size_t>(safe_rand() % 100 + 2), static_cast<std::size_t>(safe_rand() % 100 + 2), 0
    };
    data::Image::Spacing spacing = {(safe_rand() % 200 + 1) / 100., (safe_rand() % 200 + 1) / 100., 0.};
    data::Image::Origin origin   = {(safe_rand() % 200 - 100) / 3., (safe_rand() % 200 - 100) / 3., 0.};
    core::type type              = core::type::INT16;

    utestData::generator::Image::generateImage(image, size, spacing, origin, type, data::Image::GRAY_SCALE, 0);

    using ImageType = ::itk::Image<std::int16_t, 2>;

    ImageType::Pointer itkImage = io::itk::moveToItk<ImageType>(image);

    data::Image::sptr image2    = std::make_shared<data::Image>();
    bool image2ManagesHisBuffer = false;
    io::itk::moveFromItk<ImageType>(itkImage, image2, image2ManagesHisBuffer);

    io::itk::ut::helper::roundSpacing(image);
    io::itk::ut::helper::roundSpacing(image2);

    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    CPPUNIT_ASSERT(*image == *image2);

    data::Image::sptr image3 = io::itk::moveFromItk<ImageType>(itkImage, false);
    image3->setWindowCenter(image->getWindowCenter());
    image3->setWindowWidth(image->getWindowWidth());

    CPPUNIT_ASSERT(*image == *image3);
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk::ut
