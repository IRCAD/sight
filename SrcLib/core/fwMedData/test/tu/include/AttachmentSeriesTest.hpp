/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_UT_ATTACHMENTSERIESTEST_HPP__
#define __FWMEDDATA_UT_ATTACHMENTSERIESTEST_HPP__

#include <fwMedData/AttachmentSeries.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedData
{
namespace ut
{

class AttachmentSeriesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( AttachmentSeriesTest );
CPPUNIT_TEST(attachmentTest);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void attachmentTest();

private:
    ::fwMedData::AttachmentSeries::sptr m_series;
};

} //namespace ut
} //namespace fwMedData

#endif //__FWMEDDATA_UT_ATTACHMENTSERIESTEST_HPP__

