/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWSTRUCTURALPATCH_TEST_TU_FWDATA_STUDY_V1TOFWMEDDATAPSTUDYV1TEST_HPP__
#define __FWSTRUCTURALPATCH_TEST_TU_FWDATA_STUDY_V1TOFWMEDDATAPSTUDYV1TEST_HPP__

#include <cppunit/extensions/HelperMacros.h>


namespace fwStructuralPatch
{
namespace ut
{
namespace fwData
{
namespace Study
{
/**
 * @brief Test patch to convert ::fwData::Study version 1 to ::fwMedData::Study version 1.
 */
class V1ToFwMedDataStudyV1Test : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( V1ToFwMedDataStudyV1Test );
    CPPUNIT_TEST( applyPatchTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void applyPatchTest();
};
} //namespace Study
} //namespace fwData
} //namespace ut
} //namespace fwStructuralPatch

#endif //__FWSTRUCTURALPATCH_TEST_TU_FWDATA_STUDY_V1TOFWMEDDATAPSTUDYV1TEST_HPP__
