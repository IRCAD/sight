/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

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
    ::fwData::Image::sptr image = ::fwData::Image::New();
    const std::uint8_t dim = 2;
    ::fwData::Image::SizeType size(dim);
    size[0] = rand()%100 + 2;
    size[1] = rand()%100 + 2;
    std::vector<double> spacing(dim);
    spacing[0] = (rand()%200 +1) / 100.;
    spacing[1] = (rand()%200 +1) / 100.;
    std::vector<double> origin(dim);
    origin[0]            = (rand()%200 - 100) / 3.;
    origin[1]            = (rand()%200 - 100) / 3.;
    ::fwTools::Type type = ::fwTools::Type::create< std::int16_t >();

    ::fwTest::generator::Image::generateImage(image, size, spacing, origin, type);
    ::fwData::Array::sptr array = image->getDataArray();
    ::fwTest::generator::Image::randomizeArray(array);

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
