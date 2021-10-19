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

#include "SeriesDBTest.hpp"

#include <core/memory/BufferManager.hpp>
#include <core/memory/BufferObject.hpp>

#include <data/Array.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Reconstruction.hpp>
#include <data/reflection/visitor/CompareObjects.hpp>
#include <data/Series.hpp>

#include <io/vtk/SeriesDBReader.hpp>

#include <utestData/Data.hpp>

#include <filesystem>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::SeriesDBTest);

namespace sight::io::vtk
{

namespace ut
{

//------------------------------------------------------------------------------

void SeriesDBTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SeriesDBTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesDBTest::testImportSeriesDB()
{
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();

    const std::filesystem::path imagePath(utestData::Data::dir() / "sight/image/vtk/img.vtk");
    const std::filesystem::path meshPath(utestData::Data::dir() / "sight/mesh/vtk/sphere.vtk");

    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + imagePath.string(), std::filesystem::exists(imagePath));
    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + meshPath.string(), std::filesystem::exists(meshPath));

    std::vector<std::filesystem::path> paths;
    paths.push_back(imagePath);
    paths.push_back(meshPath);
    paths.push_back(meshPath);

    io::vtk::SeriesDBReader::sptr reader = io::vtk::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFiles(paths);
    reader->read();

    CPPUNIT_ASSERT_EQUAL(size_t(2), seriesDB->getContainer().size());

    data::ImageSeries::sptr imgSeries = data::ImageSeries::dynamicCast(seriesDB->at(0));
    CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", imgSeries);

    data::ModelSeries::sptr modelSeries = data::ModelSeries::dynamicCast(seriesDB->at(1));
    CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", modelSeries);

    data::ModelSeries::ReconstructionVectorType recVect = modelSeries->getReconstructionDB();
    CPPUNIT_ASSERT_EQUAL(size_t(2), recVect.size());

    data::Reconstruction::sptr rec1 = recVect.at(0);
    data::Reconstruction::sptr rec2 = recVect.at(1);

    CPPUNIT_ASSERT_EQUAL(std::string("sphere"), rec1->getOrganName());
    CPPUNIT_ASSERT_EQUAL(std::string("sphere"), rec2->getOrganName());

    data::Mesh::sptr mesh1 = rec1->getMesh();
    data::Mesh::sptr mesh2 = rec2->getMesh();

    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (data::Mesh::size_t) 720);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (data::Mesh::size_t) 362);

    data::reflection::visitor::CompareObjects visitor;
    visitor.compare(mesh1, mesh2);
    SPTR(data::reflection::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for(data::reflection::visitor::CompareObjects::PropsMapType::value_type prop : (*props))
    {
        SIGHT_ERROR("new object difference found : " << prop.first << " '" << prop.second << "'");
    }

    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0);
}

//------------------------------------------------------------------------------

bool isLoaded(core::memory::BufferObject::sptr bo)
{
    core::memory::BufferManager::csptr manager                    = core::memory::BufferManager::getDefault();
    const core::memory::BufferManager::BufferInfoMapType mapInfos = manager->getBufferInfos().get();

    core::memory::BufferManager::BufferInfoMapType::const_iterator iter = mapInfos.find(bo->getBufferPointer());
    CPPUNIT_ASSERT_MESSAGE("BufferInfo not found.", iter != mapInfos.end());
    const core::memory::BufferInfo& info = iter->second;

    return info.loaded;
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace sight::io::vtk
