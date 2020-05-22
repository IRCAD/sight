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

#ifndef __FWGDCMIO_UT_WRITERREADERTEST_HPP__
#define __FWGDCMIO_UT_WRITERREADERTEST_HPP__

#include <fwMedData/SeriesDB.hpp>
#include <fwServices/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwGdcmIO
{
namespace ut
{

class WriterReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( WriterReaderTest );
CPPUNIT_TEST( writeReadImageSeriesTest );
//FIXME: This test create wrong (random) Dicom Images that may cause the test to crash.
//    CPPUNIT_TEST( writeReadSeriesDBTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void writeReadImageSeriesTest();
    void writeReadSeriesDBTest();

private:

    /**
     * Create a series DB containing an ImageSeries with landmarks and distance
     * and a ModelSeries with one reconstruction.
     */
    ::fwMedData::SeriesDB::sptr createSeriesDB();


};

} // namespace ut
} // namespace fwGdcmIO

#endif // __FWGDCMIO_UT_WRITERREADERTEST_HPP__
