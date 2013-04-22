/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwData/Reconstruction.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwTest/Data.hpp>

#include <vtkIO/SeriesDBReader.hpp>

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

    const ::boost::filesystem::path imagePath( ::fwTest::Data::dir() / "fw4spl/image/vtk/img.vtk" );
    const ::boost::filesystem::path meshPath( ::fwTest::Data::dir() / "fw4spl/mesh/vtk/sphere.vtk" );

    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + imagePath.string(), ::boost::filesystem::exists(imagePath));
    CPPUNIT_ASSERT_MESSAGE(std::string("Missing file: ") + meshPath.string(), ::boost::filesystem::exists(meshPath));

    ::fwData::location::ILocation::VectPathType paths;
    paths.push_back(imagePath);
    paths.push_back(meshPath);
    paths.push_back(meshPath);

    ::vtkIO::SeriesDBReader::sptr reader = ::vtkIO::SeriesDBReader::New();
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
    BOOST_FOREACH( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop, (*props) )
    {
        OSLM_ERROR( "new object difference found : " << prop.first << " '" << prop.second << "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal" , props->size() == 0 );
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwVtkIO
