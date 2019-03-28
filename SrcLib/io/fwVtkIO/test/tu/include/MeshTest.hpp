/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include <fwData/Array.hpp>
#include <fwData/Mesh.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwVtkIO
{
namespace ut
{

/**
 * @brief Test many methods to create mesh.
 */
class MeshTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MeshTest );
CPPUNIT_TEST( testMeshToVtk );
CPPUNIT_TEST( testMeshToGrid );
CPPUNIT_TEST( testSyntheticMesh );
CPPUNIT_TEST( testExportImportSyntheticMesh );
CPPUNIT_TEST( testPointCloud );
CPPUNIT_TEST( testMeshUpdatePoints );
CPPUNIT_TEST( testMeshUpdateColors );
CPPUNIT_TEST( testMeshUpdateNormals );
CPPUNIT_TEST( testGridUpdatePoints );
CPPUNIT_TEST( testGridUpdateColors );
CPPUNIT_TEST( testGridUpdateNormals );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void testMeshToVtk();
    void testMeshToGrid();
    void testSyntheticMesh();
    void testExportImportSyntheticMesh();
    void testPointCloud();
    void testMeshUpdatePoints();
    void testMeshUpdateColors();
    void testMeshUpdateNormals();
    void testGridUpdatePoints();
    void testGridUpdateColors();
    void testGridUpdateNormals();
    void testGridUpdteTexCoords();

};

} // namespace ut
} // namespace fwVtkIO
