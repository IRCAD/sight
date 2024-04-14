/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "dicom_surface_test.hpp"

#include <data/mesh.hpp>
#include <data/reconstruction.hpp>

#include <io/dicom/container/dicom_surface.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::dicom::container::ut::dicom_surface_test);

namespace sight::io::dicom::container::ut
{

//------------------------------------------------------------------------------

void dicom_surface_test::basic_test()
{
    auto mesh = std::make_shared<data::mesh>();
    mesh->reserve(4, 3, data::mesh::cell_type_t::triangle, data::mesh::attribute::point_normals);
    {
        auto mesh_lock = mesh->dump_lock();
        mesh->push_point(0, 1, 2);
        mesh->push_point(3, 4, 5);
        mesh->push_point(6, 7, 8);
        mesh->push_point(9, 10, 11);
        mesh->set_point_normal(0, 0, 1, 2);
        mesh->set_point_normal(1, 3, 4, 5);
        mesh->set_point_normal(2, 6, 7, 8);
        mesh->set_point_normal(3, 9, 10, 11);
        mesh->push_cell(0, 1, 2);
        mesh->push_cell(3, 0, 1);
        mesh->push_cell(2, 3, 0);
    }
    auto reconstruction = std::make_shared<data::reconstruction>();
    reconstruction->set_mesh(mesh);
    auto mesh_lock = mesh->dump_lock(); // TODO: Fix failure because mesh isn't dump locked.
    dicom_surface dicom_surface(reconstruction);

    CPPUNIT_ASSERT_EQUAL(std::size_t(4 * 3), dicom_surface.get_point_buffer_size());
    std::uint8_t i = 0;
    for(float e : dicom_surface.get_point_buffer())
    {
        CPPUNIT_ASSERT_EQUAL(float(i), e);
        i++;
    }

    CPPUNIT_ASSERT_EQUAL(std::size_t(4 * 3), dicom_surface.get_normal_buffer_size());
    i = 0;
    for(float e : dicom_surface.get_normal_buffer())
    {
        CPPUNIT_ASSERT_EQUAL(float(i), e);
        i++;
    }

    CPPUNIT_ASSERT_EQUAL(std::size_t(3 * 3), dicom_surface.get_cell_buffer_size());
    i = 0;
    for(std::uint32_t e : dicom_surface.get_cell_buffer())
    {
        // Indices start a 1 in DICOM, hence the "+ 1".
        CPPUNIT_ASSERT_EQUAL_MESSAGE("i=" + std::to_string(i), i % 4U + 1, e);
        i++;
    }

    CPPUNIT_ASSERT(*mesh == *dicom_surface.convert_to_data());
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::container::ut
