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

#include "ImageReaderWriterTest.hpp"

#include "helper.hpp"

#include <fwItkIO/ImageReader.hpp>
#include <fwItkIO/ImageWriter.hpp>

#include <fwCore/base.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/helper/compare.hpp>

#include <fwTools/System.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwItkIO::ut::ImageReaderWriterTest );

namespace fwItkIO
{
namespace ut
{

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
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));
    this->checkSaveLoadInr( image );
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::stressTestInr()
{
    ::fwTools::Type type = ::fwTools::Type::create< std::int8_t >();
//    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< std::uint8_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< std::int16_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< std::uint16_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< std::int32_t >();
    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< std::uint32_t >();
    this->stressTestInrWithType(type, 5);

//    type = ::fwTools::Type::create< std::int64_t >();
//    this->stressTestInrWithType(type, 5);

//    type = ::fwTools::Type::create< std::uint64_t >();
//    this->stressTestInrWithType(type, 5);

    type = ::fwTools::Type::create< float >();
    this->stressTestInrWithType(type, 5);

//    type = ::fwTools::Type::create< double >();
//    this->stressTestInrWithType(type, 5);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::stressTestInrWithType(::fwTools::Type type, int nbTest)
{
    for (int nb = 0; nb < nbTest; ++nb)
    {
        ::fwData::Image::sptr image = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(image, type);
        this->checkSaveLoadInr(image);
    }
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::checkSaveLoadInr( ::fwData::Image::sptr image )
{
    // inr only support image origin (0,0,0)
    ::fwData::Image::OriginType origin(3, 0);
    image->setOrigin(origin);

    // save image in inr
    const std::filesystem::path PATH = ::fwTools::System::getTemporaryFolder() / "imageInrTest/image.inr.gz";
    std::filesystem::create_directories( PATH.parent_path() );
    ::fwItkIO::ImageWriter::sptr myWriter = ::fwItkIO::ImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFile(PATH);
    myWriter->write();

    // load Image
    ::fwData::Image::sptr image2          = ::fwData::Image::New();
    ::fwItkIO::ImageReader::sptr myReader = ::fwItkIO::ImageReader::New();
    myReader->setObject(image2);
    myReader->setFile(PATH);
    myReader->read();

    ::fwItkIO::ut::helper::roundSpacing(image2);

    ::fwTest::helper::ExcludeSetType exclude;
    exclude.insert("window_center");
    exclude.insert("window_width");
    // check Image
    // inr only support float spacing and float origin => add tolerance for comparison (+/-0.00001)
    CPPUNIT_ASSERT(::fwTest::helper::compare(image, image2, exclude));
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwItkIO
