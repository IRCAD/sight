/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/runtime/EConfigurationElement.hpp>
#include <core/thread/Worker.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/System.hpp>

#include <data/ImageSeries.hpp>
#include <data/SeriesDB.hpp>

#include <services/op/Add.hpp>
#include <services/registry/ActiveWorkers.hpp>
#include <services/registry/ObjectService.hpp>

#include <utestData/Data.hpp>
#include <utestData/generator/Image.hpp>
#include <utestData/helper/compare.hpp>

#include <io/base/services/ioTypes.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::modules::io::itk::ut::IoItkTest );

namespace sight::modules::io::itk
{
namespace ut
{

static const double EPSILON = 0.00001;

//------------------------------------------------------------------------------

void IoItkTest::setUp()
{
    // Set up context before running a test.
    core::thread::Worker::sptr worker = core::thread::Worker::New();
    services::registry::ActiveWorkers::setDefaultWorker(worker);
}

//------------------------------------------------------------------------------

void IoItkTest::tearDown()
{
    // Clean up after the test run.
    services::registry::ActiveWorkers::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

void executeService(
    const SPTR(data::Object)& obj,
    const std::string& srvImpl,
    const SPTR(core::runtime::EConfigurationElement)& cfg,
    const services::IService::AccessType access = services::IService::AccessType::INOUT)
{
    services::IService::sptr srv = services::add(srvImpl);

    CPPUNIT_ASSERT(srv);
    services::OSR::registerService(obj, sight::io::base::services::s_DATA_KEY, access, srv );
    srv->setConfiguration(cfg);
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    services::OSR::unregisterService( srv );
}

//------------------------------------------------------------------------------

void IoItkTest::testImageSeriesWriterJPG()
{
    // Create image series
    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::tools::Type::create("int16"));

    data::ImageSeries::sptr imageSeries = data::ImageSeries::New();
    imageSeries->setImage(image);

    // Create path
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "imageSeriesJPG";
    std::filesystem::create_directories(path);

    // Create Config
    core::runtime::EConfigurationElement::sptr srvCfg    = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr folderCfg = core::runtime::EConfigurationElement::New("folder");
    folderCfg->setValue(path.string());
    srvCfg->addConfigurationElement(folderCfg);

    // Create and execute service
    executeService(imageSeries,
                   "::sight::modules::io::itk::SJpgImageSeriesWriter",
                   srvCfg,
                   services::IService::AccessType::INPUT);
}

//------------------------------------------------------------------------------

void IoItkTest::testImageWriterJPG()
{
    // Create Image
    data::Image::sptr image = data::Image::New();
    utestData::generator::Image::generateRandomImage(image, core::tools::Type::create("int16"));

    // Create path
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "imageJPG";
    std::filesystem::create_directories( path );

    // Create Config
    core::runtime::EConfigurationElement::sptr srvCfg    = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr folderCfg = core::runtime::EConfigurationElement::New("folder");
    folderCfg->setValue(path.string());
    srvCfg->addConfigurationElement(folderCfg);

    // Create and execute service
    executeService(
        image,
        "::sight::modules::io::itk::JpgImageWriterService",
        srvCfg,
        services::IService::AccessType::INPUT);
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
    utestData::generator::Image::generateRandomImage(image, core::tools::Type::s_INT16);

    // inr only support image origin (0,0,0)
    const data::Image::Origin origin = {0., 0., 0.};
    image->setOrigin2(origin);

    // save image in inr
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "imageInrTest/image.inr.gz";
    std::filesystem::create_directories( path.parent_path() );

    // Create Config
    core::runtime::EConfigurationElement::sptr srvCfg  = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr fileCfg = core::runtime::EConfigurationElement::New("file");
    fileCfg->setValue(path.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    executeService(
        image,
        "::sight::modules::io::itk::InrImageWriterService",
        srvCfg,
        services::IService::AccessType::INPUT);

    // load Image
    data::Image::sptr image2 = data::Image::New();
    executeService(
        image2,
        "::sight::modules::io::itk::InrImageReaderService",
        srvCfg,
        services::IService::AccessType::INOUT);

    data::Image::Spacing spacing = image2->getSpacing2();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing2(spacing);

    // check Image
    utestData::helper::ExcludeSetType exclude;
    exclude.insert("window_center");
    exclude.insert("window_width");

    CPPUNIT_ASSERT(utestData::helper::compare(image, image2, exclude));
}

//------------------------------------------------------------------------------

void IoItkTest::ImageSeriesInrTest()
{
    data::Image::sptr image             = data::Image::New();
    data::ImageSeries::sptr imageSeries = data::ImageSeries::New();
    utestData::generator::Image::generateRandomImage(image, core::tools::Type::create("int16"));

    imageSeries->setImage(image);

    // inr only support image origin (0,0,0)
    const data::Image::Origin origin = {0., 0., 0.};
    image->setOrigin2(origin);

    // save image in inr
    const std::filesystem::path path = core::tools::System::getTemporaryFolder() / "imageInrTest/imageseries.inr.gz";
    std::filesystem::create_directories( path.parent_path() );

    // Create Config
    core::runtime::EConfigurationElement::sptr srvCfg  = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr fileCfg = core::runtime::EConfigurationElement::New("file");
    fileCfg->setValue(path.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    executeService(
        imageSeries,
        "::sight::modules::io::itk::SImageSeriesWriter",
        srvCfg,
        services::IService::AccessType::INPUT);

    // load Image
    data::Image::sptr image2 = data::Image::New();
    executeService(
        image2,
        "::sight::modules::io::itk::InrImageReaderService",
        srvCfg,
        services::IService::AccessType::INOUT);

    data::Image::Spacing spacing = image2->getSpacing2();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing2(spacing);

    // check Image
    utestData::helper::ExcludeSetType exclude;
    exclude.insert("window_center");
    exclude.insert("window_width");

    CPPUNIT_ASSERT(utestData::helper::compare(image, image2, exclude));
}

//------------------------------------------------------------------------------

void IoItkTest::SeriesDBInrTest()
{
    /*
     * - image.inr.gz : CT, type int16, size: 512x512x134, spacing 0.781:0.781:1.6
     * - skin.inr.gz : mask skin, type uint8, size: 512x512x134, spacing 0.781:0.781:1.6
     */
    const std::filesystem::path imageFile = utestData::Data::dir() / "sight/image/inr/image.inr.gz";
    const std::filesystem::path skinFile  = utestData::Data::dir() / "sight/image/inr/skin.inr.gz";

    CPPUNIT_ASSERT_MESSAGE("The file '" + imageFile.string() + "' does not exist",
                           std::filesystem::exists(imageFile));

    CPPUNIT_ASSERT_MESSAGE("The file '" + skinFile.string() + "' does not exist",
                           std::filesystem::exists(skinFile));

    // Create Config
    core::runtime::EConfigurationElement::sptr srvCfg       = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr fileImageCfg = core::runtime::EConfigurationElement::New("file");
    fileImageCfg->setValue(imageFile.string());
    srvCfg->addConfigurationElement(fileImageCfg);

    core::runtime::EConfigurationElement::sptr fileSkinCfg = core::runtime::EConfigurationElement::New("file");
    fileSkinCfg->setValue(skinFile.string());
    srvCfg->addConfigurationElement(fileSkinCfg);

    // load SeriesDB
    data::SeriesDB::sptr sdb = data::SeriesDB::New();
    executeService(
        sdb,
        "::sight::modules::io::itk::SInrSeriesDBReader",
        srvCfg,
        services::IService::AccessType::INOUT);

    const data::Image::Spacing spacing = {0.781, 0.781, 1.6};
    const data::Image::Size size       = {512, 512, 134};

    CPPUNIT_ASSERT_EQUAL(size_t(2), sdb->getContainer().size());
    data::ImageSeries::sptr imgSeries = data::ImageSeries::dynamicCast(sdb->getContainer()[0]);
    CPPUNIT_ASSERT(imgSeries);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), imgSeries->getModality());

    data::Image::sptr image = imgSeries->getImage();
    CPPUNIT_ASSERT(image);
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), image->getType().string());
    CPPUNIT_ASSERT(size == image->getSize2());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], image->getSpacing2()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], image->getSpacing2()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], image->getSpacing2()[2], EPSILON);

    imgSeries = data::ImageSeries::dynamicCast(sdb->getContainer()[1]);
    CPPUNIT_ASSERT(imgSeries);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), imgSeries->getModality());
    CPPUNIT_ASSERT(imgSeries->getImage());

    image = imgSeries->getImage();
    CPPUNIT_ASSERT(image);
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), image->getType().string());
    CPPUNIT_ASSERT(size == image->getSize2());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], image->getSpacing2()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], image->getSpacing2()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], image->getSpacing2()[2], EPSILON);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::modules::io::itk
