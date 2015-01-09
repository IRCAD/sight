/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_UT_SERIESDBTEST_HPP__
#define __FWMEDDATA_UT_SERIESDBTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>
#include "fwMedData/SeriesDB.hpp"

namespace fwMedData
{

namespace ut
{

class SeriesDBTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( SeriesDBTest );

    CPPUNIT_TEST(containerTest);

    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void containerTest();

protected:

    SPTR(::fwMedData::SeriesDB) m_series;

};

} //namespace ut
} //namespace fwMedData

#endif //__FWMEDDATA_UT_SERIESDBTEST_HPP__

