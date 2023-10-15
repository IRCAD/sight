/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <data/array.hpp>
#include <data/mesh.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::vtk::ut
{

/**
 * @brief Test many methods to create mesh.
 */
class mesh_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(mesh_test);
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

CPPUNIT_TEST(toVtkMeshWithLinesTest);
CPPUNIT_TEST(toVtkMeshWithTetrasTest);
CPPUNIT_TEST(toVtkMeshWithCellTexCoordsTest);
CPPUNIT_TEST(fromVtkMeshWithLinesTest);
CPPUNIT_TEST(fromVtkMeshWithQuadsTest);
CPPUNIT_TEST(fromVtkMeshWithCellTexCoordsTest);
CPPUNIT_TEST(fromVtkMeshWith3ComponentsCellColorsTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void testMeshToVtk();
    static void testMeshToGrid();
    static void testSyntheticMesh();
    static void testExportImportSyntheticMesh();
    static void testPointCloud();
    static void testReadVtkFile();
    static void testReadVtpFile();
    static void testReadObjFile();
    static void testReadPlyFile();
    static void testReadStlFile();
    static void testWriteVtkFile();
    static void testWriteVtpFile();
    static void testWriteObjFile();
    static void testWritePlyFile();
    static void testWriteStlFile();

    static void toVtkMeshWithLinesTest();
    static void toVtkMeshWithTetrasTest();
    static void toVtkMeshWithCellTexCoordsTest();
    static void fromVtkMeshWithLinesTest();
    static void fromVtkMeshWithQuadsTest();
    static void fromVtkMeshWithCellTexCoordsTest();
    static void fromVtkMeshWith3ComponentsCellColorsTest();
};

} // namespace sight::io::vtk::ut
