/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMDATA_UT_DICOMSERIESTEST_HPP__
#define __FWDICOMDATA_UT_DICOMSERIESTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include "fwDicomData/DicomSeries.hpp"

namespace fwDicomData
{
namespace ut
{

class DicomSeriesTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( DicomSeriesTest );
    CPPUNIT_TEST(dicomTest);
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void dicomTest();

private:
    ::fwDicomData::DicomSeries::sptr m_series;
};

} //namespace ut
} //namespace fwDicomData

#endif //__FWDICOMDATA_UT_DICOMSERIESTEST_HPP__

