/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataTools/Image.hpp>

#include <itkIO/itk.hpp>

#include "ImageConversionTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ImageConversionTest );

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
    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    typedef itk::Image< ::boost::int16_t , 3 > ImageType;
    ImageType::Pointer itkImage = ::itkIO::itkImageFactory<ImageType>( image );

    ::fwData::Image::NewSptr image2;
    ::itkIO::dataImageFactory< ImageType >( itkImage, image2 );
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image2));

    ::fwData::Image::sptr image3 = ::itkIO::dataImageFactory< ImageType >( itkImage );
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image3));

    image2->getDataArray()->setIsBufferOwner( false );
    image3->getDataArray()->setIsBufferOwner( false );
}


//------------------------------------------------------------------------------

void ImageConversionTest::stressTest()
{
    stressTestForAType< ::boost::int8_t >();
    stressTestForAType< ::boost::uint8_t >();

    stressTestForAType< ::boost::int16_t >();
    stressTestForAType< ::boost::uint16_t >();

    stressTestForAType< ::boost::int32_t >();
    stressTestForAType< ::boost::uint32_t >();

    stressTestForAType< ::boost::int64_t >();
    stressTestForAType< ::boost::uint64_t >();

    stressTestForAType< float >();
    stressTestForAType< double >();
}

//------------------------------------------------------------------------------
