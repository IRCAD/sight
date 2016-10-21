/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_UT_MESHTRIANTEST_HPP__
#define __FWDATAIO_UT_MESHTRIANTEST_HPP__

#include <fwData/Mesh.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <boost/filesystem/operations.hpp>

#include <cppunit/extensions/HelperMacros.h>

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

#endif //__FWDATAIO_UT_MESHTRIANTEST_HPP__
