/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_UT_NAVIGATIONSERIESTEST_HPP__
#define __FWMEDDATA_UT_NAVIGATIONSERIESTEST_HPP__

#include <fwMedData/NavigationSeries.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedData
{
namespace ut
{

class NavigationSeriesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( NavigationSeriesTest );
CPPUNIT_TEST(navigationTest);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void navigationTest();

private:
    ::fwMedData::NavigationSeries::sptr m_series;
};

} //namespace ut
} //namespace fwMedData

#endif //__FWMEDDATA_UT_NAVIGATIONSERIESTEST_HPP__

