/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMDATA_UT_FWDICOMDATACAMP_DICOMSERIESCAMPTEST_HPP__
#define __FWDICOMDATA_UT_FWDICOMDATACAMP_DICOMSERIESCAMPTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace fwDicomDataCamp
{
namespace ut
{

class DicomSeriesCampTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DicomSeriesCampTest );
CPPUNIT_TEST(propertiesTest);
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void propertiesTest();
};

} //namespace ut
} //namespace fwDicomDataCamp

#endif //__FWDICOMDATA_UT_FWDICOMDATACAMP_DICOMSERIESCAMPTEST_HPP__

