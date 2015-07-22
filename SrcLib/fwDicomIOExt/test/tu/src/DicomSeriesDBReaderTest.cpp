/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/DicomReaderTest.hpp>

#include <fwDicomIOExt/dcmtk/SeriesDBReader.hpp>
#include <fwDicomIOExt/gdcm/DicomSeriesDBReader.hpp>

#include "DicomSeriesDBReaderTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomIOExt::ut::DicomSeriesDBReaderTest );

namespace fwDicomIOExt
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomSeriesDBReaderTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomSeriesDBReaderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomSeriesDBReaderTest::readDicomSeriesDBGDCMTest()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    ::fwDicomIOExt::gdcm::DicomSeriesDBReader::sptr reader = ::fwDicomIOExt::gdcm::DicomSeriesDBReader::New();
    reader->setObject(seriesDB);

    reader->setFolder(path);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

}

//------------------------------------------------------------------------------

void DicomSeriesDBReaderTest::readDicomSeriesDBDCMTKTest()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    ::fwDicomIOExt::dcmtk::SeriesDBReader::sptr reader = ::fwDicomIOExt::dcmtk::SeriesDBReader::New();
    reader->setObject(seriesDB);

    reader->setFolder(path);

    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

}

} // namespace ut

} // namespace fwDicomIOExt

