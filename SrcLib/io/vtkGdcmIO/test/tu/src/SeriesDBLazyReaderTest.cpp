/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#if (SPYLOG_LEVEL >= 4 )
#include <fwCore/HiResTimer.hpp>
#endif

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwDataTools/helper/Image.hpp>

#include <fwTest/Data.hpp>
#include <fwTest/DicomReaderTest.hpp>

#include <vtkGdcmIO/SeriesDBLazyReader.hpp>

#include "SeriesDBLazyReaderTest.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( ::vtkGdcmIO::ut::SeriesDBLazyReaderTest );

namespace vtkGdcmIO
{
namespace ut
{

//------------------------------------------------------------------------------

void SeriesDBLazyReaderTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SeriesDBLazyReaderTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesDBLazyReaderTest::readSeriesDBTest()
{
    ::fwMedData::SeriesDB::sptr seriesDB = ::fwMedData::SeriesDB::New();

    const ::boost::filesystem::path path = ::fwTest::Data::dir() / "sight/Patient/Dicom/ACHGenou";

    CPPUNIT_ASSERT_MESSAGE("The dicom directory '" + path.string() + "' does not exist",
                           ::boost::filesystem::exists(path));

    ::vtkGdcmIO::SeriesDBLazyReader::sptr reader = ::vtkGdcmIO::SeriesDBLazyReader::New();
    reader->setObject(seriesDB);

    reader->setFolder(path);

    CPPUNIT_ASSERT_NO_THROW(reader->read());

    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), seriesDB->size());
    ::fwMedData::ImageSeries::sptr series = ::fwMedData::ImageSeries::dynamicCast(seriesDB->front());

    CPPUNIT_ASSERT( ::fwTest::DicomReaderTest::checkSeriesACHGenou( series ) );

#if (SPYLOG_LEVEL >= 4 ) // Log level info
    ::fwCore::HiResTimer timer;
    timer.start();
#endif

    ::fwDataTools::helper::Image locker ( series->getImage() );

    #if (SPYLOG_LEVEL >= 4 )
    timer.stop();
    OSLM_INFO( "Time in to read data : " << timer.getElapsedTimeInMilliSec() );
#endif

}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace vtkGdcmIO

