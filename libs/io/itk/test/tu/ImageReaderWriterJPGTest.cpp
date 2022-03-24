/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/base.hpp>
#include <core/tools/System.hpp>

#include <data/Image.hpp>

#include <io/itk/ImageReader.hpp>
#include <io/itk/itk.hpp>
#include <io/itk/JpgImageWriter.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::itk::ut::ImageReaderWriterJPGTest);

namespace sight::io::itk
{

namespace ut
{

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::setUp()
{
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::testImageWriter()
{
    // create Image
    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::Type::INT16);

    // save image in inr
    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "imageJPG";
    std::filesystem::create_directories(PATH);
    io::itk::JpgImageWriter::sptr myWriter = io::itk::JpgImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
}

//------------------------------------------------------------------------------

void ImageReaderWriterJPGTest::testImageWriter2()
{
    // create Image
    std::filesystem::path pathInr = utestData::Data::dir() / "sight/image/inr/image.inr.gz";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + pathInr.string() + "' does not exist",
        std::filesystem::exists(pathInr)
    );

    data::Image::sptr image             = data::Image::New();
    io::itk::ImageReader::sptr myReader = io::itk::ImageReader::New();
    myReader->setObject(image);
    myReader->setFile(pathInr);
    myReader->read();

    // save image in inr
    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "imageJPG";
    std::filesystem::create_directories(PATH);
    io::itk::JpgImageWriter::sptr myWriter = io::itk::JpgImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFolder(PATH);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::io::itk
