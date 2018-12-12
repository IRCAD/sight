/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ImageReaderWriterJPGTest.hpp"

#include <fwItkIO/ImageReader.hpp>
#include <fwItkIO/itk.hpp>
#include <fwItkIO/JpgImageWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

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
    ::boost::filesystem::path pathInr = ::fwTest::Data::dir() / "sight/image/inr/image.inr.gz";

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
