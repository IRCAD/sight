/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwTools/System.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwData/Object.hpp>
#include <fwData/Image.hpp>

#include <fwMedData/SeriesDB.hpp>
#include <fwDicomData/DicomSeries.hpp>

#include <fwTest/Data.hpp>

#include <fwDicomIOExt/dcmtk/SeriesDBReader.hpp>
#include <fwDicomIOExt/gdcm/DicomSeriesWriter.hpp>
#include <fwDicomIOExt/gdcm/DicomSeriesDBReader.hpp>


#include "DicomSeriesWriterTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOExt::ut::DicomSeriesWriterTest );

namespace fwDicomIOExt
{
namespace ut
{

//-----------------------------------------------------------------------------

void compareDicom(::fwData::Object::sptr objRef, ::fwData::Object::sptr objComp)
{
    ::fwDataCamp::visitor::CompareObjects visitor;
    visitor.compare(objRef, objComp);
    SPTR(::fwDataCamp::visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    BOOST_FOREACH( ::fwDataCamp::visitor::CompareObjects::PropsMapType::value_type prop, (*props) )
    {
        SLM_ERROR( "new object difference found : " + prop.first + " '" + prop.second + "'" );
    }
    CPPUNIT_ASSERT_MESSAGE("Object Not equal", props->size() == 0 );
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomSeriesWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

// TODO: This test is disabled as DicomSeriesDBReader doesn't store Binaries anymore.
//void DicomSeriesWriterTest::writeReadTest()
//{
//    ::fwMedData::SeriesDB::sptr srcSeriesDB = ::fwMedData::SeriesDB::New();
//    ::fwMedData::SeriesDB::sptr destSeriesDB = ::fwMedData::SeriesDB::New();
//
//    const ::boost::filesystem::path srcPath = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";
//    const ::boost::filesystem::path destPath = ::fwTools::System::getTemporaryFolder() / "dicomSeriesTest";
//    ::boost::filesystem::create_directories( destPath );
//
//    // Read Source Dicom
//    ::fwDicomIOExt::gdcm::DicomSeriesDBReader::sptr reader = ::fwDicomIOExt::gdcm::DicomSeriesDBReader::New();
//    reader->setObject(srcSeriesDB);
//    reader->setFolder(srcPath);
//    CPPUNIT_ASSERT_NO_THROW(reader->read());
//    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), srcSeriesDB->size());
//    ::fwDicomData::DicomSeries::sptr srcDicomSeries =
//            ::fwDicomData::DicomSeries::dynamicCast(srcSeriesDB->getContainer().front());
//
//    // Write Dicom
//    ::fwDicomIOExt::gdcm::DicomSeriesWriter::sptr writer = ::fwDicomIOExt::gdcm::DicomSeriesWriter::New();
//    writer->setObject(srcDicomSeries);
//    writer->setFolder(destPath);
//    CPPUNIT_ASSERT_NO_THROW(writer->write());
//
//    // Read Destination Dicom
//    reader->setObject(destSeriesDB);
//    reader->setFolder(destPath);
//    CPPUNIT_ASSERT_NO_THROW(reader->read());
//    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), destSeriesDB->size());
//    ::fwDicomData::DicomSeries::sptr destDicomSeries =
//            ::fwDicomData::DicomSeries::dynamicCast(destSeriesDB->getContainer().front());
//
//    // Compare Source and Destination Series
//    compareDicom(srcDicomSeries, destDicomSeries);
//}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwDicomIOExt
