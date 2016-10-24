/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "DicomSeriesDBReaderTest.hpp"

#include <fwDcmtkIO/SeriesDBReader.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwTest/Data.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDcmtkIO::ut::DicomSeriesDBReaderTest );

namespace fwDcmtkIO
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

void DicomSeriesDBReaderTest::readDicomSeriesDBTest()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    ::fwDcmtkIO::SeriesDBReader::sptr reader = ::fwDcmtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);

    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

}

} // namespace ut

} // namespace fwDcmtkIO

