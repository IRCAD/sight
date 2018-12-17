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

#include "IoItkTest.hpp"

#include <fwDataTools/Image.hpp>

#include <fwIO/ioTypes.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/generator/Image.hpp>
#include <fwTest/helper/compare.hpp>

#include <fwThread/Worker.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/System.hpp>

#include <boost/assign/list_of.hpp>
#include <boost/filesystem/operations.hpp>

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioITK::ut::IoItkTest );

namespace ioITK
{
namespace ut
{

static const double EPSILON = 0.00001;

//------------------------------------------------------------------------------

void IoItkTest::setUp()
{
    // Set up context before running a test.
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    ::fwServices::registry::ActiveWorkers::setDefaultWorker(worker);
}

//------------------------------------------------------------------------------

void IoItkTest::tearDown()
{
    // Clean up after the test run.
    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
}

//------------------------------------------------------------------------------

void executeService(
    const SPTR(::fwData::Object)& obj,
    const std::string& srvType,
    const std::string& srvImpl,
    const SPTR(::fwRuntime::EConfigurationElement)& cfg,
    const ::fwServices::IService::AccessType access = ::fwServices::IService::AccessType::INOUT)
{
    ::fwServices::IService::sptr srv = ::fwServices::add(srvImpl);

    CPPUNIT_ASSERT(srv);
    ::fwServices::OSR::registerService(obj, ::fwIO::s_DATA_KEY, access, srv );
    srv->setConfiguration(cfg);
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->update().wait());
    CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
    ::fwServices::OSR::unregisterService( srv );
}

//------------------------------------------------------------------------------

void IoItkTest::testImageSeriesWriterJPG()
{
    // Create image series
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::New();
    imageSeries->setImage(image);

    // Create path
    const ::boost::filesystem::path path = "imageSeriesJPG";
    ::boost::filesystem::create_directories(path);

    // Create Config
    ::fwRuntime::EConfigurationElement::sptr srvCfg    = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr folderCfg = ::fwRuntime::EConfigurationElement::New("folder");
    folderCfg->setValue(path.string());
    srvCfg->addConfigurationElement(folderCfg);

    // Create and execute service
    executeService(imageSeries,
                   "::fwIO::IWriter",
                   "::ioITK::SJpgImageSeriesWriter",
                   srvCfg,
                   ::fwServices::IService::AccessType::INPUT);

    // Remove path
    ::boost::filesystem::remove_all( path.string() );
}

//------------------------------------------------------------------------------

void IoItkTest::testImageWriterJPG()
{
    // Create Image
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    // Create path
    const ::boost::filesystem::path path = "imageJPG";
    ::boost::filesystem::create_directories( path );

    // Create Config
    ::fwRuntime::EConfigurationElement::sptr srvCfg    = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr folderCfg = ::fwRuntime::EConfigurationElement::New("folder");
    folderCfg->setValue(path.string());
    srvCfg->addConfigurationElement(folderCfg);

    // Create and execute service
    executeService(
        image,
        "::fwIO::IWriter",
        "::ioITK::JpgImageWriterService",
        srvCfg,
        ::fwServices::IService::AccessType::INPUT);

    // Remove path
    ::boost::filesystem::remove_all( path.string() );
}

//------------------------------------------------------------------------------

double tolerance(double num)
{
    return std::floor(num * 100. + .5) / 100.;
}

//------------------------------------------------------------------------------

void IoItkTest::testSaveLoadInr()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    // inr only support image origin (0,0,0)
    ::fwData::Image::OriginType origin(3, 0);
    image->setOrigin(origin);

    // save image in inr
    const ::boost::filesystem::path PATH = "imageInrTest/image.inr.gz";
    ::boost::filesystem::create_directories( PATH.parent_path() );

    // Create Config
    ::fwRuntime::EConfigurationElement::sptr srvCfg  = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr fileCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileCfg->setValue(PATH.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    executeService(
        image,
        "::fwIO::IWriter",
        "::ioITK::InrImageWriterService",
        srvCfg,
        ::fwServices::IService::AccessType::INPUT);

    // load Image
    ::fwData::Image::sptr image2 = ::fwData::Image::New();
    executeService(
        image2,
        "::fwIO::IReader",
        "::ioITK::InrImageReaderService",
        srvCfg,
        ::fwServices::IService::AccessType::INOUT);

    ::boost::filesystem::remove_all( PATH.parent_path().string() );

    ::fwData::Image::SpacingType spacing = image2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing(spacing);

    // check Image
    ::fwTest::helper::ExcludeSetType exclude;
    exclude.insert("window_center");
    exclude.insert("window_width");

    CPPUNIT_ASSERT(::fwTest::helper::compare(image, image2, exclude));
}

//------------------------------------------------------------------------------

void IoItkTest::ImageSeriesInrTest()
{
    ::fwData::Image::sptr image                = ::fwData::Image::New();
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    imageSeries->setImage(image);

    // inr only support image origin (0,0,0)
    ::fwData::Image::OriginType origin(3, 0);
    image->setOrigin(origin);

    // save image in inr
    const ::boost::filesystem::path PATH = "imageInrTest/imageseries.inr.gz";
    ::boost::filesystem::create_directories( PATH.parent_path() );

    // Create Config
    ::fwRuntime::EConfigurationElement::sptr srvCfg  = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr fileCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileCfg->setValue(PATH.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    executeService(
        imageSeries,
        "::fwIO::IWriter",
        "::ioITK::SImageSeriesWriter",
        srvCfg,
        ::fwServices::IService::AccessType::INPUT);

    // load Image
    ::fwData::Image::sptr image2 = ::fwData::Image::New();
    executeService(
        image2,
        "::fwIO::IReader",
        "::ioITK::InrImageReaderService",
        srvCfg,
        ::fwServices::IService::AccessType::INOUT);

    ::boost::filesystem::remove_all( PATH.parent_path().string() );

    ::fwData::Image::SpacingType spacing = image2->getSpacing();
    std::transform(spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing(spacing);

    // check Image
    ::fwTest::helper::ExcludeSetType exclude;
    exclude.insert("window_center");
    exclude.insert("window_width");

    CPPUNIT_ASSERT(::fwTest::helper::compare(image, image2, exclude));
}

//------------------------------------------------------------------------------

void IoItkTest::SeriesDBInrTest()
{
    /*
     * - image.inr.gz : CT, type int16, size: 512x512x134, spacing 0.781:0.781:1.6
     * - skin.inr.gz : mask skin, type uint8, size: 512x512x134, spacing 0.781:0.781:1.6
     */
    const ::boost::filesystem::path imageFile = ::fwTest::Data::dir() / "sight/image/inr/image.inr.gz";
    const ::boost::filesystem::path skinFile  = ::fwTest::Data::dir() / "sight/image/inr/skin.inr.gz";

    CPPUNIT_ASSERT_MESSAGE("The file '" + imageFile.string() + "' does not exist",
                           ::boost::filesystem::exists(imageFile));

    CPPUNIT_ASSERT_MESSAGE("The file '" + skinFile.string() + "' does not exist",
                           ::boost::filesystem::exists(skinFile));

    // Create Config
    ::fwRuntime::EConfigurationElement::sptr srvCfg       = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr fileImageCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileImageCfg->setValue(imageFile.string());
    srvCfg->addConfigurationElement(fileImageCfg);

    ::fwRuntime::EConfigurationElement::sptr fileSkinCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileSkinCfg->setValue(skinFile.string());
    srvCfg->addConfigurationElement(fileSkinCfg);

    // load SeriesDB
    ::fwMedData::SeriesDB::sptr sdb = ::fwMedData::SeriesDB::New();
    executeService(
        sdb,
        "::fwIO::IReader",
        "::ioITK::SInrSeriesDBReader",
        srvCfg,
        ::fwServices::IService::AccessType::INOUT);

    ::fwData::Image::SpacingType spacing = list_of(0.781)(0.781)(1.6);
    ::fwData::Image::SizeType size       = list_of(512)(512)(134);

    CPPUNIT_ASSERT_EQUAL(size_t(2), sdb->getContainer().size());
    ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::dynamicCast(sdb->getContainer()[0]);
    CPPUNIT_ASSERT(imgSeries);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), imgSeries->getModality());

    ::fwData::Image::sptr image = imgSeries->getImage();
    CPPUNIT_ASSERT(image);
    CPPUNIT_ASSERT_EQUAL(std::string("int16"), image->getType().string());
    CPPUNIT_ASSERT(size == image->getSize());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], image->getSpacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], image->getSpacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], image->getSpacing()[2], EPSILON);

    imgSeries = ::fwMedData::ImageSeries::dynamicCast(sdb->getContainer()[1]);
    CPPUNIT_ASSERT(imgSeries);
    CPPUNIT_ASSERT_EQUAL(std::string("OT"), imgSeries->getModality());
    CPPUNIT_ASSERT(imgSeries->getImage());

    image = imgSeries->getImage();
    CPPUNIT_ASSERT(image);
    CPPUNIT_ASSERT_EQUAL(std::string("uint8"), image->getType().string());
    CPPUNIT_ASSERT(size == image->getSize());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[0], image->getSpacing()[0], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[1], image->getSpacing()[1], EPSILON);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(spacing[2], image->getSpacing()[2], EPSILON);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace ioITK
