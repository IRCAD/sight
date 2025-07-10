/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
CPPUNIT_TEST(test_mesh_to_vtk);
CPPUNIT_TEST(test_mesh_to_grid);
CPPUNIT_TEST(test_synthetic_mesh);
CPPUNIT_TEST(test_export_import_synthetic_mesh);
CPPUNIT_TEST(test_point_cloud);
CPPUNIT_TEST(test_read_vtk_file);

// FIXME: This test is disabled on linux until vtk is patched upstream.
// see https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=1064762
#ifdef WIN32
CPPUNIT_TEST(test_read_vtp_file);
CPPUNIT_TEST(test_write_vtp_file);
#endif

CPPUNIT_TEST(test_read_obj_file);
CPPUNIT_TEST(test_read_stl_file);
CPPUNIT_TEST(test_read_ply_file);
CPPUNIT_TEST(test_write_vtk_file);
CPPUNIT_TEST(test_write_obj_file);
CPPUNIT_TEST(test_write_stl_file);
CPPUNIT_TEST(test_write_ply_file);

CPPUNIT_TEST(to_vtk_mesh_with_lines_test);
CPPUNIT_TEST(to_vtk_mesh_with_tetras_test);
CPPUNIT_TEST(to_vtk_mesh_with_cell_tex_coords_test);
CPPUNIT_TEST(from_vtk_mesh_with_lines_test);
CPPUNIT_TEST(from_vtk_mesh_with_quads_test);
CPPUNIT_TEST(from_vtk_mesh_with_cell_tex_coords_test);
CPPUNIT_TEST(from_vtk_mesh_with3_components_cell_colors_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void test_mesh_to_vtk();
    static void test_mesh_to_grid();
    static void test_synthetic_mesh();
    static void test_export_import_synthetic_mesh();
    static void test_point_cloud();
    static void test_read_vtk_file();
    static void test_read_vtp_file();
    static void test_read_obj_file();
    static void test_read_ply_file();
    static void test_read_stl_file();
    static void test_write_vtk_file();
    static void test_write_vtp_file();
    static void test_write_obj_file();
    static void test_write_ply_file();
    static void test_write_stl_file();

    static void to_vtk_mesh_with_lines_test();
    static void to_vtk_mesh_with_tetras_test();
    static void to_vtk_mesh_with_cell_tex_coords_test();
    static void from_vtk_mesh_with_lines_test();
    static void from_vtk_mesh_with_quads_test();
    static void from_vtk_mesh_with_cell_tex_coords_test();
    static void from_vtk_mesh_with3_components_cell_colors_test();
};

} // namespace sight::io::vtk::ut
