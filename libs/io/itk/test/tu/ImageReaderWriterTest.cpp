/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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
#include <core/os/temp_path.hpp>

#include <io/itk/InrImageReader.hpp>
#include <io/itk/InrImageWriter.hpp>
#include <io/itk/JpgImageWriter.hpp>
#include <io/itk/NiftiImageReader.hpp>
#include <io/itk/NiftiImageWriter.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::itk::ut::ImageReaderWriterTest);

namespace sight::io::itk::ut
{

static const double epsilon = 0.00001;

//------------------------------------------------------------------------------

void ImageReaderWriterTest::setUp()
{
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::tearDown()
{
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::inrReadWriteTest()
{
    // create Image
    data::Image::sptr image = std::make_shared<data::Image>();
    utestData::generator::Image::generateRandomImage(image, core::type::INT16);
    sight::io::itk::ut::ImageReaderWriterTest::inrReadWriteCheck(image);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::inrStressTest()
{
    core::type type = core::type::UINT8;
    sight::io::itk::ut::ImageReaderWriterTest::inrStressTestWithType(type, 5);

    type = core::type::INT16;
    sight::io::itk::ut::ImageReaderWriterTest::inrStressTestWithType(type, 5);

    type = core::type::UINT16;
    sight::io::itk::ut::ImageReaderWriterTest::inrStressTestWithType(type, 5);

    type = core::type::INT32;
    sight::io::itk::ut::ImageReaderWriterTest::inrStressTestWithType(type, 5);

    type = core::type::UINT32;
    sight::io::itk::ut::ImageReaderWriterTest::inrStressTestWithType(type, 5);

    type = core::type::FLOAT;
    sight::io::itk::ut::ImageReaderWriterTest::inrStressTestWithType(type, 5);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::niftiReadTest()
{
    //cspell: ignore 3Dkidney
    const std::filesystem::path sightImagePath(utestData::Data::dir() / "sight/image/nii/3Dkidney.nii");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + sightImagePath.string() + "' does not exist",
        std::filesystem::exists(sightImagePath)
    );

    // load Image
    data::Image::sptr sightImage                     = std::make_shared<data::Image>();
    io::itk::NiftiImageReader::sptr sightImageReader = std::make_shared<io::itk::NiftiImageReader>();
    sightImageReader->setObject(sightImage);
    sightImageReader->set_file(sightImagePath);
    sightImageReader->read();

    niftiReadCheck(
        sightImage,
        core::type::UINT8,
        3,
        {0.744924, 0.744924, 0.744924},
        {0, 0, 0},
        {481, 362, 478
        });

    const std::filesystem::path externalImagePath(utestData::Data::dir() / "sight/image/nii/brain.nii");

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + externalImagePath.string() + "' does not exist",
        std::filesystem::exists(externalImagePath)
    );

    // load Image
    data::Image::sptr externalImage                     = std::make_shared<data::Image>();
    io::itk::NiftiImageReader::sptr externalImageReader = std::make_shared<io::itk::NiftiImageReader>();
    externalImageReader->setObject(externalImage);
    externalImageReader->set_file(externalImagePath);
    externalImageReader->read();

    niftiReadCheck(
        externalImage,
        core::type::UINT8,
        3,
        {0.5, 0.5, 0.5},
        {75, 107, -69.5},
        {301, 370, 316
        });
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::niftiReadCheck(
    const data::Image::sptr& imageToTest,
    const core::type& expectedType,
    const std::size_t expectedDim,
    const data::Image::Spacing& expectedSpacing,
    const data::Image::Origin& expectedOrigin,
    const data::Image::Size& expectedSize
)
{
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "test on sight/image/nii/3Dkidney.nii failed ",
        imageToTest->getType(),
        expectedType
    );

    CPPUNIT_ASSERT_EQUAL(
        static_cast<std::size_t>(imageToTest->numDimensions()),
        expectedDim
    );

    for(std::size_t i = 0 ; i < expectedDim ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::Image::Spacing::value_type>(imageToTest->getSpacing()[i]),
            static_cast<data::Image::Spacing::value_type>(expectedSpacing[i]),
            epsilon
        );
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
            static_cast<data::Image::Origin::value_type>(imageToTest->getOrigin()[i]),
            static_cast<data::Image::Origin::value_type>(expectedOrigin[i]),
            epsilon
        );
        CPPUNIT_ASSERT_EQUAL(
            static_cast<data::Image::Size::value_type>(imageToTest->size()[i]),
            static_cast<data::Image::Size::value_type>(expectedSize[i])
        );
    }
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::niftiWriteTest()
{
    // create Image
    data::Image::sptr image = std::make_shared<data::Image>();
    utestData::generator::Image::generateRandomImage(image, core::type::INT16);

    // WARNING!
    // There is a conversion subtlety. Nifti stores the data as float, and convert them.
    // The writing/reading generates a conversion double => float => double.
    // This leads to several errors.
    // To prevent this, the spacing and origin used for the test are set as float, converted to double, to be able to
    // test the direct equality.
    const data::Image::Spacing spacingD = {0.5F, 0.001F, 1.25F};
    const data::Image::Origin originD   = {0.5F, 0.25F, 0.25F};
    image->setSpacing(spacingD);
    image->setOrigin(originD);

    // save image in nifti
    core::os::temp_dir tmpDir;
    const std::filesystem::path filename = tmpDir / "image.nii";
    auto myWriter                        = std::make_shared<io::itk::NiftiImageWriter>();

    myWriter->setObject(image);
    myWriter->set_file(filename);
    myWriter->write();

    CPPUNIT_ASSERT_MESSAGE(
        "test on '" + filename.string() + "' failed ",
        std::filesystem::exists(filename)
    );

    // load Image
    data::Image::sptr image2 = std::make_shared<data::Image>();
    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    auto myReader = std::make_shared<io::itk::NiftiImageReader>();
    myReader->setObject(image2);
    myReader->set_file(filename);
    myReader->read();

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::jpegWriteTest()
{
    // create Image
    data::Image::sptr image = std::make_shared<data::Image>();
    utestData::generator::Image::generateRandomImage(image, core::type::INT16);

    // save image in inr
    core::os::temp_dir tmpDir;
    auto myWriter = std::make_shared<io::itk::JpgImageWriter>();
    myWriter->setObject(image);
    myWriter->set_folder(tmpDir);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::inrReadJpegWriteTest()
{
    // create Image
    std::filesystem::path pathInr = utestData::Data::dir() / "sight" / "image" / "inr" / "image.inr.gz";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + pathInr.string() + "' does not exist",
        std::filesystem::exists(pathInr)
    );

    data::Image::sptr image                = std::make_shared<data::Image>();
    io::itk::InrImageReader::sptr myReader = std::make_shared<io::itk::InrImageReader>();
    myReader->setObject(image);
    myReader->set_file(pathInr);
    myReader->read();

    // save image in inr
    core::os::temp_dir tmpDir;
    auto myWriter = std::make_shared<io::itk::JpgImageWriter>();
    myWriter->setObject(image);
    myWriter->set_folder(tmpDir);
    CPPUNIT_ASSERT_NO_THROW(myWriter->write());
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::inrStressTestWithType(core::type type, int nbTest)
{
    for(int nb = 0 ; nb < nbTest ; ++nb)
    {
        data::Image::sptr image = std::make_shared<data::Image>();
        utestData::generator::Image::generateRandomImage(image, type);
        sight::io::itk::ut::ImageReaderWriterTest::inrReadWriteCheck(image);
    }
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::inrReadWriteCheck(data::Image::sptr image)
{
    // inr only support image origin (0,0,0)
    const data::Image::Origin origin = {0., 0., 0.};
    image->setOrigin(origin);

    // save image in inr
    core::os::temp_dir tmpDir;
    const std::filesystem::path PATH = tmpDir / "image.inr.gz";
    auto myWriter                    = std::make_shared<io::itk::InrImageWriter>();
    myWriter->setObject(image);
    myWriter->set_file(PATH);
    myWriter->write();

    // load Image
    auto image2   = std::make_shared<data::Image>();
    auto myReader = std::make_shared<io::itk::InrImageReader>();
    myReader->setObject(image2);
    myReader->set_file(PATH);
    myReader->read();

    io::itk::ut::helper::roundSpacing(image2);

    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    // check Image
    // inr only support float spacing and float origin => add tolerance for comparison (+/-0.00001)
    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

} // namespace sight::io::itk::ut
