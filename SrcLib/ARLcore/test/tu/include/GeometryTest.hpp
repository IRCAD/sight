/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARLCORE_UT_GEOMETRYTEST_HPP__
#define __ARLCORE_UT_GEOMETRYTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

namespace ARLcore
{
namespace ut
{

class GeometryTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( GeometryTest );
CPPUNIT_TEST( ReconstructionPolyscopicTest );
CPPUNIT_TEST( SphereCenterEstimationTest );
CPPUNIT_TEST( RigidTransfoTest     );
CPPUNIT_TEST( EpipolarMatchingTest     );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();


    void ReconstructionPolyscopicTest();
    void SphereCenterEstimationTest();
    void RigidTransfoTest();
    void EpipolarMatchingTest();



};

} //namespace ut
} //namespace ARLCORE
#endif //__ARLCORE_UT_GEOMETRYTEST_HPP__

