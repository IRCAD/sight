/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_TEST_TU_MESHTRIANTEST_HPP_
#define _FWDATAIO_TEST_TU_MESHTRIANTEST_HPP_

#include <boost/filesystem/operations.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <fwRuntime/EConfigurationElement.hpp>

namespace fwDataIO
{
namespace ut
{

/**
 * @brief Test many methods to read/create fw4spl trian mesh file.
 */
class MeshTrianTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( MeshTrianTest );
    CPPUNIT_TEST( testSimpleMesh );
    CPPUNIT_TEST( testMeshWithCellNormals );
    CPPUNIT_TEST_SUITE_END();


public:
    // interface
    void setUp();
    void tearDown();

    void testSimpleMesh();
    void testMeshWithCellNormals();

    void compareMesh(::fwData::Mesh::sptr mesh1, ::fwData::Mesh::sptr mesh2);


};

} //namespace ut
} //namespace fwDataIO

#endif //_FWDATAIO_TEST_TU_MESHTRIANTEST_HPP_
