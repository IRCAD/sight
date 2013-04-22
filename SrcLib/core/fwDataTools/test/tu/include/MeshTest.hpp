/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_TEST_TU_MESHTEST_HPP_
#define _FWDATATOOLS_TEST_TU_MESHTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Mesh.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Array.hpp>

namespace fwDataTools
{
namespace ut
{

/**
 * @brief Test many methods to create mesh.
 */
class MeshTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( MeshTest );
    CPPUNIT_TEST( testMeshToTriangularMesh );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testMeshToTriangularMesh();

};

} //namespace ut
} //namespace fwDataTools

#endif //_FWDATATOOLS_TEST_TU_MESHTEST_HPP_
