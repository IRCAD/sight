/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_UT_FWDATA_PATIENT_V1TOFWMEDDATAPATIENTV1TEST_HPP__
#define __FWSTRUCTURALPATCH_UT_FWDATA_PATIENT_V1TOFWMEDDATAPATIENTV1TEST_HPP__

#include <cppunit/extensions/HelperMacros.h>


namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
{
namespace Patient
{
/**
 * @brief Test patch to convert ::fwData::Patient version 1 to ::fwMedData::Patient version 1.
 */
class V1ToFwMedDataPatientV1Test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( V1ToFwMedDataPatientV1Test );
CPPUNIT_TEST( applyPatchTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void applyPatchTest();
};
} //namespace Patient
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

#endif //__FWSTRUCTURALPATCH_UT_FWDATA_PATIENT_V1TOFWMEDDATAPATIENTV1TEST_HPP__
