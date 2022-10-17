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

#include "SeriesSetTest.hpp"

#include <core/memory/BufferManager.hpp>
#include <core/memory/BufferObject.hpp>

#include <data/Array.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/Series.hpp>

#include <io/vtk/SeriesSetReader.hpp>

#include <utestData/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::SeriesSetTest);

namespace sight::io::vtk::ut
{

//------------------------------------------------------------------------------

void SeriesSetTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SeriesSetTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesSetTest::testImportSeriesSet()
{
    auto series_set = data::SeriesSet::New();

    const std::filesystem::path imagePath(utestData::Data::dir() / "sight/image/vtk/img.vtk");
    const std::filesystem::path meshPath(utestData::Data::dir() / "sight/mesh/vtk/sphere.vtk");

    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + imagePath.string(), std::filesystem::exists(imagePath));
    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + meshPath.string(), std::filesystem::exists(meshPath));

    std::vector<std::filesystem::path> paths;
    paths.push_back(imagePath);
    paths.push_back(meshPath);
    paths.push_back(meshPath);

    io::vtk::SeriesSetReader::sptr reader = io::vtk::SeriesSetReader::New();
    reader->setObject(series_set);
    reader->setFiles(paths);
    reader->read();

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), series_set->size());

    data::ImageSeries::sptr imgSeries = data::ImageSeries::dynamicCast(series_set->at(0));
    CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", imgSeries);

    data::ModelSeries::sptr modelSeries = data::ModelSeries::dynamicCast(series_set->at(1));
    CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", modelSeries);

    data::ModelSeries::ReconstructionVectorType recVect = modelSeries->getReconstructionDB();
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), recVect.size());

    data::Reconstruction::sptr rec1 = recVect.at(0);
    data::Reconstruction::sptr rec2 = recVect.at(1);

    CPPUNIT_ASSERT_EQUAL(std::string("sphere"), rec1->getOrganName());
    CPPUNIT_ASSERT_EQUAL(std::string("sphere"), rec2->getOrganName());

    data::Mesh::sptr mesh1 = rec1->getMesh();
    data::Mesh::sptr mesh2 = rec2->getMesh();

    CPPUNIT_ASSERT_EQUAL(mesh1->numCells(), (data::Mesh::size_t) 720);
    CPPUNIT_ASSERT_EQUAL(mesh1->numPoints(), (data::Mesh::size_t) 362);

    CPPUNIT_ASSERT(*mesh1 == *mesh2);
}

//------------------------------------------------------------------------------

bool isLoaded(core::memory::BufferObject::sptr bo)
{
    core::memory::BufferManager::csptr manager                    = core::memory::BufferManager::getDefault();
    const core::memory::BufferManager::BufferInfoMapType mapInfos = manager->getBufferInfos().get();

    auto iter = mapInfos.find(bo->getBufferPointer());
    CPPUNIT_ASSERT_MESSAGE("BufferInfo not found.", iter != mapInfos.end());
    const core::memory::BufferInfo& info = iter->second;

    return info.loaded;
}

} // namespace sight::io::vtk::ut
