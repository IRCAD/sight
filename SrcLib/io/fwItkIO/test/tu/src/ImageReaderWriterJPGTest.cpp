/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ImageReaderWriterJPGTest.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwItkIO/ImageReader.hpp>
#include <fwItkIO/JpgImageWriter.hpp>
#include <fwItkIO/itk.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/generator/Image.hpp>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwItkIO::ut::ImageReaderWriterJPGTest );

namespace fwItkIO
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::setUp()
{
    // Set up context before running a test.
    srand(time(NULL));
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::testImageWriter()
{
    // create Image
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    // save image in inr
    const ::boost::filesystem::path PATH = "imageJPG";
    ::boost::filesystem::create_directories( PATH );
    ::fwItkIO::JpgImageWriter::sptr myWriter = ::fwItkIO::JpgImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::testImageWriter2()
{
    // create Image
    ::boost::filesystem::path pathInr = ::fwTest::Data::dir() / "fw4spl/image/inr/image.inr.gz";

    CPPUNIT_ASSERT_MESSAGE("The file '" + pathInr.string() + "' does not exist",
                           ::boost::filesystem::exists(pathInr));

    ::fwData::Image::sptr image           = ::fwData::Image::New();
    ::fwItkIO::ImageReader::sptr myReader = ::fwItkIO::ImageReader::New();
    myReader->setObject(image);
    myReader->setFile(pathInr);
    myReader->read();

    // save image in inr
    const ::boost::filesystem::path PATH = "imageJPG";
    ::boost::filesystem::create_directories( PATH );
    ::fwItkIO::JpgImageWriter::sptr myWriter = ::fwItkIO::JpgImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());

    ::boost::filesystem::remove_all( PATH.string() );
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwItkIO
