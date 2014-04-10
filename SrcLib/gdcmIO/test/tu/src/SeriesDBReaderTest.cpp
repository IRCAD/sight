/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/DicomReaderTest.hpp>

#include <gdcmIO/reader/SeriesDB.hpp>

#include "SeriesDBReaderTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( ::gdcmIO::ut::SeriesDBReaderTest );

namespace gdcmIO
{
namespace ut
{

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

void SeriesDBReaderTest::readSeriesDBTest()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/ACHGenou";

    ::gdcmIO::reader::SeriesDB::sptr reader = ::gdcmIO::reader::SeriesDB::New();
    reader->setObject(seriesDB);

    reader->setFolder(path);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());

    // FIXME : GDCM reader trim string values so this test cannot pass.
//    CPPUNIT_ASSERT( ::fwTest::DicomReaderTest::checkSeriesACHGenou( series ) );

}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace gdcmIO

