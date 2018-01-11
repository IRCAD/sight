/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __PATCHMEDICALDATA_UT_PATCHTEST_HPP__
#define __PATCHMEDICALDATA_UT_PATCHTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace patchMedicalData
{
namespace ut
{

class PatchTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( PatchTest );
CPPUNIT_TEST( patchMedicalDataTest );
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void patchMedicalDataTest();
};

} //namespace ut
} //namespace patchMedicalData

#endif //__PATCHMEDICALDATA_UT_PATCHTEST_HPP__

