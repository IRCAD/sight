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

#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>

#include <service/op.hpp>

#include <utest_data/Data.hpp>

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
    const std::filesystem::path imageFile = utest_data::Data::dir() / "sight/image/vtk/img.vtk";
    const std::filesystem::path meshFile  = utest_data::Data::dir() / "sight/mesh/vtk/sphere.vtk";

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

    auto series_set = std::make_shared<data::series_set>();

    service::base::sptr srv = service::add("sight::module::io::vtk::series_set_reader");

    CPPUNIT_ASSERT_MESSAGE("Create series_set_reader failed", srv);

    srv->set_inout(series_set, "data");
    srv->set_config(readerSrvCfg);
    srv->configure();
    srv->start().wait();
    srv->update().wait();
    srv->stop().wait();
    service::remove(srv);

    // Data expected
    const data::image::Spacing spacingExpected = {1.732, 1.732, 3.2};
    const data::image::Origin originExpected   = {34.64, 86.6, 56};
    const data::image::Size sizeExpected       = {230, 170, 58};

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());

    data::image_series::sptr imageSeries = std::dynamic_pointer_cast<data::image_series>(series_set->at(0));
    data::model_series::sptr modelSeries = std::dynamic_pointer_cast<data::model_series>(series_set->at(1));
    CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", imageSeries);
    CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", modelSeries);

    // Data read.
    const data::image::Spacing spacingRead = imageSeries->getSpacing();
    const data::image::Spacing originRead  = imageSeries->getOrigin();
    const data::image::Size sizeRead       = imageSeries->size();

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

    data::reconstruction::sptr rec1 = modelSeries->getReconstructionDB()[0];
    data::reconstruction::sptr rec2 = modelSeries->getReconstructionDB()[1];
    data::mesh::sptr mesh1          = rec1->getMesh();
    data::mesh::sptr mesh2          = rec2->getMesh();

    CPPUNIT_ASSERT_EQUAL((data::mesh::size_t) 720, mesh1->numCells());
    CPPUNIT_ASSERT_EQUAL((data::mesh::size_t) 362, mesh1->numPoints());

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

void SeriesSetReaderTest::testMergeSeriesSetReader()
{
    const std::filesystem::path imageFile = utest_data::Data::dir() / "sight/image/vtk/img.vtk";

    CPPUNIT_ASSERT_MESSAGE(
        "The file '" + imageFile.string() + "' does not exist",
        std::filesystem::exists(imageFile)
    );

    service::config_t readerSrvCfg;
    readerSrvCfg.add("file", imageFile.string());

    auto imageSeries = std::make_shared<data::image_series>();
    auto series_set  = std::make_shared<data::series_set>();
    series_set->push_back(imageSeries);

    service::base::sptr srv = service::add("sight::module::io::vtk::series_set_reader");

    CPPUNIT_ASSERT_MESSAGE("Create series_set_reader failed", srv);

    srv->set_inout(series_set, "data");
    srv->set_config(readerSrvCfg);
    srv->configure();
    srv->start().wait();
    srv->update().wait();
    srv->stop().wait();
    service::remove(srv);

    CPPUNIT_ASSERT_EQUAL(std::size_t(1), series_set->size());
}

} // namespace sight::module::io::vtk::ut
