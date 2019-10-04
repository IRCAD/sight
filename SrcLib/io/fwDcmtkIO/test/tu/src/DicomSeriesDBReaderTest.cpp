/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

    const std::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/JMSGenou";

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           std::filesystem::exists(path));

    ::fwDcmtkIO::SeriesDBReader::sptr reader = ::fwDcmtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFolder(path);

    CPPUNIT_ASSERT_NO_THROW(reader->readDicomSeries());

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());

}

} // namespace ut

} // namespace fwDcmtkIO
