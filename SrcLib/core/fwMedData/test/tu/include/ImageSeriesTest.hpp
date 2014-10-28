/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_UT_IMAGESERIESTEST_HPP__
#define __FWMEDDATA_UT_IMAGESERIESTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedData
{
namespace ut
{

class ImageSeriesTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( ImageSeriesTest );
    CPPUNIT_TEST(imageTest);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void imageTest();

private:
    ImageSeries::sptr m_series;
};

} //namespace ut
} //namespace fwMedData

#endif //__FWMEDDATA_UT_IMAGESERIESTEST_HPP__

