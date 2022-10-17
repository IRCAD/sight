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

#include "IoItkTest.hpp"

#include <core/thread/Worker.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/System.hpp>

#include <data/ImageSeries.hpp>
#include <data/SeriesSet.hpp>

#include <io/base/service/ioTypes.hpp>

#include <service/op/Add.hpp>
#include <service/registry/ObjectService.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::itk::ut::IoItkTest);

namespace sight::module::io::itk::ut
{

static const double EPSILON = 0.00001;

//------------------------------------------------------------------------------

void IoItkTest::setUp()
{
}

//------------------------------------------------------------------------------

void IoItkTest::tearDown()
{
}

//------------------------------------------------------------------------------

void executeService(
    const SPTR(data::Object)& obj,
    const std::string& srvImpl,
    const boost::property_tree::ptree& cfg,
    const data::Access access = data::Access::inout
)
{
    service::IService::sptr srv = service::add(srvImpl);
    CPPUNIT_ASSERT(srv);

    if(access == data::Access::inout)
    {
        srv->setInOut(obj, sight::io::base::service::s_DATA_KEY);
    }
    else
    {
        srv->setInput(obj, sight::io::base::service::s_DATA_KEY);
    }

    srv->setConfiguration(cfg);
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    service::OSR::unregisterService(srv);
}

//------------------------------------------------------------------------------

void IoItkTest::testImageSeriesWriterJPG()
{
    // Create image series
    auto imageSeries = data::ImageSeries::New();
    utestData::generator::Image::generateRandomImage(imageSeries, core::Type::INT16);

    // Create path
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "imageSeriesJPG";
    std::filesystem::create_directories(path);

    // Create Config

    service::IService::ConfigType srvCfg;
    srvCfg.add("folder", path.string());

    // Create and execute service
    executeService(
        imageSeries,
        "sight::module::io::itk::SSlicedImageSeriesWriter",
        srvCfg,
        data::Access::in
    );
}

//------------------------------------------------------------------------------

double tolerance(double num)
{
    return std::floor(num * 100. + .5) / 100.;
}

//------------------------------------------------------------------------------

void IoItkTest::testSaveLoadInr()
{
    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::Type::INT16);

    // inr only support image origin (0,0,0)
    const data::Image::Origin origin = {0., 0., 0.};
    image->setOrigin(origin);

    // save image in inr
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "imageInrTest/image.inr.gz";
    std::filesystem::create_directories(path.parent_path());

    // Create Config
    service::IService::ConfigType srvCfg;
    srvCfg.add("file", path.string());

    // Create and execute service
    executeService(
        image,
        "sight::module::io::itk::SImageWriter",
        srvCfg,
        data::Access::in
    );

    // load Image
    data::Image::sptr image2 = data::Image::New();
    executeService(
        image2,
        "sight::module::io::itk::SImageReader",
        srvCfg,
        data::Access::inout
    );

    data::Image::Spacing spacing = image2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing(spacing);

    // check Image
    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void IoItkTest::testSaveLoadNifti()
{
    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::Type::INT16);

    const data::Image::Origin origin = {0.5F, 0.2F, 1.2F};
    image->setOrigin(origin);

    // save image in inr
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "imageNiftiTest/image.nii";
    std::filesystem::create_directories(path.parent_path());

    // Create Config
    service::IService::ConfigType srvCfg;
    srvCfg.add("file", path.string());

    // Create and execute service
    executeService(
        image,
        "sight::module::io::itk::SImageWriter",
        srvCfg,
        data::Access::in
    );

    // load Image
    data::Image::sptr image2 = data::Image::New();
    executeService(
        image2,
        "sight::module::io::itk::SImageReader",
        srvCfg,
        data::Access::inout
    );

    data::Image::Spacing spacing = image2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing(spacing);

    // check Image
    image2->setWindowCenter(image->getWindowCenter());
    image2->setWindowWidth(image->getWindowWidth());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void IoItkTest::ImageSeriesInrTest()
{
    auto imageSeries = data::ImageSeries::New();
    utestData::generator::Image::generateRandomImage(imageSeries, core::Type::INT16);

    // inr only support image origin (0,0,0)
    const data::Image::Origin origin = {0., 0., 0.};
    imageSeries->setOrigin(origin);

    // save image in inr
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "imageInrTest/imageseries.inr.gz";
    std::filesystem::create_directories(path.parent_path());

    // Create Config
    service::IService::ConfigType srvCfg;
    srvCfg.add("file", path.string());

    // Create and execute service
    executeService(
        imageSeries,
        "sight::module::io::itk::SImageSeriesWriter",
        srvCfg,
        data::Access::in
    );

    // load Image
    auto imageSeries2 = data::ImageSeries::New();
    executeService(
        imageSeries2,
        "sight::module::io::itk::SImageSeriesReader",
        srvCfg,
        data::Access::inout
    );

    data::Image::Spacing spacing = imageSeries2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    imageSeries2->setSpacing(spacing);

    // check Image
    imageSeries2->setWindowCenter(imageSeries->getWindowCenter());
    imageSeries2->setWindowWidth(imageSeries->getWindowWidth());

    // ITK reader change the description of the image, the modality is set to "OT", etc ...
    // We only compare "Image" part...
    const auto image  = data::Image::dynamicCast(imageSeries);
    const auto image2 = data::Image::dynamicCast(imageSeries2);
    image2->setDescription(image->getDescription());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void IoItkTest::ImageSeriesNiftiTest()
{
    auto imageSeries = data::ImageSeries::New();
    utestData::generator::Image::generateRandomImage(imageSeries, core::Type::INT16);

    // inr only support image origin (0,0,0)
    const data::Image::Origin origin = {0., 0., 0.};
    imageSeries->setOrigin(origin);

    // save image in inr
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "imageNiftiTest/imageseries.nii";
    std::filesystem::create_directories(path.parent_path());

    // Create Config
    service::IService::ConfigType srvCfg;
    srvCfg.add("file", path.string());

    // Create and execute service
    executeService(
        imageSeries,
        "sight::module::io::itk::SImageSeriesWriter",
        srvCfg,
        data::Access::in
    );

    // load Image
    data::ImageSeries::sptr imageSeries2 = data::ImageSeries::New();
    executeService(
        imageSeries2,
        "sight::module::io::itk::SImageSeriesReader",
        srvCfg,
        data::Access::inout
    );

    data::Image::Spacing spacing = imageSeries2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    imageSeries2->setSpacing(spacing);

    // check Image
    imageSeries2->setWindowCenter(imageSeries->getWindowCenter());
    imageSeries2->setWindowWidth(imageSeries->getWindowWidth());

    // ITK reader change the description of the image, the modality is set to "OT", etc ...
    // We only compare "Image" part...
    const auto image  = data::Image::dynamicCast(imageSeries);
    const auto image2 = data::Image::dynamicCast(imageSeries2);
    image2->setDescription(image->getDescription());

    CPPUNIT_ASSERT(*image == *image2);
}

//------------------------------------------------------------------------------

void IoItkTest::SeriesSetInrTest()
{
    /*
     * - image.inr.gz : CT, type int16, size: 512x512x134, spacing 0.781:0.781:1.6
     * - skin.inr.gz : mask skin, type uint8, size: 512x512x134, spacing 0.781:0.781:1.6
     */
    const std::filesystem::path imageFile = utestData::Data::dir() / "sight/image/inr/image.inr.gz";
    const std::filesystem::path skinFile  = utestData::Data::dir() / "sight/image/inr/skin.inr.gz";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imageFile.string() + "' does not exist",
        std::filesystem::exists(imageFile)
    );

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + skinFile.string() + "' does not exist",
        std::filesystem::exists(skinFile)
    );

    // Create Config
    service::IService::ConfigType srvCfg;
    srvCfg.add("file", imageFile.string());
    srvCfg.add("file", skinFile.string());

    // load SeriesSet
    auto series_set = data::SeriesSet::New();
    executeService(
        series_set,
        "sight::module::io::itk::SSeriesSetReader",
        srvCfg,
        data::Access::inout
    );

    const data::Image::Spacing spacing = {0.781, 0.781, 1.6};
    const data::Image::Size size       = {512, 512, 134};

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());
    data::ImageSeries::sptr imgSeries = data::ImageSeries::dynamicCast(series_set->at(0));
    CPPUNIT_ASSERT(imgSeries);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), imgSeries->getModality());

    CPPUNIT_ASSERT_EQUAL(std::string("int16"), imgSeries->getType().name());
    CPPUNIT_ASSERT(size == imgSeries->getSize());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], imgSeries->getSpacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], imgSeries->getSpacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], imgSeries->getSpacing()[2], EPSILON);

    imgSeries = data::ImageSeries::dynamicCast(series_set->at(1));
    CPPUNIT_ASSERT(imgSeries);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), imgSeries->getModality());

    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), imgSeries->getType().name());
    CPPUNIT_ASSERT(size == imgSeries->getSize());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], imgSeries->getSpacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], imgSeries->getSpacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], imgSeries->getSpacing()[2], EPSILON);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk::ut
