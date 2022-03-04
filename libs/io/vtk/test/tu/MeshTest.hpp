/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <data/Array.hpp>
#include <data/Mesh.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::vtk
{

namespace ut
{

/**
 * @brief Test many methods to create mesh.
 */
class MeshTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(MeshTest);
CPPUNIT_TEST(testMeshToVtk);
CPPUNIT_TEST(testMeshToGrid);
CPPUNIT_TEST(testSyntheticMesh);
CPPUNIT_TEST(testExportImportSyntheticMesh);
CPPUNIT_TEST(testPointCloud);
CPPUNIT_TEST(testReadVtkFile);
CPPUNIT_TEST(testReadVtpFile);
CPPUNIT_TEST(testReadObjFile);
CPPUNIT_TEST(testReadStlFile);
CPPUNIT_TEST(testReadPlyFile);
CPPUNIT_TEST(testWriteVtkFile);
CPPUNIT_TEST(testWriteVtpFile);
CPPUNIT_TEST(testWriteObjFile);
CPPUNIT_TEST(testWriteStlFile);
CPPUNIT_TEST(testWritePlyFile);
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
    void testReadVtkFile();
    void testReadVtpFile();
    void testReadObjFile();
    void testReadPlyFile();
    void testReadStlFile();
    void testWriteVtkFile();
    void testWriteVtpFile();
    void testWriteObjFile();
    void testWritePlyFile();
    void testWriteStlFile();
};

} // namespace ut

} // namespace sight::io::vtk
