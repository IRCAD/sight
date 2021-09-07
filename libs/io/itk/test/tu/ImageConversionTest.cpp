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

#include "ImageConversionTest.hpp"

#include "helper.hpp"

#include <core/base.hpp>
#include <core/tools/random/Generator.hpp>

#include <data/Object.hpp>

#include <io/itk/itk.hpp>

#include <utestData/generator/Image.hpp>
#include <utestData/helper/compare.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::itk::ut::ImageConversionTest);

namespace sight::io::itk
{

namespace ut
{

using core::tools::random::safeRand;

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
    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::tools::Type::s_INT16);

    typedef ::itk::Image<std::int16_t, 3> ImageType;
    ImageType::Pointer itkImage = io::itk::itkImageFactory<ImageType>(image);

    data::Image::sptr image2    = data::Image::New();
    bool image2ManagesHisBuffer = false;
    io::itk::dataImageFactory<ImageType>(itkImage, image2, image2ManagesHisBuffer);

    io::itk::ut::helper::roundSpacing(image);
    io::itk::ut::helper::roundSpacing(image2);

    utestData::helper::ExcludeSetType exclude;
    exclude.insert("array.isOwner");
    exclude.insert("window_center");
    exclude.insert("window_width");

    CPPUNIT_ASSERT(utestData::helper::compare(image, image2, exclude));

    bool image3ManagesHisBuffer = false;
    data::Image::sptr image3    = io::itk::dataImageFactory<ImageType>(itkImage, image3ManagesHisBuffer);
    CPPUNIT_ASSERT(utestData::helper::compare(image, image3, exclude));
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
    data::Image::sptr image = data::Image::New();
    data::Image::Size size  = {static_cast<size_t>(safeRand() % 100 + 2), static_cast<size_t>(safeRand() % 100 + 2), 0
    };
    data::Image::Spacing spacing = {(safeRand() % 200 + 1) / 100., (safeRand() % 200 + 1) / 100., 0.};
    data::Image::Origin origin   = {(safeRand() % 200 - 100) / 3., (safeRand() % 200 - 100) / 3., 0.};
    core::tools::Type type       = core::tools::Type::s_INT16;

    utestData::generator::Image::generateImage(image, size, spacing, origin, type, data::Image::GRAY_SCALE);
    utestData::generator::Image::randomizeImage(image);

    typedef ::itk::Image<std::int16_t, 2> ImageType;

    ImageType::Pointer itkImage = io::itk::itkImageFactory<ImageType>(image);

    data::Image::sptr image2    = data::Image::New();
    bool image2ManagesHisBuffer = false;
    io::itk::dataImageFactory<ImageType>(itkImage, image2, image2ManagesHisBuffer);

    io::itk::ut::helper::roundSpacing(image);
    io::itk::ut::helper::roundSpacing(image2);

    utestData::helper::ExcludeSetType exclude;
    exclude.insert("array.isOwner");
    exclude.insert("window_center");
    exclude.insert("window_width");

    CPPUNIT_ASSERT(utestData::helper::compare(image, image2, exclude));

    bool image3ManagesHisBuffer = false;
    data::Image::sptr image3    = io::itk::dataImageFactory<ImageType>(itkImage, image3ManagesHisBuffer);
    CPPUNIT_ASSERT(utestData::helper::compare(image, image3, exclude));
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::io::itk
