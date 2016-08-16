/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_UT_DICOMSERIESTEST_HPP__
#define __FWMEDDATA_UT_DICOMSERIESTEST_HPP__

#include "fwMedData/DicomSeries.hpp"

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedData
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
    ::fwMedData::DicomSeries::sptr m_series;
};

} //namespace ut
} //namespace fwMedData

#endif //__FWMEDDATA_UT_DICOMSERIESTEST_HPP__

