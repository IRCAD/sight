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

#include "SeriesDBReaderTest.hpp"

#include <core/runtime/EConfigurationElement.hpp>
#include <core/tools/System.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/location/ILocation.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/reflection/visitor/CompareObjects.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>
#include <service/registry/ObjectService.hpp>

#include <utestData/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::module::io::vtk::ut::SeriesDBReaderTest );

static const double epsilon = 0.00001;

namespace sight::module::io::vtk
{
namespace ut
{

//-----------------------------------------------------------------------------

void compare(data::Object::sptr objRef, data::Object::sptr objComp)
{
    data::reflection::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(data::reflection::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for( data::reflection::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        SIGHT_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
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
    const std::filesystem::path imageFile = utestData::Data::dir() / "sight/image/vtk/img.vtk";
    const std::filesystem::path meshFile  = utestData::Data::dir() / "sight/mesh/vtk/sphere.vtk";

    CPPUNIT_ASSERT_MESSAGE("The file '" + imageFile.string() + "' does not exist",
                           std::filesystem::exists(imageFile));

    CPPUNIT_ASSERT_MESSAGE("The file '" + meshFile.string() + "' does not exist",
                           std::filesystem::exists(meshFile));

    core::runtime::EConfigurationElement::sptr readerSrvCfg = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr file1Cfg     = core::runtime::EConfigurationElement::New("file");
    core::runtime::EConfigurationElement::sptr file2Cfg     = core::runtime::EConfigurationElement::New("file");
    file1Cfg->setValue(imageFile.string());
    readerSrvCfg->addConfigurationElement(file1Cfg);
    file2Cfg->setValue(meshFile.string());
    readerSrvCfg->addConfigurationElement(file2Cfg);
    readerSrvCfg->addConfigurationElement(file2Cfg);

    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    service::IService::sptr srv = service::add( "::sight::module::io::vtk::SSeriesDBReader" );

    CPPUNIT_ASSERT_MESSAGE("Create SSeriesDBReader failed", srv);

    srv->registerInOut(seriesDB, "data");
    srv->setConfiguration(readerSrvCfg);
    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    service::OSR::unregisterService(srv);

    // Data expected
    const data::Image::Spacing spacingExpected = {1.732, 1.732, 3.2};
    const data::Image::Origin originExpected   = {34.64, 86.6, 56};
    const data::Image::Size sizeExpected       = { 230, 170, 58};

    CPPUNIT_ASSERT_EQUAL(size_t(2), seriesDB->size());

    data::ImageSeries::sptr imageSeries = data::ImageSeries::dynamicCast(seriesDB->getContainer()[0]);
    data::ModelSeries::sptr modelSeries = data::ModelSeries::dynamicCast(seriesDB->getContainer()[1]);
    CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", imageSeries);
    CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", modelSeries);

    // Data read.
    data::Image::sptr image                = imageSeries->getImage();
    const data::Image::Spacing spacingRead = image->getSpacing2();
    const data::Image::Spacing originRead  = image->getOrigin2();
    const data::Image::Size sizeRead       = image->getSize2();

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

    data::Reconstruction::sptr rec1 = modelSeries->getReconstructionDB()[0];
    data::Reconstruction::sptr rec2 = modelSeries->getReconstructionDB()[1];
    data::Mesh::sptr mesh1          = rec1->getMesh();
    data::Mesh::sptr mesh2          = rec2->getMesh();

    CPPUNIT_ASSERT_EQUAL((data::Mesh::Size)720, mesh1->getNumberOfCells());
    CPPUNIT_ASSERT_EQUAL((data::Mesh::Size)362, mesh1->getNumberOfPoints());

    compare(mesh1, mesh2);
}

//------------------------------------------------------------------------------

void SeriesDBReaderTest::testMergeSeriesDBReader()
{
    const std::filesystem::path imageFile = utestData::Data::dir() / "sight/image/vtk/img.vtk";

    CPPUNIT_ASSERT_MESSAGE("The file '" + imageFile.string() + "' does not exist",
                           std::filesystem::exists(imageFile));

    core::runtime::EConfigurationElement::sptr readerSrvCfg = core::runtime::EConfigurationElement::New("service");
    core::runtime::EConfigurationElement::sptr fileCfg      = core::runtime::EConfigurationElement::New("file");
    fileCfg->setValue(imageFile.string());
    readerSrvCfg->addConfigurationElement(fileCfg);

    data::ImageSeries::sptr imageSeries = data::ImageSeries::New();
    data::SeriesDB::sptr seriesDB       = data::SeriesDB::New();
    seriesDB->getContainer().push_back(imageSeries);

    service::IService::sptr srv = service::add( "::sight::module::io::vtk::SSeriesDBReader" );

    CPPUNIT_ASSERT_MESSAGE("Create SSeriesDBReader failed", srv);

    srv->registerInOut(seriesDB, "data");
    srv->setConfiguration(readerSrvCfg);
    srv->configure();
    srv->start().wait();
    srv->update().wait();
    srv->stop().wait();
    service::OSR::unregisterService(srv);

    CPPUNIT_ASSERT_EQUAL(size_t(1), seriesDB->size());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::module::io::vtk
