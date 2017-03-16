/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "SeriesDBReaderTest.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/ILocation.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwDataTools/helper/Image.hpp>
#include <fwDataTools/Image.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwTest/Data.hpp>

#include <fwTools/System.hpp>

#include <boost/filesystem/operations.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::ioVTK::ut::SeriesDBReaderTest );

static const double epsilon = 0.00001;

namespace ioVTK
{
namespace ut
{

//-----------------------------------------------------------------------------

void compare(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0 );
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::testSeriesDBReader()
{
    const ::boost::filesystem::path imageFile = ::fwTest::Data::dir() / "fw4spl/image/vtk/img.vtk";
    const ::boost::filesystem::path meshFile  = ::fwTest::Data::dir() / "fw4spl/mesh/vtk/sphere.vtk";

    CPPUNIT_ASSERT_MESSAGE("The file '" + imageFile.string() + "' does not exist",
                           ::boost::filesystem::exists(imageFile));

    CPPUNIT_ASSERT_MESSAGE("The file '" + meshFile.string() + "' does not exist",
                           ::boost::filesystem::exists(meshFile));

    ::fwRuntime::EConfigurationElement::sptr readerSrvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr file1Cfg     = ::fwRuntime::EConfigurationElement::New("file");
    ::fwRuntime::EConfigurationElement::sptr file2Cfg     = ::fwRuntime::EConfigurationElement::New("file");
    file1Cfg->setValue(imageFile.string());
    readerSrvCfg->addConfigurationElement(file1Cfg);
    file2Cfg->setValue(meshFile.string());
    readerSrvCfg->addConfigurationElement(file2Cfg);
    readerSrvCfg->addConfigurationElement(file2Cfg);

    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    ::fwServices::IService::sptr srv;
    srv = ::fwServices::registry::ServiceFactory::getDefault()->create("::io::IReader", "::ioVTK::SSeriesDBReader" );

    CPPUNIT_ASSERT_MESSAGE("Create SSeriesDBReader failed", srv);

    ::fwServices::OSR::registerService(seriesDB, srv);
    srv->setConfiguration(readerSrvCfg);
    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    ::fwServices::OSR::unregisterService(srv);

    // Data expected
    const size_t dim = 3;
    ::fwData::Image::SpacingType spacingExpected(dim);
    spacingExpected[0] = 1.732;
    spacingExpected[1] = 1.732;
    spacingExpected[2] = 3.2;

    ::fwData::Image::OriginType originExpected(dim);
    originExpected[0] = 34.64;
    originExpected[1] = 86.6;
    originExpected[2] = 56;

    ::fwData::Image::SizeType sizeExpected(dim);
    sizeExpected[0] = 230;
    sizeExpected[1] = 170;
    sizeExpected[2] = 58;

    CPPUNIT_ASSERT_EQUAL(size_t(2), seriesDB->size());

    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(seriesDB->getContainer()[0]);
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(seriesDB->getContainer()[1]);
    CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", imageSeries);
    CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", modelSeries);

    // Data read.
    ::fwData::Image::sptr image              = imageSeries->getImage();
    ::fwData::Image::SpacingType spacingRead = image->getSpacing();
    ::fwData::Image::SpacingType originRead  = image->getOrigin();
    ::fwData::Image::SizeType sizeRead       = image->getSize();

    CPPUNIT_ASSERT_EQUAL(spacingExpected.size(), spacingRead.size() );
    CPPUNIT_ASSERT_EQUAL(originExpected.size(), originRead.size() );
    CPPUNIT_ASSERT_EQUAL(sizeExpected.size(), sizeRead.size() );

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on x", spacingExpected[0], spacingRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on y", spacingExpected[1], spacingRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect spacing on z", spacingExpected[2], spacingRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on x", originExpected[0], originRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on y", originExpected[1], originRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect origin on z", originExpected[2], originRead[2], epsilon);

    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on x", sizeExpected[0], sizeRead[0], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on y", sizeExpected[1], sizeRead[1], epsilon);
    CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE("Incorrect size on z", sizeExpected[2], sizeRead[2], epsilon);

    CPPUNIT_ASSERT_EQUAL(size_t(2),  modelSeries->getReconstructionDB().size());

    ::fwData::Reconstruction::sptr rec1 = modelSeries->getReconstructionDB()[0];
    ::fwData::Reconstruction::sptr rec2 = modelSeries->getReconstructionDB()[1];
    ::fwData::Mesh::sptr mesh1          = rec1->getMesh();
    ::fwData::Mesh::sptr mesh2          = rec2->getMesh();

    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id)720, mesh1->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL((::fwData::Mesh::Id)362, mesh1->getNumberOfPoints());

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::testMergeSeriesDBReader()
{
    const ::boost::filesystem::path imageFile = ::fwTest::Data::dir() / "fw4spl/image/vtk/img.vtk";

    CPPUNIT_ASSERT_MESSAGE("The file '" + imageFile.string() + "' does not exist",
                           ::boost::filesystem::exists(imageFile));

    ::fwRuntime::EConfigurationElement::sptr readerSrvCfg = ::fwRuntime::EConfigurationElement::New("service");
    ::fwRuntime::EConfigurationElement::sptr fileCfg      = ::fwRuntime::EConfigurationElement::New("file");
    fileCfg->setValue(imageFile.string());
    readerSrvCfg->addConfigurationElement(fileCfg);

    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::New();
    ::fwMedData::SeriesDB::sptr seriesDB       = ::fwMedData::SeriesDB::New();
    seriesDB->getContainer().push_back(imageSeries);

    ::fwServices::IService::sptr srv;
    srv = ::fwServices::registry::ServiceFactory::getDefault()->create("::io::IReader", "::ioVTK::SSeriesDBReader" );

    CPPUNIT_ASSERT_MESSAGE("Create SSeriesDBReader failed", srv);

    ::fwServices::OSR::registerService(seriesDB, srv);
    srv->setConfiguration(readerSrvCfg);
    srv->configure();
    srv->start().wait();
    srv->update().wait();
    srv->stop().wait();
    ::fwServices::OSR::unregisterService(srv);

    CPPUNIT_ASSERT_EQUAL(size_t(2), seriesDB->size());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace ioVTK
