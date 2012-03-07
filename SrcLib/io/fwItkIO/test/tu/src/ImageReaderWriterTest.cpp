/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwDataTools/Image.hpp>

#include <itkIO/ImageWriter.hpp>
#include <itkIO/ImageReader.hpp>


#include "ImageReaderWriterTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ImageReaderWriterTest );

//------------------------------------------------------------------------------

void ImageReaderWriterTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testSaveLoadInr()
{
    // create Image
    ::fwData::Image::NewSptr image;
    ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));
    this->checkSaveLoadInr( image );
}


//------------------------------------------------------------------------------

void ImageReaderWriterTest::stressTestInr()
{
    ::fwTools::Type type = ::fwTools::Type::create< ::boost::int8_t >();
//    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< ::boost::uint8_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< ::boost::int16_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< ::boost::uint16_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< ::boost::int32_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< ::boost::uint32_t >();
    this->stressTestInrWithType(type, 5);

//    type = ::fwTools::Type::create< ::boost::int64_t >();
//    this->stressTestInrWithType(type, 5);

//    type = ::fwTools::Type::create< ::boost::uint64_t >();
//    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< float >();
    this->stressTestInrWithType(type, 5);

//    type = ::fwTools::Type::create< double >();
//    this->stressTestInrWithType(type, 5);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::stressTestInrWithType(::fwTools::Type type, int nbTest)
{
    for (int nb=0 ; nb < nbTest ; ++nb)
    {
        ::fwData::Image::NewSptr image;
        ::fwDataTools::Image::generateRandomImage(image, type);
        this->checkSaveLoadInr(image);
    }
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::checkSaveLoadInr( ::fwData::Image::NewSptr image )
{
    // inr only support image origin (0,0,0)
    ::fwData::Image::OriginType origin(3,0);
    image->setOrigin(origin);

    // save image in inr
    const ::boost::filesystem::path PATH = "imageInrTest/image.inr.gz";
    ::boost::filesystem::create_directories( PATH.parent_path() );
    ::itkIO::ImageWriter::NewSptr myWriter;
    myWriter->setObject(image);
    myWriter->setFile(PATH);
    myWriter->write();

    // load Image
    ::fwData::Image::NewSptr image2;
    ::itkIO::ImageReader::NewSptr myReader;
    myReader->setObject(image2);
    myReader->setFile(PATH);
    myReader->read();

    ::boost::filesystem::remove_all( PATH.parent_path().string() );

    // check Image
    // inr only support float spacing and float origin => add tolerance for comparison (+/-0.00001)
    CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image2, 0.00001, 0.00001));
}

//------------------------------------------------------------------------------
