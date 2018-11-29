/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwMemory/BufferManager.hpp>
#include <fwMemory/BufferObject.hpp>

#include <fwData/Reconstruction.hpp>
#include <fwData/Image.hpp>
#include <fwData/Array.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwTest/Data.hpp>

#include <fwVtkIO/SeriesDBReader.hpp>

#include "SeriesDBTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwVtkIO::ut::SeriesDBTest );

namespace fwVtkIO
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
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const ::boost::filesystem::path imagePath( ::fwTest::Data::dir() / "sight/image/vtk/img.vtk" );
    const ::boost::filesystem::path meshPath( ::fwTest::Data::dir() / "sight/mesh/vtk/sphere.vtk" );

    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + imagePath.string(), ::boost::filesystem::exists(imagePath));
    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + meshPath.string(), ::boost::filesystem::exists(meshPath));

    ::fwData::location::ILocation::VectPathType paths;
    paths.push_back(imagePath);
    paths.push_back(meshPath);
    paths.push_back(meshPath);

    ::fwVtkIO::SeriesDBReader::sptr reader = ::fwVtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFiles(paths);
    reader->read();

    CPPUNIT_ASSERT_EQUAL(size_t(2), seriesDB->getContainer().size());

    ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::dynamicCast(seriesDB->at(0));
    CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", imgSeries);

    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(seriesDB->at(1));
    CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", modelSeries);

    ::fwMedData::ModelSeries::ReconstructionVectorType recVect = modelSeries->getReconstructionDB();
    CPPUNIT_ASSERT_EQUAL(size_t(2), recVect.size());

    ::fwData::Reconstruction::sptr rec1 = recVect.at(0);
    ::fwData::Reconstruction::sptr rec2 = recVect.at(0);

    CPPUNIT_ASSERT_EQUAL(std::string("sphere"), rec1->getOrganName());
    CPPUNIT_ASSERT_EQUAL(std::string("sphere"), rec2->getOrganName());

    ::fwData::Mesh::sptr mesh1 = rec1->getMesh();
    ::fwData::Mesh::sptr mesh2 = rec1->getMesh();

    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfCells(), (::fwData::Mesh::Id)720);
    CPPUNIT_ASSERT_EQUAL(mesh1->getNumberOfPoints(), (::fwData::Mesh::Id)362);

    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(mesh1, mesh2);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    for( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop :  (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0 );
}

//------------------------------------------------------------------------------

bool isLoaded(::fwMemory::BufferObject::sptr bo)
{
    ::fwMemory::BufferManager::csptr manager = ::fwMemory::BufferManager::getDefault();
    const ::fwMemory::BufferManager::BufferInfoMapType mapInfos = manager->getBufferInfos().get();

    ::fwMemory::BufferManager::BufferInfoMapType::const_iterator iter = mapInfos.find(bo->getBufferPointer());
    CPPUNIT_ASSERT_MESSAGE("BufferInfo not found.", iter != mapInfos.end());
    const ::fwMemory::BufferInfo& info = iter->second;

    return info.loaded;
}

//------------------------------------------------------------------------------

void SeriesDBTest::testLazyImportSeriesDB()
{
    ::fwMemory::BufferManager::sptr manager = ::fwMemory::BufferManager::getDefault();
    {
        ::fwCore::mt::WriteLock lock( manager->getMutex() );
        manager->setLoadingMode(::fwMemory::BufferManager::LAZY);
    }

    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const ::boost::filesystem::path imagePath( ::fwTest::Data::dir() / "sight/image/vtk/img.vtk" );
    const ::boost::filesystem::path meshPath( ::fwTest::Data::dir() / "sight/mesh/vtk/sphere.vtk" );

    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + imagePath.string(), ::boost::filesystem::exists(imagePath));
    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + meshPath.string(), ::boost::filesystem::exists(meshPath));

    ::fwData::location::ILocation::VectPathType paths;
    paths.push_back(imagePath);
    paths.push_back(meshPath);


    ::fwVtkIO::SeriesDBReader::sptr reader = ::fwVtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFiles(paths);
    reader->setLazyMode(true);
    reader->read();


    CPPUNIT_ASSERT_EQUAL(size_t(2), seriesDB->getContainer().size());

    //check ImageSeries
    {
        ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::dynamicCast(seriesDB->at(0));
        CPPUNIT_ASSERT_MESSAGE("ImageSeries dynamicCast failed", imgSeries);

        ::fwMemory::BufferObject::sptr bo = imgSeries->getImage()->getDataArray()->getBufferObject();
        CPPUNIT_ASSERT_MESSAGE("ImageSeries is not lazy-loaded", !isLoaded(bo));

        ::fwMemory::BufferObject::Lock lock = bo->lock();

        CPPUNIT_ASSERT_MESSAGE("ImageSeries is still lazy-loaded", isLoaded(bo));
    }

    //check ModelSeries
    {
        ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(seriesDB->at(1));
        CPPUNIT_ASSERT_MESSAGE("ModelSeries dynamicCast failed", modelSeries);

        ::fwMedData::ModelSeries::ReconstructionVectorType recVect = modelSeries->getReconstructionDB();
        CPPUNIT_ASSERT_EQUAL(size_t(1), recVect.size());

        ::fwData::Mesh::sptr mesh = recVect[0]->getMesh();

        ::fwMemory::BufferObject::sptr points = mesh->getPointsArray()->getBufferObject();
        CPPUNIT_ASSERT_MESSAGE("ModelSeries points are lazy-loaded", isLoaded(points));

        ::fwMemory::BufferObject::sptr cell = mesh->getCellDataArray()->getBufferObject();
        CPPUNIT_ASSERT_MESSAGE("ModelSeries cells are lazy-loaded", isLoaded(cell));
    }

    {
        ::fwCore::mt::WriteLock lock( manager->getMutex() );
        manager->setLoadingMode(::fwMemory::BufferManager::DIRECT);
    }
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwVtkIO
