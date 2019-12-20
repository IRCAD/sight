/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwItkIO/itk.hpp>

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/helper/compare.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwItkIO::ut::ImageConversionTest );

namespace fwItkIO
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageConversionTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
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
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::s_INT16);

    typedef itk::Image< std::int16_t, 3 > ImageType;
    ImageType::Pointer itkImage = ::fwItkIO::itkImageFactory<ImageType>( image );

    ::fwData::Image::sptr image2 = ::fwData::Image::New();
    bool image2ManagesHisBuffer = false;
    ::fwItkIO::dataImageFactory< ImageType >( itkImage, image2, image2ManagesHisBuffer );

    ::fwItkIO::ut::helper::roundSpacing(image);
    ::fwItkIO::ut::helper::roundSpacing(image2);

    ::fwTest::helper::ExcludeSetType exclude;
    exclude.insert("array.isOwner");
    exclude.insert("window_center");
    exclude.insert("window_width");

    CPPUNIT_ASSERT(::fwTest::helper::compare(image, image2, exclude));

    bool image3ManagesHisBuffer = false;
    ::fwData::Image::sptr image3 = ::fwItkIO::dataImageFactory< ImageType >( itkImage, image3ManagesHisBuffer );
    CPPUNIT_ASSERT(::fwTest::helper::compare(image, image3, exclude));
}

//------------------------------------------------------------------------------

void ImageConversionTest::stressTest()
{
    stressTestForAType< std::int8_t >();
    stressTestForAType< std::uint8_t >();

    stressTestForAType< std::int16_t >();
    stressTestForAType< std::uint16_t >();

    stressTestForAType< std::int32_t >();
    stressTestForAType< std::uint32_t >();

    stressTestForAType< std::int64_t >();
    stressTestForAType< std::uint64_t >();

    stressTestForAType< float >();
    stressTestForAType< double >();
}

//------------------------------------------------------------------------------

void ImageConversionTest::testConversion2D()
{
    // create Image
    ::fwData::Image::sptr image      = ::fwData::Image::New();
    ::fwData::Image::Size size       = { static_cast<size_t>(rand()%100 + 2), static_cast<size_t>(rand()%100 + 2), 0};
    ::fwData::Image::Spacing spacing = { (rand()%200 +1) / 100., (rand()%200 +1) / 100., 0.};
    ::fwData::Image::Origin origin   = {( rand()%200 - 100) / 3., (rand()%200 - 100) / 3., 0.};
    ::fwTools::Type type             = ::fwTools::Type::s_INT16;

    ::fwTest::generator::Image::generateImage(image, size, spacing, origin, type, ::fwData::Image::GRAY_SCALE);
    ::fwTest::generator::Image::randomizeImage(image);

    typedef itk::Image< std::int16_t, 2 > ImageType;

    ImageType::Pointer itkImage = ::fwItkIO::itkImageFactory<ImageType>( image );

    ::fwData::Image::sptr image2 = ::fwData::Image::New();
    bool image2ManagesHisBuffer = false;
    ::fwItkIO::dataImageFactory< ImageType >( itkImage, image2, image2ManagesHisBuffer );

    ::fwItkIO::ut::helper::roundSpacing(image);
    ::fwItkIO::ut::helper::roundSpacing(image2);

    ::fwTest::helper::ExcludeSetType exclude;
    exclude.insert("array.isOwner");
    exclude.insert("window_center");
    exclude.insert("window_width");

    CPPUNIT_ASSERT(::fwTest::helper::compare(image, image2, exclude));

    bool image3ManagesHisBuffer = false;
    ::fwData::Image::sptr image3 = ::fwItkIO::dataImageFactory< ImageType >( itkImage, image3ManagesHisBuffer );
    CPPUNIT_ASSERT(::fwTest::helper::compare(image, image3, exclude));
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwItkIO
