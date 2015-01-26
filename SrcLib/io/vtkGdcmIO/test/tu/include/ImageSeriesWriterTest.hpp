/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKGDCMIO_TEST_TU_IMAGESERIESWRITERTEST_HPP__
#define __FWVTKGDCMIO_TEST_TU_IMAGESERIESWRITERTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>

namespace vtkGdcmIO
{
namespace ut
{

class ImageSeriesWriterTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageSeriesWriterTest );
    CPPUNIT_TEST( writeReadTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void writeReadTest();

};

} // namespace ut
} // namespace vtkGdcmIO

#endif // __FWVTKGDCMIO_TEST_TU_IMAGESERIESWRITERTEST_HPP__
