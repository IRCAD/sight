/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWXML_TEST_TU_MESHTEST_HPP_
#define _FWXML_TEST_TU_MESHTEST_HPP_

#include <boost/foreach.hpp>
#include <cppunit/extensions/HelperMacros.h>

#include <fwData/Mesh.hpp>
#include <fwData/Array.hpp>

/**
 * @brief Test many methods to create mesh.
 */
class MeshTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( MeshTest );
    CPPUNIT_TEST( testExportImportSyntheticMesh );
    CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testExportImportSyntheticMesh();

    void compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2);
    void compareBuffer(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2);
    void compareDataArrayMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2);
};

#endif //_FWXML_TEST_TU_MESHTEST_HPP_
