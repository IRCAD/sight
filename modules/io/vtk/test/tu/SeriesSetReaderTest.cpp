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

#include "SeriesSetReaderTest.hpp"

#include <core/tools/system.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <service/op.hpp>

#include <utestData/Data.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::io::vtk::ut::SeriesSetReaderTest);

static const double epsilon = 0.00001;

namespace sight::module::io::vtk::ut
{

//------------------------------------------------------------------------------

void SeriesSetReaderTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::testSeriesSetReader()
{
    const std::filesystem::path imageFile = utestData::Data::dir() / "sight/image/vtk/img.vtk";
    const std::filesystem::path meshFile  = utestData::Data::dir() / "sight/mesh/vtk/sphere.vtk";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imageFile.string() + "' does not exist",
        std::filesystem::exists(imageFile)
    );

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + meshFile.string() + "' does not exist",
        std::filesystem::exists(meshFile)
    );

    service::config_t readerSrvCfg;
    readerSrvCfg.add("file", imageFile.string());
    service::config_t file2Cfg;
    readerSrvCfg.add("file", meshFile.string());
    readerSrvCfg.add("file", meshFile.string());

    auto series_set = std::make_shared<data::SeriesSet>();

    service::base::sptr srv = service::add("sight::module::io::vtk::SSeriesSetReader");

    CPPUNIT_ASSERT_MESSAGE("Create SSeriesSetReader failed", srv);

    srv->setInOut(series_set, "data");
    srv->setConfiguration(readerSrvCfg);
    srv->configure();
    srv->start().wait();
    srv->update().wait();
    srv->stop().wait();
    service::remove(srv);

    // Data expected
    const data::Image::Spacing spacingExpected = {1.732, 1.732, 3.2};
    const data::Image::Origin originExpected   = {34.64, 86.6, 56};
    const data::Image::Size sizeExpected       = {230, 170, 58};

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());

    data::ImageSeries::sptr imageSeries = std::dynamic_pointer_cast<data::ImageSeries>(series_set->at(0));
    data::ModelSeries::sptr modelSeries = std::dynamic_pointer_cast<data::ModelSeries>(series_set->at(1));
    CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", imageSeries);
    CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", modelSeries);

    // Data read.
    const data::Image::Spacing spacingRead = imageSeries->getSpacing();
    const data::Image::Spacing originRead  = imageSeries->getOrigin();
    const data::Image::Size sizeRead       = imageSeries->size();

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

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), modelSeries->getReconstructionDB().size());

    data::Reconstruction::sptr rec1 = modelSeries->getReconstructionDB()[0];
    data::Reconstruction::sptr rec2 = modelSeries->getReconstructionDB()[1];
    data::Mesh::sptr mesh1          = rec1->getMesh();
    data::Mesh::sptr mesh2          = rec2->getMesh();

    CPPUNIT_ASSERT_EQUAL((data::Mesh::size_t) 720, mesh1->numCells());
    CPPUNIT_ASSERT_EQUAL((data::Mesh::size_t) 362, mesh1->numPoints());

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::testMergeSeriesSetReader()
{
    const std::filesystem::path imageFile = utestData::Data::dir() / "sight/image/vtk/img.vtk";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imageFile.string() + "' does not exist",
        std::filesystem::exists(imageFile)
    );

    service::config_t readerSrvCfg;
    readerSrvCfg.add("file", imageFile.string());

    auto imageSeries = std::make_shared<data::ImageSeries>();
    auto series_set  = std::make_shared<data::SeriesSet>();
    series_set->push_back(imageSeries);

    service::base::sptr srv = service::add("sight::module::io::vtk::SSeriesSetReader");

    CPPUNIT_ASSERT_MESSAGE("Create SSeriesSetReader failed", srv);

    srv->setInOut(series_set, "data");
    srv->setConfiguration(readerSrvCfg);
    srv->configure();
    srv->start().wait();
    srv->update().wait();
    srv->stop().wait();
    service::remove(srv);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
}

} // namespace sight::module::io::vtk::ut
