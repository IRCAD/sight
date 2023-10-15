/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/os/temp_path.hpp>
#include <core/tools/failed.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <service/op.hpp>

#include <utest_data/Data.hpp>
#include <utest_data/generator/image.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <filesystem>
#include <fstream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::vtk::ut::ImageReaderWriterTest);

static const double epsilon = 0.00001;

namespace sight::module::io::vtk::ut
{

//------------------------------------------------------------------------------

void runImageSrv(
    const std::string& srvname,
    const boost::property_tree::ptree& cfg,
    const SPTR(data::object)& image
)
{
    service::base::sptr srv = service::add(srvname);

    CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

    if(srv->is_a("sight::io::service::reader"))
    {
        srv->set_inout(image, "data");
    }
    else
    {
        srv->set_input(image, "data");
    }

    CPPUNIT_ASSERT_NO_THROW(srv->set_config(cfg));
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

boost::property_tree::ptree getIOConfiguration(const std::filesystem::path& file)
{
    service::config_t readerSrvCfg;
    readerSrvCfg.add("file", file.string());

    return readerSrvCfg;
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtkImageReader()
{
    const std::filesystem::path file = utest_data::Data::dir() / "sight/image/vtk/img.vtk";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + file.string() + "' does not exist",
        std::filesystem::exists(file)
    );

    data::image::sptr image = std::make_shared<data::image>();

    // Data expected
    data::image::Spacing spacingExpected;
    spacingExpected[0] = 1.732;
    spacingExpected[1] = 1.732;
    spacingExpected[2] = 3.2;

    data::image::Origin originExpected;
    originExpected[0] = 34.64;
    originExpected[1] = 86.6;
    originExpected[2] = 56;

    data::image::Size sizeExpected;
    sizeExpected[0] = 230;
    sizeExpected[1] = 170;
    sizeExpected[2] = 58;

    runImageSrv("sight::module::io::vtk::image_reader", getIOConfiguration(file), image);

    // Data read.
    data::image::Spacing spacingRead = image->getSpacing();
    data::image::Origin originRead   = image->getOrigin();
    data::image::Size sizeRead       = image->size();

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
    const std::filesystem::path file = utest_data::Data::dir() / "sight/image/vti/BostonTeapot.vti";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + file.string() + "' does not exist",
        std::filesystem::exists(file)
    );

    data::image::sptr image = std::make_shared<data::image>();
    runImageSrv("sight::module::io::vtk::image_reader", getIOConfiguration(file), image);

    // Data expected
    data::image::Spacing spacingExpected;
    spacingExpected[0] = 1.0;
    spacingExpected[1] = 1.0;
    spacingExpected[2] = 1.0;

    data::image::Origin originExpected;
    originExpected[0] = 1.1;
    originExpected[1] = 2.2;
    originExpected[2] = 3.3;

    data::image::Size sizeExpected;
    sizeExpected[0] = 256;
    sizeExpected[1] = 256;
    sizeExpected[2] = 178;

    core::type expectedType("int8"); // MHD File image type : MET_CHAR

    // Data read.
    data::image::Spacing spacingRead = image->getSpacing();
    data::image::Origin originRead   = image->getOrigin();
    data::image::Size sizeRead       = image->size();

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
    const std::filesystem::path file = utest_data::Data::dir() / "sight/image/mhd/BostonTeapot.mhd";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + file.string() + "' does not exist",
        std::filesystem::exists(file)
    );

    data::image::sptr image = std::make_shared<data::image>();
    runImageSrv("sight::module::io::vtk::image_reader", getIOConfiguration(file), image);

    // Data expected
    data::image::Spacing spacingExpected;
    spacingExpected[0] = 1.0;
    spacingExpected[1] = 1.0;
    spacingExpected[2] = 1.0;

    data::image::Origin originExpected;
    originExpected[0] = 1.1;
    originExpected[1] = 2.2;
    originExpected[2] = 3.3;

    data::image::Size sizeExpected;
    sizeExpected[0] = 256;
    sizeExpected[1] = 256;
    sizeExpected[2] = 178;

    core::type expectedType("int8"); // MHD File image type : MET_CHAR

    // Data read.
    data::image::Spacing spacingRead = image->getSpacing();
    data::image::Origin originRead   = image->getOrigin();
    data::image::Size sizeRead       = image->size();

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
    core::os::temp_file tmpFile;

    std::ofstream o_file(tmpFile, std::ios::out | std::ios::trunc | std::ios::binary);
    o_file.close();

    data::image::sptr image = std::make_shared<data::image>();

    {
        const std::string srvname("sight::module::io::vtk::image_reader");

        service::base::sptr srv = service::add(srvname);

        CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

        srv->set_inout(image, "data");

        CPPUNIT_ASSERT_NO_THROW(srv->set_config(getIOConfiguration(tmpFile)));
        CPPUNIT_ASSERT_NO_THROW(srv->configure());
        CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
        CPPUNIT_ASSERT_THROW(srv->update().get(), core::tools::failed);
        CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
        service::remove(srv);
    }
}

//------------------------------------------------------------------------------
void ImageReaderWriterTest::testVtkImageWriter()
{
    // Data to write
    core::type type                      = core::type::UINT8;
    const data::image::Size sizeExpected = {10, 20, 30
    };
    const data::image::Spacing spacingExpected = {0.24, 1.07, 2.21
    };
    const data::image::Origin originExpected = {-5.6, 15.16, 11.11
    };

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateImage(
        image,
        sizeExpected,
        spacingExpected,
        originExpected,
        type,
        data::image::RGBA,
        0
    );

    // Write to vtk image.
    core::os::temp_dir tmpDir;
    const auto file = tmpDir / "tempFile.vtk";

    runImageSrv("sight::module::io::vtk::image_writer", getIOConfiguration(file), image);

    // Read image from disk
    data::image::sptr imageFromDisk = std::make_shared<data::image>();
    runImageSrv("sight::module::io::vtk::image_reader", getIOConfiguration(file), imageFromDisk);

    // Data read
    data::image::Spacing spacingRead = image->getSpacing();
    data::image::Origin originRead   = image->getOrigin();
    data::image::Size sizeRead       = image->size();

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

    const auto imageDumpLock         = image->dump_lock();
    const auto imageFromDiskDumpLock = imageFromDisk->dump_lock();

    const char* const ptrOnGeneratedImage = static_cast<char*>(image->buffer());
    const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->buffer());

    CPPUNIT_ASSERT_EQUAL(image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT(std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage));
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtkImageSeriesWriter()
{
    core::type type  = core::type::FLOAT;
    auto imageSeries = std::make_shared<data::image_series>();
    utest_data::generator::image::generateRandomImage(imageSeries, type);

    core::os::temp_dir tmpDir;
    const auto file = tmpDir / "imageSeries.vtk";

    // Write image series
    runImageSrv("sight::module::io::vtk::image_series_writer", getIOConfiguration(file), imageSeries);

    // Read image series
    auto imageSeries2 = std::make_shared<data::image_series>();
    runImageSrv("sight::module::io::vtk::image_reader", getIOConfiguration(file), imageSeries2);

    imageSeries2->setWindowCenter(imageSeries->getWindowCenter());
    imageSeries2->setWindowWidth(imageSeries->getWindowWidth());

    CPPUNIT_ASSERT(*imageSeries == *imageSeries2);
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testVtiImageWriter()
{
    // Data to write
    core::type type                      = core::type::UINT8;
    const data::image::Size sizeExpected = {10, 20, 30
    };
    const data::image::Spacing spacingExpected = {0.24, 1.07, 2.21
    };
    const data::image::Origin originExpected = {-5.6, 15.16, 11.11
    };

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateImage(
        image,
        sizeExpected,
        spacingExpected,
        originExpected,
        type,
        data::image::GRAY_SCALE,
        0
    );

    // Write to vtk image.
    core::os::temp_dir tmpDir;
    const auto file = tmpDir / "tempFile.vti";

    runImageSrv("sight::module::io::vtk::image_writer", getIOConfiguration(file), image);

    // Read image from disk
    data::image::sptr imageFromDisk = std::make_shared<data::image>();
    runImageSrv("sight::module::io::vtk::image_reader", getIOConfiguration(file), imageFromDisk);

    // Data read
    data::image::Spacing spacingRead = image->getSpacing();
    data::image::Origin originRead   = image->getOrigin();
    data::image::Size sizeRead       = image->size();

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

    const auto imageDumpLock         = image->dump_lock();
    const auto imageFromDiskDumpLock = imageFromDisk->dump_lock();

    const char* const ptrOnGeneratedImage = static_cast<char*>(image->buffer());
    const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->buffer());

    CPPUNIT_ASSERT_EQUAL(image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT(std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage));
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testMhdImageWriter()
{
    // Data to write
    core::type type                      = core::type::UINT8;
    const data::image::Size sizeExpected = {10, 20, 30
    };
    const data::image::Spacing spacingExpected = {0.24, 1.07, 2.21
    };
    const data::image::Origin originExpected = {-5.6, 15.16, 11.11
    };

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateImage(
        image,
        sizeExpected,
        spacingExpected,
        originExpected,
        type,
        data::image::RGB,
        0
    );

    // Write to vtk image.
    core::os::temp_dir tmpDir;
    const auto file = tmpDir / "tempFile.mhd";

    runImageSrv("sight::module::io::vtk::image_writer", getIOConfiguration(file), image);

    // Read image from disk
    data::image::sptr imageFromDisk = std::make_shared<data::image>();
    runImageSrv("sight::module::io::vtk::image_reader", getIOConfiguration(file), imageFromDisk);

    // Data read
    data::image::Spacing spacingRead = image->getSpacing();
    data::image::Origin originRead   = image->getOrigin();
    data::image::Size sizeRead       = image->size();

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

    const auto imageDumpLock         = image->dump_lock();
    const auto imageFromDiskDumpLock = imageFromDisk->dump_lock();

    const char* const ptrOnGeneratedImage = static_cast<char*>(image->buffer());
    const char* const ptrOnReadImage      = static_cast<char*>(imageFromDisk->buffer());

    CPPUNIT_ASSERT_EQUAL(image->getType(), imageFromDisk->getType());
    CPPUNIT_ASSERT(std::equal(ptrOnGeneratedImage, ptrOnGeneratedImage + image->getSizeInBytes(), ptrOnReadImage));
}

//------------------------------------------------------------------------------

void ImageReaderWriterTest::testImageWriterExtension()
{
    // Data to write
    const auto type = core::type::UINT8;
    const data::image::Size sizeExpected {10, 20, 30};
    const data::image::Spacing spacingExpected {0.24, 1.07, 2.21};
    const data::image::Origin originExpected {-5.6, 15.16, 11.11};

    data::image::sptr image = std::make_shared<data::image>();
    utest_data::generator::image::generateImage(
        image,
        sizeExpected,
        spacingExpected,
        originExpected,
        type,
        data::image::GRAY_SCALE,
        0
    );

    // Write to vtk image.
    core::os::temp_dir tmpDir;
    const auto file = tmpDir / "tempFile.xxx";

    {
        const std::string srvname("sight::module::io::vtk::image_writer");

        service::base::sptr srv = service::add(srvname);

        CPPUNIT_ASSERT_MESSAGE(std::string("Failed to create service ") + srvname, srv);

        srv->set_input(image, "data");
        CPPUNIT_ASSERT_NO_THROW(srv->set_config(getIOConfiguration(file)));
        CPPUNIT_ASSERT_NO_THROW(srv->configure());
        CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
        CPPUNIT_ASSERT_THROW(srv->update().get(), core::tools::failed);
        CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
        service::remove(srv);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk::ut
