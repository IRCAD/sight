/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWVTKIO_TEST_TU_MESHTEST_HPP_
#define _FWVTKIO_TEST_TU_MESHTEST_HPP_

#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Mesh.hpp>
#include <fwData/Array.hpp>

/**
 * @brief Test many methods to create mesh.
 */
class MeshTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( MeshTest );
    CPPUNIT_TEST( testMeshToVtk );
    CPPUNIT_TEST( testSyntheticMesh );
    CPPUNIT_TEST( testExportImportSyntheticMesh );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testMeshToVtk();
    void testSyntheticMesh();
    void testExportImportSyntheticMesh();

    static void compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2);
    static void compareBuffer(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2);
};

#endif //_FWVTKIO_TEST_TU_MESHTEST_HPP_
