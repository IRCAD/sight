/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ImageReaderWriterTest.hpp"

#include "helper.hpp"

#include <core/base.hpp>
#include <core/tools/System.hpp>

#include <io/itk/ImageReader.hpp>
#include <io/itk/ImageWriter.hpp>

#include <utestData/generator/Image.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::itk::ut::ImageReaderWriterTest);

namespace sight::io::itk
{

namespace ut
{

//------------------------------------------------------------------------------

void ImageReaderWriterTest::setUp()
{
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testSaveLoadInr()
{
    // create Image
    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::tools::Type::create("int16"));
    this->checkSaveLoadInr(image);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::stressTestInr()
{
    core::tools::Type type = core::tools::Type::create<std::int8_t>();
//    this->stressTestInrWithType(type, 5);

    type = core::tools::Type::create<std::uint8_t>();
    this->stressTestInrWithType(type, 5);

    type = core::tools::Type::create<std::int16_t>();
    this->stressTestInrWithType(type, 5);

    type = core::tools::Type::create<std::uint16_t>();
    this->stressTestInrWithType(type, 5);

    type = core::tools::Type::create<std::int32_t>();
    this->stressTestInrWithType(type, 5);

    type = core::tools::Type::create<std::uint32_t>();
    this->stressTestInrWithType(type, 5);

//    type = core::tools::Type::create< std::int64_t >();
//    this->stressTestInrWithType(type, 5);

//    type = core::tools::Type::create< std::uint64_t >();
//    this->stressTestInrWithType(type, 5);

    type = core::tools::Type::create<float>();
    this->stressTestInrWithType(type, 5);

//    type = core::tools::Type::create< double >();
//    this->stressTestInrWithType(type, 5);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::stressTestInrWithType(core::tools::Type type, int nbTest)
{
    for(int nb = 0 ; nb < nbTest ; ++nb)
    {
        data::Image::sptr image = data::Image::New();
        utestData::generator::Image::generateRandomImage(image, type);
        this->checkSaveLoadInr(image);
    }
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::checkSaveLoadInr(data::Image::sptr image)
{
    // inr only support image origin (0,0,0)
    const data::Image::Origin origin = {0., 0., 0.};
    image->setOrigin(origin);

    // save image in inr
    const std::filesystem::path PATH = core::tools::System::getTemporaryFolder() / "imageInrTest/image.inr.gz";
    std::filesystem::create_directories(PATH.parent_path());
    io::itk::ImageWriter::sptr myWriter = io::itk::ImageWriter::New();
    myWriter->setObject(image);
    myWriter->setFile(PATH);
    myWriter->write();

    // load Image
    data::Image::sptr image2            = data::Image::New();
    io::itk::ImageReader::sptr myReader = io::itk::ImageReader::New();
    myReader->setObject(image2);
    myReader->setFile(PATH);
    myReader->read();

    io::itk::ut::helper::roundSpacing(image2);

    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    // check Image
    // inr only support float spacing and float origin => add tolerance for comparison (+/-0.00001)
    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::io::itk
