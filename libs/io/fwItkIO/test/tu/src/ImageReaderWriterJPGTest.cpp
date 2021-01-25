/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ImageReaderWriterJPGTest.hpp"

#include <fwItkIO/ImageReader.hpp>
#include <fwItkIO/itk.hpp>
#include <fwItkIO/JpgImageWriter.hpp>

#include <core/base.hpp>
#include <core/tools/System.hpp>

#include <data/Image.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

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
    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::tools::Type::create("int16"));

    // save image in inr
    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "imageJPG";
    std::filesystem::create_directories( PATH );
    ::fwItkIO::JpgImageWriter::sptr myWriter = ::fwItkIO::JpgImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::testImageWriter2()
{
    // create Image
    std::filesystem::path pathInr = utestData::Data::dir() / "sight/image/inr/image.inr.gz";

    CPPUNIT_ASSERT_MESSAGE("The file '" + pathInr.string() + "' does not exist",
                           std::filesystem::exists(pathInr));

    data::Image::sptr image = data::Image::New();
    ::fwItkIO::ImageReader::sptr myReader = ::fwItkIO::ImageReader::New();
    myReader->setObject(image);
    myReader->setFile(pathInr);
    myReader->read();

    // save image in inr
    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "imageJPG";
    std::filesystem::create_directories( PATH );
    ::fwItkIO::JpgImageWriter::sptr myWriter = ::fwItkIO::JpgImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwItkIO
