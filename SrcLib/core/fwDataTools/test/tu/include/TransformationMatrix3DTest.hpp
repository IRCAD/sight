/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_TEST_TU_TRANSFORMATIONMATRIX3DTEST_HPP__
#define __FWDATATOOLS_TEST_TU_TRANSFORMATIONMATRIX3DTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/TransformationMatrix3D.hpp>


namespace fwDataTools
{
namespace ut
{

/**
 * @brief Test many methods to create image.
 */
class TransformationMatrix3DTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TransformationMatrix3DTest );
    CPPUNIT_TEST( identityMatrixTest );
    CPPUNIT_TEST( matrixTest );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();


    void identityMatrixTest();
    void matrixTest();

};

} //namespace ut
} //namespace fwDataTools

#endif //__FWDATATOOLS_TEST_TU_TRANSFORMATIONMATRIX3DTEST_HPP__
