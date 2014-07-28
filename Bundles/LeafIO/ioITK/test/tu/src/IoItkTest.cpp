/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/list_of.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/System.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwData/Object.hpp>

#include <fwMedData/SeriesDB.hpp>
#include <fwMedData/ImageSeries.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/generator/Image.hpp>

#include <fwDataTools/Image.hpp>

#include <fwServices/registry/ServiceFactory.hpp>
#include <fwServices/registry/ObjectService.hpp>


#include "IoItkTest.hpp"

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioITK::ut::IoItkTest );


namespace ioITK
{
namespace ut
{

static const double EPSILON = 0.00001;

//-----------------------------------------------------------------------------

void compare(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    BOOST_FOREACH( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop, (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal" , props->size() == 0 );
}

//------------------------------------------------------------------------------

void IoItkTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IoItkTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void IoItkTest::executeService(
        const SPTR(::fwData::Object)& obj,
        const std::string& srvType,
        const std::string& srvImpl,
        const SPTR(::fwRuntime::EConfigurationElement)& cfg )
{
    ::fwServices::IService::sptr srv
        = ::fwServices::registry::ServiceFactory::getDefault()->create(srvType, srvImpl);

    CPPUNIT_ASSERT(srv);
    ::fwServices::OSR::registerService( obj , srv );
    srv->setConfiguration(cfg);
    CPPUNIT_ASSERT_NO_THROW(srv->configure());
    CPPUNIT_ASSERT_NO_THROW(srv->start());
    CPPUNIT_ASSERT_NO_THROW(srv->update());
    CPPUNIT_ASSERT_NO_THROW(srv->stop());
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
    ::fwRuntime::EConfigurationElement::sptr srvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr folderCfg = ::fwRuntime::EConfigurationElement::New("folder");
    folderCfg->setValue(path.string());
    srvCfg->addConfigurationElement(folderCfg);

    // Create and execute service
    this->executeService(imageSeries, "::io::IWriter", "::ioITK::SJpgImageSeriesWriter", srvCfg);

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
    ::fwRuntime::EConfigurationElement::sptr srvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr folderCfg = ::fwRuntime::EConfigurationElement::New("folder");
    folderCfg->setValue(path.string());
    srvCfg->addConfigurationElement(folderCfg);

    // Create and execute service
    this->executeService( image, "::io::IWriter", "::ioITK::JpgImageWriterService", srvCfg );

    // Remove path
    ::boost::filesystem::remove_all( path.string() );
}

//------------------------------------------------------------------------------

double tolerance(double num)
{
    return std::floor(num * 100. + .5) / 100.;
}

void IoItkTest::testSaveLoadInr()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    // inr only support image origin (0,0,0)
    ::fwData::Image::OriginType origin(3,0);
    image->setOrigin(origin);

    // save image in inr
    const ::boost::filesystem::path PATH = "imageInrTest/image.inr.gz";
    ::boost::filesystem::create_directories( PATH.parent_path() );

    // Create Config
    ::fwRuntime::EConfigurationElement::sptr srvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr fileCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileCfg->setValue(PATH.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    this->executeService( image, "::io::IWriter", "::ioITK::InrImageWriterService", srvCfg );

    // load Image
    ::fwData::Image::sptr image2 = ::fwData::Image::New();
    this->executeService( image2, "::io::IReader", "::ioITK::InrImageReaderService", srvCfg );

    ::boost::filesystem::remove_all( PATH.parent_path().string() );

    ::fwData::Image::SpacingType spacing = image2->getSpacing();
    std::transform (spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing(spacing);

    // check Image
    compare(image, image2);
}

//------------------------------------------------------------------------------

void IoItkTest::ImageSeriesInrTest()
{
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::New();
    ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create("int16"));

    imageSeries->setImage(image);

    // inr only support image origin (0,0,0)
    ::fwData::Image::OriginType origin(3,0);
    image->setOrigin(origin);

    // save image in inr
    const ::boost::filesystem::path PATH = "imageInrTest/imageseries.inr.gz";
    ::boost::filesystem::create_directories( PATH.parent_path() );

    // Create Config
    ::fwRuntime::EConfigurationElement::sptr srvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr fileCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileCfg->setValue(PATH.string());
    srvCfg->addConfigurationElement(fileCfg);

    // Create and execute service
    this->executeService( imageSeries, "::io::IWriter", "::ioITK::SImageSeriesWriter", srvCfg );

    // load Image
    ::fwData::Image::sptr image2 = ::fwData::Image::New();
    this->executeService( image2, "::io::IReader", "::ioITK::InrImageReaderService", srvCfg );

    ::boost::filesystem::remove_all( PATH.parent_path().string() );

    ::fwData::Image::SpacingType spacing = image2->getSpacing();
    std::transform (spacing.begin(), spacing.end(), spacing.begin(), tolerance);
    image2->setSpacing(spacing);

    // check Image
    compare(image, image2);
}

//------------------------------------------------------------------------------

void IoItkTest::SeriesDBInrTest()
{
    /*
     * - image.inr.gz : CT, type int16, size: 512x512x134, spacing 0.781:0.781:1.6
     * - skin.inr.gz : mask skin, type uint8, size: 512x512x134, spacing 0.781:0.781:1.6
     */
    const ::boost::filesystem::path imageFile = ::fwTest::Data::dir() / "fw4spl/image/inr/image.inr.gz";
    const ::boost::filesystem::path skinFile = ::fwTest::Data::dir() / "fw4spl/image/inr/skin.inr.gz";
    // Create Config
    ::fwRuntime::EConfigurationElement::sptr srvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr fileImageCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileImageCfg->setValue(imageFile.string());
    srvCfg->addConfigurationElement(fileImageCfg);

    ::fwRuntime::EConfigurationElement::sptr fileSkinCfg = ::fwRuntime::EConfigurationElement::New("file");
    fileSkinCfg->setValue(skinFile.string());
    srvCfg->addConfigurationElement(fileSkinCfg);

    // load SeriesDB
    ::fwMedData::SeriesDB::sptr sdb = ::fwMedData::SeriesDB::New();
    this->executeService( sdb, "::io::IReader", "::ioITK::SInrSeriesDBReader", srvCfg );

    ::fwData::Image::SpacingType spacing = list_of(0.781)(0.781)(1.6);
    ::fwData::Image::SizeType size = list_of(512)(512)(134);

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

