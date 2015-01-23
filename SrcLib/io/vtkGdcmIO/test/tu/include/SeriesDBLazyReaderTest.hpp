/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKGDCMIO_TEST_TU_SERIESDBLAZYREADER_HPP__
#define __FWVTKGDCMIO_TEST_TU_SERIESDBLAZYREADER_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include <fwServices/macros.hpp>

namespace vtkGdcmIO
{
namespace ut
{

class SeriesDBLazyReaderTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SeriesDBLazyReaderTest );
    CPPUNIT_TEST( readSeriesDBTest );
    CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void readSeriesDBTest();

};

} // namespace ut
} // namespace vtkGdcmIO

#endif // __FWVTKGDCMIO_TEST_TU_SERIESDBLAZYREADER_HPP__

