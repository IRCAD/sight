/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_TEST_TU_WRITERREADERTEST_HPP_
#define _GDCMIO_TEST_TU_WRITERREADERTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

namespace gdcmIO
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
} // namespace gdcmIO

#endif // _GDCMIO_TEST_TU_WRITERREADERTEST_HPP_
