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

#include <core/runtime/EConfigurationElement.hpp>
#include <core/thread/Worker.hpp>
#include <core/tools/System.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>

#include <service/base.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::vtk::ut::ImageReaderWriterTest);

static const double epsilon = 0.00001;

namespace sight::module::io::vtk
{

namespace ut
{

//------------------------------------------------------------------------------

void runImageSrv(
    const std::string& srvname,
    const SPTR(core::runtime::EConfigurationElement)& cfg,
    const SPTR(data::Object)& image
)
{
    service::IService::sptr srv = service::add(srvname);

    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

    if(srv->isA("sight::io::base::service::IReader"))
    {
        srv->setInOut(image, "data");
    }
    else
    {
        srv->setInput(image, "data");
    }

    CPPUNIT_ASSERT_NO_THROW(srv->setConfiguration(cfg));
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    service::remove(srv);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::setUp()
{
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::tearDown()
{
}

//------------------------------------------------------------------------------

core::runtime::EConfigurationElement::sptr getIOConfiguration(const std::filesystem::path& file)
{
    core::runtime::EConfigurationElement::sptr readerSrvCfg = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr readerCfg    = core::runtime::EConfigurationElement::New("file");
    readerCfg->setValue(file.string());
    readerSrvCfg->addConfigurationElement(readerCfg);

    return readerSrvCfg;
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtkImageReader()
{
    const std::filesystem::path file = utestData::Data::dir() / "sight/image/vtk/img.vtk";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + file.string() + "' does not exist",
        std::filesystem::exists(file)
    );

    data::Image::sptr image = data::Image::New();

    // Data expected
    data::Image::Spacing spacingExpected;
    spacingExpected[0] = 1.732;
    spacingExpected[1] = 1.732;
    spacingExpected[2] = 3.2;

    data::Image::Origin originExpected;
    originExpected[0] = 34.64;
    originExpected[1] = 86.6;
    originExpected[2] = 56;

    data::Image::Size sizeExpected;
    sizeExpected[0] = 230;
    sizeExpected[1] = 170;
    sizeExpected[2] = 58;

    runImageSrv("sight::module::io::vtk::SImageReader", getIOConfiguration(file), image);

    // Data read.
    data::Image::Spacing spacingRead = image->getSpacing();
    data::Image::Origin originRead   = image->getOrigin();
    data::Image::Size sizeRead       = image->getSize();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size());
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size());
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtiImageReader()
{
    const std::filesystem::path file = utestData::Data::dir() / "sight/image/vti/BostonTeapot.vti";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + file.string() + "' does not exist",
        std::filesystem::exists(file)
    );

    data::Image::sptr image = data::Image::New();
    runImageSrv("sight::module::io::vtk::SImageReader", getIOConfiguration(file), image);

    // Data expected
    data::Image::Spacing spacingExpected;
    spacingExpected[0] = 1.0;
    spacingExpected[1] = 1.0;
    spacingExpected[2] = 1.0;

    data::Image::Origin originExpected;
    originExpected[0] = 1.1;
    originExpected[1] = 2.2;
    originExpected[2] = 3.3;

    data::Image::Size sizeExpected;
    sizeExpected[0] = 256;
    sizeExpected[1] = 256;
    sizeExpected[2] = 178;

    core::tools::Type expectedType("int8"); // MHD File image type : MET_CHAR

    // Data read.
    data::Image::Spacing spacingRead = image->getSpacing();
    data::Image::Origin originRead   = image->getOrigin();
    data::Image::Size sizeRead       = image->getSize();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size());
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size());
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    CPPUNIT_ASSERT_EQUAL(expectedType, image->getType());
}

//------------------------------------------------------------------------------
void ImageReaderWriterTest::testMhdImageReader()
{
    const std::filesystem::path file = utestData::Data::dir() / "sight/image/mhd/BostonTeapot.mhd";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + file.string() + "' does not exist",
        std::filesystem::exists(file)
    );

    data::Image::sptr image = data::Image::New();
    runImageSrv("sight::module::io::vtk::SImageReader", getIOConfiguration(file), image);

    // Data expected
    data::Image::Spacing spacingExpected;
    spacingExpected[0] = 1.0;
    spacingExpected[1] = 1.0;
    spacingExpected[2] = 1.0;

    data::Image::Origin originExpected;
    originExpected[0] = 1.1;
    originExpected[1] = 2.2;
    originExpected[2] = 3.3;

    data::Image::Size sizeExpected;
    sizeExpected[0] = 256;
    sizeExpected[1] = 256;
    sizeExpected[2] = 178;

    core::tools::Type expectedType("int8"); // MHD File image type : MET_CHAR

    // Data read.
    data::Image::Spacing spacingRead = image->getSpacing();
    data::Image::Origin originRead   = image->getOrigin();
    data::Image::Size sizeRead       = image->getSize();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size());
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size());
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    CPPUNIT_ASSERT_EQUAL(expectedType, image->getType());
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testImageReaderExtension()
{
    const std::filesystem::path file = core::tools::System::getTemporaryFolder() / "img.xxx";

    std::ofstream ofile;
    ofile.open(file.string().c_str());
    ofile.close();

    data::Image::sptr image = data::Image::New();

    {
        const std::string srvname("sight::module::io::vtk::SImageReader");

        service::IService::sptr srv = service::add(srvname);

        CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

        srv->setInOut(image, "data");

        CPPUNIT_ASSERT_NO_THROW(srv->setConfiguration(getIOConfiguration(file)));
        CPPUNIT_ASSERT_NO_THROW(srv->configure());
        CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
        CPPUNIT_ASSERT_THROW(srv->update().get(), core::tools::Failed);
        CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
        service::remove(srv);
    }
    std::filesystem::remove(file);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testBitmapImageWriter()
{
    // Data to write
    const core::tools::Type type         = core::tools::Type::s_UINT8;
    const data::Image::Size sizeExpected = {10, 20, 0};

    // Use standard information for spacing and origin
    // As the data will be lost in the file format
    const data::Image::Spacing spacingExpected = {1., 1., 0
    };
    const data::Image::Origin originExpected = {0., 0., 0.
    };

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateImage(
        image,
        sizeExpected,
        spacingExpected,
        originExpected,
        type,
        data::Image::RGB
    );

    // Test all teh available extensions
    std::vector<std::string> extensions;
    extensions.push_back("bmp");
    extensions.push_back("jpeg");
    extensions.push_back("jpg");
    extensions.push_back("png");
    extensions.push_back("tiff");

    for(const std::string& ext : extensions)
    {
        // Write to bitmap image.
        const std::filesystem::path file = core::tools::System::getTemporaryFolder() / ("temporaryFile." + ext);

        runImageSrv("sight::module::io::vtk::SImageWriter", getIOConfiguration(file), image);

        // Read image from disk
        const data::Image::sptr imageFromDisk = data::Image::New();
        runImageSrv("sight::module::io::vtk::SImageReader", getIOConfiguration(file), imageFromDisk);

        std::filesystem::remove(file);

        // Data read
        data::Image::Spacing spacingRead = image->getSpacing();
        data::Image::Origin originRead   = image->getOrigin();
        data::Image::Size sizeRead       = image->getSize();

        CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size());
        CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size());
        CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size());

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);

        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);

        CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);

        const auto imageDumpLock         = image->lock();
        const auto imageFromDiskDumpLock = imageFromDisk->lock();

        const char* const ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
        const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->getBuffer());

        CPPUNIT_ASSERT_EQUAL(image->getType(), imageFromDisk->getType());
        CPPUNIT_ASSERT(
            std::equal(
                ptrOnGeneratedImage,
                ptrOnGeneratedImage + image->getSizeInBytes(),
                ptrOnReadImage
            )
        );
    }
}

//------------------------------------------------------------------------------
void ImageReaderWriterTest::testVtkImageWriter()
{
    // Data to write
    core::tools::Type type               = core::tools::Type::s_UINT8;
    const data::Image::Size sizeExpected = {10, 20, 30
    };
    const data::Image::Spacing spacingExpected = {0.24, 1.07, 2.21
    };
    const data::Image::Origin originExpected = {-5.6, 15.16, 11.11
    };

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateImage(
        image,
        sizeExpected,
        spacingExpected,
        originExpected,
        type,
        data::Image::RGBA
    );
    utestData::generator::Image::randomizeImage(image);

    // Write to vtk image.
    const std::filesystem::path file = core::tools::System::getTemporaryFolder() / "temporaryFile.vtk";

    runImageSrv("sight::module::io::vtk::SImageWriter", getIOConfiguration(file), image);

    // Read image from disk
    data::Image::sptr imageFromDisk = data::Image::New();
    runImageSrv("sight::module::io::vtk::SImageReader", getIOConfiguration(file), imageFromDisk);

    std::filesystem::remove(file);

    // Data read
    data::Image::Spacing spacingRead = image->getSpacing();
    data::Image::Origin originRead   = image->getOrigin();
    data::Image::Size sizeRead       = image->getSize();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size());
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size());
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    const auto imageDumpLock         = image->lock();
    const auto imageFromDiskDumpLock = imageFromDisk->lock();

    const char* const ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
    const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->getBuffer());

    CPPUNIT_ASSERT_EQUAL(image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT(std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage));
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtkImageSeriesWriter()
{
    core::tools::Type type  = core::tools::Type::s_FLOAT;
    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, type);

    data::ImageSeries::sptr imageSeries = data::ImageSeries::New();
    imageSeries->setImage(image);

    const std::filesystem::path file = core::tools::System::getTemporaryFolder() / "imageSeries.vtk";

    // Write image series
    runImageSrv("sight::module::io::vtk::SImageSeriesWriter", getIOConfiguration(file), imageSeries);

    // Read image series
    data::Image::sptr image2 = data::Image::New();
    runImageSrv("sight::module::io::vtk::SImageReader", getIOConfiguration(file), image2);

    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtiImageWriter()
{
    // Data to write
    core::tools::Type type               = core::tools::Type::s_UINT8;
    const data::Image::Size sizeExpected = {10, 20, 30
    };
    const data::Image::Spacing spacingExpected = {0.24, 1.07, 2.21
    };
    const data::Image::Origin originExpected = {-5.6, 15.16, 11.11
    };

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateImage(
        image,
        sizeExpected,
        spacingExpected,
        originExpected,
        type,
        data::Image::GRAY_SCALE
    );
    utestData::generator::Image::randomizeImage(image);

    // Write to vtk image.
    const std::filesystem::path file = core::tools::System::getTemporaryFolder() / "temporaryFile.vti";

    runImageSrv("sight::module::io::vtk::SImageWriter", getIOConfiguration(file), image);

    // Read image from disk
    data::Image::sptr imageFromDisk = data::Image::New();
    runImageSrv("sight::module::io::vtk::SImageReader", getIOConfiguration(file), imageFromDisk);

    // Data read
    data::Image::Spacing spacingRead = image->getSpacing();
    data::Image::Origin originRead   = image->getOrigin();
    data::Image::Size sizeRead       = image->getSize();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size());
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size());
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    const auto imageDumpLock         = image->lock();
    const auto imageFromDiskDumpLock = imageFromDisk->lock();

    const char* const ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
    const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->getBuffer());

    CPPUNIT_ASSERT_EQUAL(image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT(std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage));
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testMhdImageWriter()
{
    // Data to write
    core::tools::Type type               = core::tools::Type::s_UINT8;
    const data::Image::Size sizeExpected = {10, 20, 30
    };
    const data::Image::Spacing spacingExpected = {0.24, 1.07, 2.21
    };
    const data::Image::Origin originExpected = {-5.6, 15.16, 11.11
    };

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateImage(
        image,
        sizeExpected,
        spacingExpected,
        originExpected,
        type,
        data::Image::RGB
    );
    utestData::generator::Image::randomizeImage(image);

    // Write to vtk image.
    const std::filesystem::path file = core::tools::System::getTemporaryFolder() / "temporaryFile.mhd";

    runImageSrv("sight::module::io::vtk::SImageWriter", getIOConfiguration(file), image);

    // Read image from disk
    data::Image::sptr imageFromDisk = data::Image::New();
    runImageSrv("sight::module::io::vtk::SImageReader", getIOConfiguration(file), imageFromDisk);

    // Data read
    data::Image::Spacing spacingRead = image->getSpacing();
    data::Image::Origin originRead   = image->getOrigin();
    data::Image::Size sizeRead       = image->getSize();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size());
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size());
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size());

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1]);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2]);

    const auto imageDumpLock         = image->lock();
    const auto imageFromDiskDumpLock = imageFromDisk->lock();

    const char* const ptrOnGeneratedImage = static_cast<char*>(image->getBuffer());
    const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->getBuffer());

    CPPUNIT_ASSERT_EQUAL(image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT(std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage));
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testImageWriterExtension()
{
    // Data to write
    core::tools::Type type               = core::tools::Type::s_UINT8;
    const data::Image::Size sizeExpected = {10, 20, 30
    };
    const data::Image::Spacing spacingExpected = {0.24, 1.07, 2.21
    };
    const data::Image::Origin originExpected = {-5.6, 15.16, 11.11
    };

    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateImage(
        image,
        sizeExpected,
        spacingExpected,
        originExpected,
        type,
        data::Image::GRAY_SCALE
    );
    utestData::generator::Image::randomizeImage(image);

    // Write to vtk image.
    const std::filesystem::path file = core::tools::System::getTemporaryFolder() / "temporaryFile.xxx";

    {
        const std::string srvname("sight::module::io::vtk::SImageWriter");

        service::IService::sptr srv = service::add(srvname);

        CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

        srv->setInput(image, "data");
        CPPUNIT_ASSERT_NO_THROW(srv->setConfiguration(getIOConfiguration(file)));
        CPPUNIT_ASSERT_NO_THROW(srv->configure());
        CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
        CPPUNIT_ASSERT_THROW(srv->update().get(), core::tools::Failed);
        CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
        service::remove(srv);
    }
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::module::io::vtk
