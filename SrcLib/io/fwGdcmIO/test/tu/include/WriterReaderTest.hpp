/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
