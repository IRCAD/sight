/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "mesh_test.hpp"

#include <core/profiling.hpp>

#include <data/mesh.hpp>

#include <utest_data/generator/mesh.hpp>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <tuple>
#include <utility>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::mesh_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void mesh_test::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void mesh_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void mesh_test::insertion()
{
    {
        data::mesh::sptr mesh = std::make_shared<data::mesh>();
        const auto lock       = mesh->dump_lock();
        mesh->push_point(10, 20, 30);
        mesh->push_point(10, 10, 10);
        mesh->push_point(20, 21, 10);
        mesh->push_point(30, 30, 10);
        mesh->push_point(15, 20, 35);
        mesh->push_point(20, 20, 10);
        mesh->push_point(20, 63, 17);
        mesh->push_point(27, 83, 52);

        mesh->push_cell(1, 2, 3);
        mesh->push_cell(1, 3, 4);
        mesh->push_cell(1, 2, 5);

        std::array<data::mesh::point_t, 3> p = {3, 6, 5};
        mesh->push_cell(std::vector(p.begin(), p.end()));

        std::array<data::mesh::point_t, 3> p2 = {1, 3, 5};
        mesh->push_cell(std::vector(p2.begin(), p2.end()));

        std::array<data::mesh::point_t, 3> p3 = {7, 2, 5};
        mesh->push_cell(std::vector(p3.begin(), p3.end()));

        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(8), mesh->num_points());
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(6), mesh->num_cells());

        auto it = mesh->cbegin<data::iterator::point::xyz>();
        CPPUNIT_ASSERT_EQUAL(10.F, it->x);
        CPPUNIT_ASSERT_EQUAL(20.F, it->y);
        CPPUNIT_ASSERT_EQUAL(30.F, it->z);
        it += 2;
        CPPUNIT_ASSERT_EQUAL(20.F, it->x);
        CPPUNIT_ASSERT_EQUAL(21.F, it->y);
        CPPUNIT_ASSERT_EQUAL(10.F, it->z);
        it += 5;
        CPPUNIT_ASSERT_EQUAL(27.F, it->x);
        CPPUNIT_ASSERT_EQUAL(83.F, it->y);
        CPPUNIT_ASSERT_EQUAL(52.F, it->z);
        --it;
        CPPUNIT_ASSERT_EQUAL(20.F, it->x);
        CPPUNIT_ASSERT_EQUAL(63.F, it->y);
        CPPUNIT_ASSERT_EQUAL(17.F, it->z);
        it -= 2;
        CPPUNIT_ASSERT_EQUAL(15.F, it->x);
        CPPUNIT_ASSERT_EQUAL(20.F, it->y);
        CPPUNIT_ASSERT_EQUAL(35.F, it->z);

        auto cell_it = mesh->cbegin<data::iterator::cell::triangle>();
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(1), cell_it->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(2), cell_it->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(3), cell_it->pt[2]);
        ++cell_it;

        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(1), cell_it->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(3), cell_it->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(4), cell_it->pt[2]);

        cell_it += 2;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(3), cell_it->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(6), cell_it->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(5), cell_it->pt[2]);

        cell_it++;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(1), cell_it->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(3), cell_it->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(5), cell_it->pt[2]);

        cell_it += 1;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(7), cell_it->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(2), cell_it->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(5), cell_it->pt[2]);

        --cell_it;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(1), cell_it->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(3), cell_it->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(5), cell_it->pt[2]);

        cell_it--;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(3), cell_it->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(6), cell_it->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(5), cell_it->pt[2]);
        cell_it -= 2;
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(1), cell_it->pt[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(3), cell_it->pt[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::point_t>(4), cell_it->pt[2]);

        mesh->set_point(4, 45, 59, 48);
        auto it_pt = mesh->begin<data::iterator::point::xyz>();
        it_pt += 4;
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(45), it_pt->x);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(59), it_pt->y);
        CPPUNIT_ASSERT_EQUAL(static_cast<float>(48), it_pt->z);
    }

    {
        const data::mesh::size_t nb_points      = 60;
        const data::mesh::size_t nb_cells       = 59;
        const data::mesh::cell_type_t cell_type = data::mesh::cell_type_t::triangle;
        const data::mesh::attribute extra_array =
            data::mesh::attribute::point_normals
            | data::mesh::attribute::point_colors
            | data::mesh::attribute::point_tex_coords
            | data::mesh::attribute::cell_normals
            | data::mesh::attribute::cell_colors
            | data::mesh::attribute::cell_tex_coords;

        data::mesh::sptr mesh = std::make_shared<data::mesh>();

        mesh->reserve(nb_points, nb_cells, cell_type, extra_array);
        const auto lock = mesh->dump_lock();

        for(std::size_t i = 0 ; i < nb_points ; ++i)
        {
            const auto val                                         = static_cast<uint8_t>(i);
            const std::array<data::mesh::color_t, 4> color         = {val, val, val, val};
            const auto float_val                                   = static_cast<float>(i);
            const std::array<data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
            const std::array<data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
            const std::size_t value                                = 3 * i;
            const auto id                                          = mesh->push_point(
                static_cast<float>(value),
                static_cast<float>(value + 1),
                static_cast<float>(value + 2)
            );
            mesh->set_point_color(id, color);
            mesh->set_point_normal(id, normal);
            mesh->set_point_tex_coord(id, tex_coords);
        }

        for(data::mesh::size_t i = 0 ; i < nb_cells ; ++i)
        {
            const auto id = mesh->push_cell(i, i + 1, i + 2);

            const auto val                                         = static_cast<data::mesh::color_t>(i);
            const std::array<data::mesh::color_t, 4> color         = {val, val, val, val};
            const auto float_val                                   = static_cast<float>(i);
            const std::array<data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
            const std::array<data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
            mesh->set_cell_color(id, color);
            mesh->set_cell_normal(id, normal);
            mesh->set_cell_tex_coord(id, tex_coords);
        }

        CPPUNIT_ASSERT_EQUAL(nb_points, mesh->num_points());
        CPPUNIT_ASSERT_EQUAL(nb_cells, mesh->num_cells());
        CPPUNIT_ASSERT_EQUAL(mesh->allocated_size_in_bytes(), mesh->size_in_bytes());
        const bool resize_memory = mesh->shrink_to_fit();
        CPPUNIT_ASSERT_EQUAL(false, resize_memory);
    }
    {
        const data::mesh::size_t nb_points = 1;
        const data::mesh::size_t nb_cells  = 1;

        const data::mesh::cell_type_t cell_type = data::mesh::cell_type_t::point;
        const data::mesh::attribute extra_array =
            data::mesh::attribute::point_normals
            | data::mesh::attribute::point_colors
            | data::mesh::attribute::point_tex_coords;

        data::mesh::sptr mesh = std::make_shared<data::mesh>();

        mesh->reserve(nb_points, nb_cells, cell_type, extra_array);
        const auto lock                                   = mesh->dump_lock();
        const std::size_t point_normals_allocated_size    = nb_points * 3LL * sizeof(data::mesh::normal_t);
        const std::size_t point_colors_allocated_size     = nb_points * 4LL * sizeof(data::mesh::color_t);
        const std::size_t point_tex_coords_allocated_size = nb_points * 2LL * sizeof(data::mesh::texcoord_t);
        const std::size_t point_position_allocated_size   = nb_points * 3LL * sizeof(data::mesh::position_t);

        // data::mesh::cell_type_t::POINT;
        const std::size_t cell_data_allocated_size = nb_cells * 1LL * sizeof(data::mesh::cell_t);

        const std::size_t point_size = point_position_allocated_size + cell_data_allocated_size
                                       + point_normals_allocated_size + point_colors_allocated_size
                                       + point_tex_coords_allocated_size;

        CPPUNIT_ASSERT_EQUAL(
            point_size,
            mesh->allocated_size_in_bytes()
        );
        for(std::size_t i = 0 ; i < 1001 ; ++i)
        {
            const auto val                                         = static_cast<uint8_t>(i);
            const std::array<data::mesh::color_t, 4> color         = {val, val, val, val};
            const auto float_val                                   = static_cast<float>(i);
            const std::array<data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
            const std::array<data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
            const std::size_t value                                = 3 * i;
            const auto id                                          = mesh->push_point(
                static_cast<float>(value),
                static_cast<float>(value + 1),
                static_cast<float>(value + 2)
            );
            mesh->set_point_color(id, color);
            mesh->set_point_normal(id, normal);
            mesh->set_point_tex_coord(id, tex_coords);
        }

        namespace point = iterator::point;
        {
            auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

            data::mesh::size_t count = 0;
            for(const auto& [p, n, c, uv] : range)
            {
                const auto f_value = static_cast<float>(3 * count);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_value, p.x, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_value + 1, p.y, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_value + 2, p.z, EPSILON);
                const auto c_val = static_cast<data::mesh::color_t>(count);
                CPPUNIT_ASSERT_EQUAL(c_val, c.r);
                CPPUNIT_ASSERT_EQUAL(c_val, c.g);
                CPPUNIT_ASSERT_EQUAL(c_val, c.b);
                CPPUNIT_ASSERT_EQUAL(c_val, c.a);
                const auto f_val = static_cast<data::mesh::normal_t>(count);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, n.nx, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, n.ny, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, n.nz, EPSILON);

                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, uv.u, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, uv.v, EPSILON);

                ++count;
            }

            CPPUNIT_ASSERT_EQUAL(mesh->num_points(), count);
        }

        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36040), mesh->allocated_size_in_bytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36036), mesh->size_in_bytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(0), mesh->num_cells());
        const auto idx = mesh->push_point(0.0, 0.0, 0.0);
        mesh->set_point_color(idx, 255, 0, 0, 255);
        mesh->set_point_normal(idx, 2.0, 1.0, 3.0);
        mesh->set_point_tex_coord(idx, 3.0, 1.0);
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(72040), mesh->allocated_size_in_bytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36072), mesh->size_in_bytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(1002), mesh->num_points());
        CPPUNIT_ASSERT_EQUAL(extra_array, mesh->attributes());
        CPPUNIT_ASSERT(mesh->has<data::mesh::attribute::point_colors>());
        CPPUNIT_ASSERT(mesh->has<data::mesh::attribute::point_normals>());
        CPPUNIT_ASSERT(mesh->has<data::mesh::attribute::point_tex_coords>());
    }

    {
        const data::mesh::size_t nb_points           = 1;
        const data::mesh::size_t nb_cells            = 1;
        const data::mesh::cell_type_t cell_typ_e1    = data::mesh::cell_type_t::triangle;
        const data::mesh::attribute attributes_array =
            data::mesh::attribute::cell_normals
            | data::mesh::attribute::cell_colors
            | data::mesh::attribute::cell_tex_coords;
        data::mesh::sptr mesh = std::make_shared<data::mesh>();
        mesh->reserve(nb_points, nb_cells, cell_typ_e1, attributes_array);
        const auto lock                                   = mesh->dump_lock();
        const std::size_t point_normals_allocated_size    = nb_points * 3LL * sizeof(data::mesh::normal_t);
        const std::size_t point_colors_allocated_size     = nb_points * 4LL * sizeof(data::mesh::color_t);
        const std::size_t point_tex_coords_allocated_size = nb_points * 2LL * sizeof(data::mesh::texcoord_t);
        const std::size_t point_position_allocated_size   = nb_points * 3LL * sizeof(data::mesh::position_t);

        // data::mesh::cell_type_t::triangle;
        const std::size_t cell_data_allocated_size = nb_cells * 3LL * sizeof(data::mesh::cell_t);

        const std::size_t point_size = point_position_allocated_size + cell_data_allocated_size
                                       + point_normals_allocated_size + point_colors_allocated_size
                                       + point_tex_coords_allocated_size;

        CPPUNIT_ASSERT_EQUAL(
            point_size,
            mesh->allocated_size_in_bytes()
        );

        for(data::mesh::size_t i = 0 ; i < 1001 ; ++i)
        {
            const auto id = mesh->push_cell(i, i + 1, i + 2);

            const auto val                                         = static_cast<data::mesh::color_t>(i);
            const std::array<data::mesh::color_t, 4> color         = {val, val, val, val};
            const auto float_val                                   = static_cast<float>(i);
            const std::array<data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
            const std::array<data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
            mesh->set_cell_color(id, color);
            mesh->set_cell_normal(id, normal);
            mesh->set_cell_tex_coord(id, tex_coords);
        }

        namespace cell = iterator::cell;
        {
            const auto range = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

            data::mesh::size_t count = 0;
            for(const auto& [p, n, c, uv] : range)
            {
                CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(count), p.pt[0]);
                CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(count + 1), p.pt[1]);
                CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(count + 2), p.pt[2]);

                const auto c_val = static_cast<data::mesh::color_t>(count);
                CPPUNIT_ASSERT_EQUAL(c_val, c.r);
                CPPUNIT_ASSERT_EQUAL(c_val, c.g);
                CPPUNIT_ASSERT_EQUAL(c_val, c.b);
                CPPUNIT_ASSERT_EQUAL(c_val, c.a);

                const auto f_val = static_cast<data::mesh::normal_t>(count);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, n.nx, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, n.ny, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, n.nz, EPSILON);

                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, uv.u, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL(f_val, uv.v, EPSILON);

                ++count;
            }

            CPPUNIT_ASSERT_EQUAL(mesh->num_cells(), count);
        }

        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36048), mesh->allocated_size_in_bytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(36036), mesh->size_in_bytes());
        CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(0), mesh->num_points());

        CPPUNIT_ASSERT_EQUAL(
            data::mesh::attribute::cell_normals
            | data::mesh::attribute::cell_colors
            | data::mesh::attribute::cell_tex_coords,
            mesh->attributes()
        );
        CPPUNIT_ASSERT_EQUAL(attributes_array, mesh->attributes());
        CPPUNIT_ASSERT(mesh->has<data::mesh::attribute::cell_colors>());
        CPPUNIT_ASSERT(mesh->has<data::mesh::attribute::cell_normals>());
        CPPUNIT_ASSERT(mesh->has<data::mesh::attribute::cell_tex_coords>());
    }
}

//-----------------------------------------------------------------------------

void mesh_test::copy()
{
    data::mesh::sptr mesh              = std::make_shared<data::mesh>();
    data::mesh::sptr shallow_copy_mesh = std::make_shared<data::mesh>();
    CPPUNIT_ASSERT(*mesh == *shallow_copy_mesh);

    const auto dump_lock = mesh->dump_lock();

    mesh->push_point(10, 20, 30);
    mesh->push_point(10, 10, 10);
    mesh->push_point(20, 20, 10);
    mesh->push_point(30, 30, 10);
    mesh->push_point(15, 20, 35);
    mesh->push_point(20, 20, 10);
    mesh->push_point(20, 63, 17);
    mesh->push_point(27, 83, 52);

    mesh->push_cell(1, 2, 0, 2);
    mesh->push_cell(1, 3, 4, 5);
    mesh->push_cell(1, 2, 5, 4);

    std::array<data::mesh::cell_t, 4> pt1 = {3, 6, 5, 1};
    mesh->push_cell(std::vector(pt1.begin(), pt1.end()));

    std::array<data::mesh::cell_t, 4> pt2 = {1, 3, 5, 7};
    mesh->push_cell(std::vector(pt2.begin(), pt2.end()));

    std::array<data::mesh::cell_t, 4> pt3 = {7, 2, 5, 4};
    mesh->push_cell(std::vector(pt3.begin(), pt3.end()));

    mesh->resize(
        mesh->num_points(),
        mesh->num_cells(),
        data::mesh::cell_type_t::quad,
        data::mesh::attribute::point_colors | data::mesh::attribute::point_tex_coords
    );

    for(data::mesh::size_t i = 0 ; i < mesh->num_points() ; ++i)
    {
        mesh->set_point_color(
            data::mesh::point_t(i),
            data::mesh::color_t(i + 1),
            data::mesh::color_t(i + 2),
            data::mesh::color_t(i + 3),
            data::mesh::color_t(i + 4)
        );

        mesh->set_point_tex_coord(
            data::mesh::point_t(i),
            1.0F / (data::mesh::texcoord_t(i) + 1.0F),
            1.0F / (data::mesh::texcoord_t(i) + 2.0F)
        );
    }

    namespace point = data::iterator::point;
    namespace cell  = data::iterator::cell;

    // check deep copy
    {
        data::mesh::sptr deep_copy_mesh = data::object::copy(mesh);
        const auto copy_dump_lock       = deep_copy_mesh->dump_lock();

        CPPUNIT_ASSERT(*mesh == *deep_copy_mesh);

        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), deep_copy_mesh->num_points());
        CPPUNIT_ASSERT_EQUAL(mesh->num_cells(), deep_copy_mesh->num_cells());
        CPPUNIT_ASSERT_EQUAL(mesh->size_in_bytes(), deep_copy_mesh->size_in_bytes());

        auto point_itr           = mesh->cbegin<point::xyz>();
        const auto point_end     = mesh->cend<point::xyz>();
        auto point_itr_copy_mesh = deep_copy_mesh->cbegin<point::xyz>();

        for( ; point_itr != point_end ; ++point_itr, ++point_itr_copy_mesh)
        {
            CPPUNIT_ASSERT_EQUAL(point_itr->x, point_itr_copy_mesh->x);
            CPPUNIT_ASSERT_EQUAL(point_itr->y, point_itr_copy_mesh->y);
            CPPUNIT_ASSERT_EQUAL(point_itr->z, point_itr_copy_mesh->z);
        }

        const sight::data::mesh::csptr mesh3 = mesh; // copy into a const pointer to explicit test const begin()
        auto cell_itr                        = mesh3->begin<cell::quad>();
        const auto cell_end                  = mesh3->end<cell::quad>();
        auto cell_itr_copy_mesh              = deep_copy_mesh->cbegin<cell::quad>();

        for( ; cell_itr != cell_end ; ++cell_itr, ++cell_itr_copy_mesh)
        {
            for(std::size_t i = 0 ; i < 4 ; ++i)
            {
                CPPUNIT_ASSERT_EQUAL(cell_itr->pt[i], cell_itr_copy_mesh->pt[i]);
            }
        }
    }

    //check shallow copy
    {
        shallow_copy_mesh->shallow_copy(mesh);
        CPPUNIT_ASSERT(*mesh == *shallow_copy_mesh);
        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), shallow_copy_mesh->num_points());
        CPPUNIT_ASSERT_EQUAL(mesh->num_cells(), shallow_copy_mesh->num_cells());
        CPPUNIT_ASSERT_EQUAL(mesh->size_in_bytes(), shallow_copy_mesh->size_in_bytes());

        {
            auto point_itr      = mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>().begin();
            auto point_itr_copy =
                shallow_copy_mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>().begin();

            const auto& [p1, n1, c1, u1] = *point_itr;
            const auto& [p2, n2, c2, u2] = *point_itr_copy;
            CPPUNIT_ASSERT_EQUAL(&p1.x, &p2.x);
            CPPUNIT_ASSERT_EQUAL(&n1.nx, &n2.nx);
            CPPUNIT_ASSERT_EQUAL(&c1.r, &c2.r);
            CPPUNIT_ASSERT_EQUAL(&u1.u, &u2.u);
        }

        {
            const auto cell_itr           = mesh->zip_range<cell::quad, cell::rgba, cell::uv>().begin();
            const auto cell_itr_copy_mesh = shallow_copy_mesh->zip_range<cell::quad, cell::rgba, cell::uv>().begin();

            const auto& [i1, c1, t1] = *cell_itr;
            const auto& [i2, c2, t2] = *cell_itr_copy_mesh;
            CPPUNIT_ASSERT_EQUAL((i1.pt).data(), (i2.pt).data());
            CPPUNIT_ASSERT_EQUAL(&c1.r, &c2.r);
            CPPUNIT_ASSERT_EQUAL(&t1.u, &t2.u);
        }
    }
}

//------------------------------------------------------------------------------

void mesh_test::allocation_test()
{
    data::mesh::sptr mesh = std::make_shared<data::mesh>();

    const auto lock = mesh->dump_lock();

    mesh->push_point(10, 20, 30);
    mesh->push_point(10, 10, 10);
    mesh->push_point(20, 20, 10);

    mesh->push_cell(0, 1, 2);
    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(3), mesh->num_points());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(1), mesh->num_cells());

    CPPUNIT_ASSERT_EQUAL(
        std::size_t(3LL * 3 * sizeof(data::mesh::position_t) + 3 * sizeof(data::mesh::cell_t)),
        mesh->size_in_bytes()
    );

    data::mesh::size_t point_size = 3000;
    data::mesh::size_t cell_size  = 2000;
    mesh->reserve(point_size, cell_size, data::mesh::cell_type_t::triangle);

    const std::size_t point_position_allocated_size = point_size * 3LL * sizeof(data::mesh::position_t);
    const std::size_t cell_data_allocated_size      = cell_size * 3LL * sizeof(data::mesh::cell_t);

    CPPUNIT_ASSERT_EQUAL(
        point_position_allocated_size + cell_data_allocated_size,
        mesh->allocated_size_in_bytes()
    );

    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(3), mesh->num_points());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(1), mesh->num_cells());

    CPPUNIT_ASSERT_EQUAL(data::mesh::attribute::none, mesh->attributes());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::point_colors>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::cell_colors>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::point_normals>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::cell_normals>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::point_tex_coords>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::cell_tex_coords>());

    mesh->reserve(
        point_size,
        cell_size,
        data::mesh::cell_type_t::triangle,
        data::mesh::attribute::point_normals
        | data::mesh::attribute::point_colors | data::mesh::attribute::point_tex_coords
    );

    const std::size_t point_normals_allocated_size    = point_size * 3LL * sizeof(data::mesh::normal_t);
    const std::size_t point_colors_allocated_size     = point_size * 4LL * sizeof(data::mesh::color_t);
    const std::size_t point_tex_coords_allocated_size = point_size * 2LL * sizeof(data::mesh::texcoord_t);
    CPPUNIT_ASSERT_EQUAL(
        point_position_allocated_size + cell_data_allocated_size
        + point_normals_allocated_size + point_colors_allocated_size + point_tex_coords_allocated_size,
        mesh->allocated_size_in_bytes()
    );

    CPPUNIT_ASSERT_EQUAL(
        data::mesh::attribute::point_normals
        | data::mesh::attribute::point_colors | data::mesh::attribute::point_tex_coords,
        mesh->attributes()
    );
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::mesh::attribute::point_colors>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::cell_colors>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::mesh::attribute::point_normals>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::cell_normals>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::mesh::attribute::point_tex_coords>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::cell_tex_coords>());

    mesh->reserve(
        point_size,
        cell_size,
        data::mesh::cell_type_t::triangle,
        data::mesh::attribute::point_normals
        | data::mesh::attribute::point_colors
        | data::mesh::attribute::point_tex_coords
        | data::mesh::attribute::cell_normals
        | data::mesh::attribute::cell_colors
        | data::mesh::attribute::cell_tex_coords
    );

    std::size_t oldsize  = mesh->allocated_size_in_bytes();
    bool adjusted        = mesh->shrink_to_fit();
    std::size_t new_size = mesh->allocated_size_in_bytes();

    CPPUNIT_ASSERT_EQUAL(true, adjusted);
    CPPUNIT_ASSERT(oldsize > new_size);

    CPPUNIT_ASSERT_EQUAL(
        data::mesh::attribute::point_normals
        | data::mesh::attribute::point_colors
        | data::mesh::attribute::point_tex_coords
        | data::mesh::attribute::cell_normals
        | data::mesh::attribute::cell_colors
        | data::mesh::attribute::cell_tex_coords,
        mesh->attributes()
    );
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::mesh::attribute::point_colors>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::mesh::attribute::cell_colors>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::mesh::attribute::point_normals>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::mesh::attribute::cell_normals>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::mesh::attribute::point_tex_coords>());
    CPPUNIT_ASSERT_EQUAL(true, mesh->has<data::mesh::attribute::cell_tex_coords>());

    mesh->clear();
    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(0), mesh->num_points());
    CPPUNIT_ASSERT_EQUAL(static_cast<data::mesh::size_t>(0), mesh->num_cells());
    CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), mesh->size_in_bytes());

    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::point_colors>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::cell_colors>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::point_normals>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::cell_normals>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::point_tex_coords>());
    CPPUNIT_ASSERT_EQUAL(false, mesh->has<data::mesh::attribute::cell_tex_coords>());
}

//------------------------------------------------------------------------------

void mesh_test::iterator_test()
{
    auto msg = [](auto& _count){return "iteration: " + std::to_string(_count);};

    const data::mesh::size_t nb_points      = 60;
    const data::mesh::size_t nb_cells       = 59;
    const data::mesh::cell_type_t cell_type = data::mesh::cell_type_t::triangle;
    const data::mesh::attribute extra_array =
        data::mesh::attribute::point_normals
        | data::mesh::attribute::point_colors
        | data::mesh::attribute::point_tex_coords
        | data::mesh::attribute::cell_normals
        | data::mesh::attribute::cell_colors
        | data::mesh::attribute::cell_tex_coords;

    data::mesh::sptr mesh = std::make_shared<data::mesh>();

    mesh->resize(nb_points, nb_cells, cell_type, extra_array);
    const auto lock = mesh->dump_lock();

    for(data::mesh::size_t i = 0 ; i < nb_points ; ++i)
    {
        const auto val                                         = static_cast<uint8_t>(i);
        const std::array<data::mesh::color_t, 4> color         = {val, val, val, val};
        const auto float_val                                   = static_cast<float>(i);
        const std::array<data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
        const std::array<data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
        const std::size_t value                                = std::size_t(3) * i;
        mesh->set_point(i, static_cast<float>(value), static_cast<float>(value + 1), static_cast<float>(value + 2));
        mesh->set_point_color(i, color);
        mesh->set_point_normal(i, normal);
        mesh->set_point_tex_coord(i, tex_coords);
    }

    for(data::mesh::size_t i = 0 ; i < nb_cells ; ++i)
    {
        mesh->set_cell(i, i, i + 1, i + 2);

        const auto val                                         = static_cast<data::mesh::color_t>(i);
        const std::array<data::mesh::color_t, 4> color         = {val, val, val, val};
        const auto float_val                                   = static_cast<float>(i);
        const std::array<data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
        const std::array<data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
        mesh->set_cell_color(i, color);
        mesh->set_cell_normal(i, normal);
        mesh->set_cell_tex_coord(i, tex_coords);
    }

    namespace point = iterator::point;
    namespace cell  = iterator::cell;
    {
        auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

        data::mesh::size_t count = 0;
        for(const auto& [p, n, c, uv] : range)
        {
            const auto f_value = static_cast<float>(3 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(f_value, p.x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(f_value + 1, p.y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(f_value + 2, p.z, EPSILON);

            const auto c_val = static_cast<data::mesh::color_t>(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.a);

            const auto f_val = static_cast<data::mesh::normal_t>(count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nz, EPSILON);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), count);
    }

    {
        const sight::data::mesh::csptr mesh3 = mesh; // copy into a const pointer to explicit test const begin()
        auto range                           = mesh3->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>();
        auto begin                           = std::make_reverse_iterator(range.end());
        auto end                             = std::make_reverse_iterator(range.begin());

        std::size_t count = nb_points;
        std::for_each(
            begin,
            end,
            [&](const auto& _it)
            {
                const auto& [p, n, c, uv] = _it;
                --count;
                const auto f_value = static_cast<float>(3 * count);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_value, p.x, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_value + 1, p.y, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_value + 2, p.z, EPSILON);

                const auto c_val = static_cast<data::mesh::color_t>(count);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.r);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.g);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.b);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.a);

                const auto f_val = static_cast<data::mesh::normal_t>(count);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nx, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.ny, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nz, EPSILON);

                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.u, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.v, EPSILON);
            });

        CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(0), count);
    }

    {
        const auto range = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

        data::mesh::size_t count = 0;
        for(const auto& [p, n, c, uv] : range)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::mesh::size_t>(count), p.pt[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::mesh::size_t>(count + 1), p.pt[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::mesh::size_t>(count + 2), p.pt[2]);

            const auto c_val = static_cast<data::mesh::color_t>(count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.a);

            const auto f_val = static_cast<data::mesh::normal_t>(count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nz, EPSILON);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_cells(), count);
    }

    {
        auto range = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();
        auto begin = std::make_reverse_iterator(range.end());
        auto end   = std::make_reverse_iterator(range.begin());

        data::mesh::size_t count = 0;
        std::for_each(
            begin,
            end,
            [&](const auto& _it)
            {
                const auto& [p, n, c, uv] = _it;

                CPPUNIT_ASSERT_EQUAL_MESSAGE(
                    msg(count),
                    static_cast<data::mesh::size_t>(nb_cells - 1 - count),
                    p.pt[0]
                );
                CPPUNIT_ASSERT_EQUAL_MESSAGE(
                    msg(count),
                    static_cast<data::mesh::size_t>(nb_cells - 1 - count + 1),
                    p.pt[1]
                );
                CPPUNIT_ASSERT_EQUAL_MESSAGE(
                    msg(count),
                    static_cast<data::mesh::size_t>(nb_cells - 1 - count + 2),
                    p.pt[2]
                );

                const auto c_val = static_cast<data::mesh::color_t>(nb_cells - 1 - count);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.r);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.g);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.b);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.a);

                const auto f_val = static_cast<data::mesh::normal_t>(nb_cells - 1 - count);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nx, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.ny, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nz, EPSILON);

                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.u, EPSILON);
                CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.v, EPSILON);
                ++count;
            });

        CPPUNIT_ASSERT_EQUAL(mesh->num_cells(), count);
    }

    data::mesh::csptr mesh2 = data::mesh::copy(mesh);
    const auto lock2        = mesh2->dump_lock();
    {
        const auto range = mesh2->crange<point::xyz>();

        float count = 0;
        for(const auto& point : range)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(count++, point.x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(count++, point.y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(count++, point.z, EPSILON);
        }
    }

    {
        auto range     = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();
        auto it        = std::make_reverse_iterator(range.end());
        const auto end = std::make_reverse_iterator(range.begin());

        data::mesh::size_t count = 0;
        for( ; it != end ; ++it)
        {
            const auto& [p, n, c, uv] = *it;
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                msg(count),
                static_cast<data::mesh::size_t>(nb_cells - 1 - count),
                p.pt[0]
            );
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                msg(count),
                static_cast<data::mesh::size_t>(nb_cells - 1 - count + 1),
                p.pt[1]
            );
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                msg(count),
                static_cast<data::mesh::size_t>(nb_cells - 1 - count + 2),
                p.pt[2]
            );

            const auto c_val = static_cast<data::mesh::color_t>(nb_cells - 1 - count);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c_val, c.a);

            const auto f_val =
                static_cast<data::mesh::normal_t>(nb_cells - 1 - count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, n.nz, EPSILON);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), f_val, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_cells(), count);
    }

    data::mesh::sptr mesh3 = std::make_shared<data::mesh>();
    const auto lock3       = mesh3->dump_lock();
    mesh3->resize(nb_points, nb_cells, data::mesh::cell_type_t::quad, extra_array);

    {
        auto range = mesh3->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

        uint32_t count = 0;
        for(auto&& [p, n, c, uv] : range)
        {
            p.x = static_cast<float>(3 * count);
            p.y = static_cast<float>(3 * count + 1);
            p.z = static_cast<float>(3 * count + 2);

            c.r = static_cast<std::uint8_t>(4 * count);
            c.g = static_cast<std::uint8_t>(4 * count + 1);
            c.b = static_cast<std::uint8_t>(4 * count + 2);
            c.a = static_cast<std::uint8_t>(4 * count + 3);

            n.nx = static_cast<float>(3 * count + 1);
            n.ny = static_cast<float>(3 * count + 2);
            n.nz = static_cast<float>(3 * count + 3);

            uv.u = static_cast<float>(2 * count);
            uv.v = static_cast<float>(2 * count + 1);
            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), count);
    }

    {
        auto range = mesh3->zip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

        data::mesh::size_t count = 0;
        for(auto&& [p, n, c, uv] : range)
        {
            for(std::uint8_t i = 0 ; i < 3 ; ++i)
            {
                p.pt[i] = count + i;
            }

            c.r = static_cast<std::uint8_t>(4 * count);
            c.g = static_cast<std::uint8_t>(4 * count + 1);
            c.b = static_cast<std::uint8_t>(4 * count + 2);
            c.a = static_cast<std::uint8_t>(4 * count + 3);

            n.nx = static_cast<float>(3 * count + 1);
            n.ny = static_cast<float>(3 * count + 2);
            n.nz = static_cast<float>(3 * count + 3);

            uv.u = static_cast<float>(2 * count);
            uv.v = static_cast<float>(2 * count + 1);
            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_cells(), count);
    }

    {
        auto range = mesh3->czip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

        data::mesh::size_t count = 0;
        for(const auto& [p, n, c, uv] : range)
        {
            const auto f_value = static_cast<float>(3 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(f_value, p.x, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(f_value + 1, p.y, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(f_value + 2, p.z, EPSILON);

            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count, static_cast<data::mesh::size_t>(c.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 1, static_cast<data::mesh::size_t>(c.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 2, static_cast<data::mesh::size_t>(c.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 3, static_cast<data::mesh::size_t>(c.a));

            const auto n_val = static_cast<data::mesh::normal_t>(3 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n_val + 1, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n_val + 2, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n_val + 3, n.nz, EPSILON);

            const auto uv_val = static_cast<data::mesh::normal_t>(2 * count);

            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uv_val, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uv_val + 1, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), count);
    }

    {
        auto range = mesh3->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

        data::mesh::size_t count = 0;
        for(const auto& [p, n, c, uv] : range)
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::mesh::size_t>(count), p.pt[0]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::mesh::size_t>(count + 1), p.pt[1]);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<data::mesh::size_t>(count + 2), p.pt[2]);

            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count, static_cast<data::mesh::size_t>(c.r));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 1, static_cast<data::mesh::size_t>(c.g));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 2, static_cast<data::mesh::size_t>(c.b));
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), 4 * count + 3, static_cast<data::mesh::size_t>(c.a));

            const auto n_val = static_cast<data::mesh::normal_t>(3 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n_val + 1, n.nx, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n_val + 2, n.ny, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), n_val + 3, n.nz, EPSILON);

            const auto uv_val = static_cast<data::mesh::normal_t>(2 * count);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uv_val, uv.u, EPSILON);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), uv_val + 1, uv.v, EPSILON);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_cells(), count);
    }
}

//------------------------------------------------------------------------------

void mesh_test::iterator_copy_test()
{
    auto msg = [](auto& _count){return "iteration: " + std::to_string(_count);};

    data::mesh::sptr mesh = std::make_shared<data::mesh>();
    mesh->resize(
        10,
        10,
        data::mesh::cell_type_t::point,
        data::mesh::attribute::point_normals
        | data::mesh::attribute::point_colors
        | data::mesh::attribute::point_tex_coords
        | data::mesh::attribute::cell_normals
        | data::mesh::attribute::cell_colors
        | data::mesh::attribute::cell_tex_coords
    );

    const auto lock = mesh->dump_lock();

    namespace point = iterator::point;
    namespace cell  = iterator::cell;
    {
        //fill the mesh
        auto range = mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point, cell::nxyz,
                                     cell::rgba, cell::uv>();

        data::mesh::size_t count = 0;
        for(auto&& [p, n, c, uv, cell, cn, cc, cuv] : range)
        {
            // point
            p.x = static_cast<float>(count);
            p.y = static_cast<float>(count) + 0.6F;
            p.z = static_cast<float>(count) + 0.8F;

            n.nx = static_cast<float>(count) + 0.1F;
            n.ny = static_cast<float>(count) + 0.2F;
            n.nz = static_cast<float>(count) + 0.3F;

            c.r = static_cast<std::uint8_t>(4 * count);
            c.g = static_cast<std::uint8_t>(4 * count + 1);
            c.b = static_cast<std::uint8_t>(4 * count + 2);
            c.a = static_cast<std::uint8_t>(4 * count + 3);

            uv.u = static_cast<float>(count) + 0.7F;
            uv.v = static_cast<float>(count) + 0.9F;

            // cell
            cell.pt = count;

            cn.nx = static_cast<float>(count) + 0.11F;
            cn.ny = static_cast<float>(count) + 0.22F;
            cn.nz = static_cast<float>(count) + 0.33F;

            cc.r = static_cast<std::uint8_t>(4 * count + 1);
            cc.g = static_cast<std::uint8_t>(4 * count + 2);
            cc.b = static_cast<std::uint8_t>(4 * count + 3);
            cc.a = static_cast<std::uint8_t>(4 * count + 4);

            cuv.u = static_cast<float>(count) + 0.77F;
            cuv.v = static_cast<float>(count) + 0.99F;

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), count);
    }

    {
        // check the mesh points
        auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point, cell::nxyz,
                                      cell::rgba, cell::uv>();

        data::mesh::size_t count = 0;
        for(const auto& [p, n, c, uv, cell, cn, cc, cuv] : range)
        {
            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count), p.x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.6F, p.y, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.8F, p.z, 0.00001);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.1F, n.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.2F, n.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.3F, n.nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count), c.r);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 1), c.g);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 2), c.b);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 3), c.a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.7, uv.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.9, uv.v, 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL(count, cell.pt);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.11F, cn.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.22F, cn.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.33F, cn.nz, 0.00001);

            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 1), cc.r);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 2), cc.g);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 3), cc.b);
            CPPUNIT_ASSERT_EQUAL(static_cast<std::uint8_t>(4 * count + 4), cc.a);

            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.77, cuv.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(static_cast<float>(count) + 0.99, cuv.v, 0.00001);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), count);
    }

    data::mesh::sptr copied_mesh = std::make_shared<data::mesh>();
    CPPUNIT_ASSERT(*mesh != *copied_mesh);

    copied_mesh->resize(
        10,
        10,
        data::mesh::cell_type_t::point,
        data::mesh::attribute::point_normals
        | data::mesh::attribute::point_colors
        | data::mesh::attribute::point_tex_coords
        | data::mesh::attribute::cell_normals
        | data::mesh::attribute::cell_colors
        | data::mesh::attribute::cell_tex_coords
    );
    const auto lock2 = copied_mesh->dump_lock();
    {
        // copy the mesh points and cells with the iterator
        auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                      cell::point, cell::nxyz, cell::rgba, cell::uv>();
        auto range_copy = copied_mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                                 cell::point, cell::nxyz, cell::rgba, cell::uv>();

        std::copy(range.begin(), range.end(), range_copy.begin());
    }

    CPPUNIT_ASSERT(*mesh == *copied_mesh);

    {
        // check the copied mesh
        auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point,
                                      cell::nxyz, cell::rgba, cell::uv>();
        auto range_copy = copied_mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point,
                                                  cell::nxyz, cell::rgba, cell::uv>();
        auto combined_range = boost::combine(range, range_copy);

        data::mesh::size_t count = 0;
        for(const auto& [pt1, pt2] : combined_range)
        {
            const auto& [p1, n1, c1, uv1, cell1, cn1, cc1, cuv1] = pt1;
            const auto& [p2, n2, c2, uv2, cell2, cn2, cc2, cuv2] = pt2;

            // point
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), p1.x, p2.x);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), p1.y, p2.y);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), p1.z, p2.z);

            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), n1.nx, n2.nx);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), n1.ny, n2.ny);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), n1.nz, n2.nz);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.r, c2.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.g, c2.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.b, c2.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.a, c2.a);

            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), uv1.u, uv2.u);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), uv1.v, uv2.v);

            // cell
            CPPUNIT_ASSERT_EQUAL(cell1.pt, cell2.pt);

            CPPUNIT_ASSERT_EQUAL(cn1.nx, cn2.nx);
            CPPUNIT_ASSERT_EQUAL(cn1.ny, cn2.ny);
            CPPUNIT_ASSERT_EQUAL(cn1.nz, cn2.nz);

            CPPUNIT_ASSERT_EQUAL(cc1.r, cc2.r);
            CPPUNIT_ASSERT_EQUAL(cc1.g, cc2.g);
            CPPUNIT_ASSERT_EQUAL(cc1.b, cc2.b);
            CPPUNIT_ASSERT_EQUAL(cc1.a, cc2.a);

            CPPUNIT_ASSERT_EQUAL(cuv1.u, cuv2.u);
            CPPUNIT_ASSERT_EQUAL(cuv1.v, cuv2.v);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), count);
    }

    {
        // check the mesh points are deep copied and not shallow copied

        auto range = mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                     cell::point, cell::nxyz, cell::rgba, cell::uv>();

        auto point = boost::make_tuple(
            data::iterator::point::xyz({1.0F, 1.1F, 1.2F}),
            data::iterator::point::nxyz({1.F, 0.F, 0.F}),
            data::iterator::point::rgba({25, 15, 18, 32}),
            data::iterator::point::uv({0.5F, 1.F}),
            data::iterator::cell::point({0}),
            data::iterator::cell::nxyz({0.F, 1.F, 0.F}),
            data::iterator::cell::rgba({20, 13, 10, 37}),
            data::iterator::cell::uv({0.2F, 0.8F})
        );
        std::fill(range.begin(), range.end(), point);

        // check the mesh points are filled
        data::mesh::size_t count = 0;
        const auto& [p2, n2, c2, uv2, cell2, cn2, cc2, cuv2] = point;

        for(const auto& [p1, n1, c1, uv1, cell1, cn1, cc1, cuv1] : range)
        {
            // point
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), p1.x, p2.x);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), p1.y, p2.y);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), p1.z, p2.z);

            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), n1.nx, n2.nx);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), n1.ny, n2.ny);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), n1.nz, n2.nz);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.r, c2.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.g, c2.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.b, c2.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), c1.a, c2.a);

            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), uv1.u, uv2.u);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), uv1.v, uv2.v);

            // cell
            CPPUNIT_ASSERT_EQUAL(cell1.pt, cell2.pt);

            CPPUNIT_ASSERT_EQUAL(cn1.nx, cn2.nx);
            CPPUNIT_ASSERT_EQUAL(cn1.ny, cn2.ny);
            CPPUNIT_ASSERT_EQUAL(cn1.nz, cn2.nz);

            CPPUNIT_ASSERT_EQUAL(cc1.r, cc2.r);
            CPPUNIT_ASSERT_EQUAL(cc1.g, cc2.g);
            CPPUNIT_ASSERT_EQUAL(cc1.b, cc2.b);
            CPPUNIT_ASSERT_EQUAL(cc1.a, cc2.a);

            CPPUNIT_ASSERT_EQUAL(cuv1.u, cuv2.u);
            CPPUNIT_ASSERT_EQUAL(cuv1.v, cuv2.v);

            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), count);

        auto range_copy = copied_mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                                  cell::point, cell::nxyz, cell::rgba, cell::uv>();

        count = 0;
        for(const auto& [p, n, c, uv, cell, cn, cc, cuv] : range_copy)
        {
            // point
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count), p.x, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.6F, p.y, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.8F, p.z, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.1F, n.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.2F, n.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.3F, n.nz, 0.00001);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count), c.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 1), c.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 2), c.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 3), c.a);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.7, uv.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.9, uv.v, 0.00001);

            // cell
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), count, cell.pt);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.11F, cn.nx, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.22F, cn.ny, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.33F, cn.nz, 0.00001);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 1), cc.r);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 2), cc.g);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 3), cc.b);
            CPPUNIT_ASSERT_EQUAL_MESSAGE(msg(count), static_cast<std::uint8_t>(4 * count + 4), cc.a);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.77, cuv.u, 0.00001);
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(msg(count), static_cast<float>(count) + 0.99, cuv.v, 0.00001);
            ++count;
        }

        CPPUNIT_ASSERT_EQUAL(mesh->num_points(), count);
    }
}

//------------------------------------------------------------------------------

void mesh_test::benchmark_iterator()
{
    data::mesh::sptr mesh = std::make_shared<data::mesh>();
    const auto lock       = mesh->dump_lock();

    static const std::size_t s_N = 1;

    mesh->resize(
        655360,
        10000,
        data::mesh::cell_type_t::triangle,
        data::mesh::attribute::point_normals
    );

    auto pos  = mesh->begin<iterator::point::xyz>();
    auto norm = mesh->begin<iterator::point::nxyz>();

    auto pos2  = mesh->end<iterator::point::xyz>();
    auto norm2 = mesh->end<iterator::point::nxyz>();

    auto fn2 = [](const auto& _tuple)
               {
                   auto& [p, n] = _tuple;
                   p.x          = std::intptr_t(&p) % std::intptr_t(&n) == 0 ? 5.F : 1.F;
                   p.y          = 2.F;
                   p.z          = 3.F;
                   n.nx         = -1.F;
                   n.ny         = -1.F;
                   n.nz         = 1.F;
               };

    auto fn3 = [](auto& _p)
               {
                   _p.x = std::intptr_t(&_p) % 2 == 0 ? 5.F : 1.F;
                   _p.y = 2.F;
                   _p.z = 3.F;
               };

    auto fn4 = [](auto& _n)
               {
                   _n.nx = std::intptr_t(&_n) % 2 == 0 ? 5.F : 1.F;
                   _n.ny = -1.F;
                   _n.nz = 1.F;
               };

    {
        FW_PROFILE("std::for_each - array_iterator (nouveau en MR)");
        for(std::size_t i = 0 ; i < s_N ; ++i)
        {
            std::for_each(pos, pos2, fn3);
            std::for_each(norm, norm2, fn4);
        }
    }

    auto range = mesh->zip_range<iterator::point::xyz, iterator::point::nxyz>();
    {
        FW_PROFILE("std::for_each - zip array_iterator");
        for(std::size_t i = 0 ; i < s_N ; ++i)
        {
            std::for_each(range.begin(), range.end(), fn2);
        }
    }

    std::vector<iterator::point::xyz> pos_v(655360);
    std::vector<iterator::point::nxyz> norm_v(655360);
    {
        auto z = boost::combine(pos_v, norm_v);
        {
            FW_PROFILE("std::for - std boost combine");
            for(std::size_t i = 0 ; i < s_N ; ++i)
            {
                for(auto&& [p, n] : z)
                {
                    p.x  = p.x > 2.F ? 5.F : 1.F;
                    p.y  = 2.F;
                    p.z  = 3.F;
                    n.nx = -1.F;
                    n.ny = -1.F;
                    n.nz = 1.F;
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void mesh_test::equality_test()
{
    auto mesh1 = std::make_shared<data::mesh>();
    auto mesh2 = std::make_shared<data::mesh>();

    CPPUNIT_ASSERT(*mesh1 == *mesh2 && !(*mesh1 != *mesh2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            mesh1->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Meshes should be different when applying " #op " to the first", \
                *mesh1 != *mesh2 && !(*mesh1 == *mesh2) \
            ); \
            mesh2->op; \
            CPPUNIT_ASSERT_MESSAGE( \
                "Meshes should be equal when " #op " to both", \
                *mesh1 == *mesh2 && !(*mesh1 != *mesh2) \
            );

    mesh1->resize(1, 1, data::mesh::cell_type_t::quad, data::mesh::attribute::none);
    CPPUNIT_ASSERT(*mesh1 != *mesh2 && !(*mesh1 == *mesh2));
    mesh2->resize(1, 1, data::mesh::cell_type_t::quad, data::mesh::attribute::none);
    auto lock1 = mesh1->dump_lock();
    auto lock2 = mesh2->dump_lock();
    mesh1->set_point(0, {1, 2, 3});
    mesh2->set_point(0, {1, 2, 3});
    mesh1->set_cell(0, {1, 2, 3, 4});
    mesh2->set_cell(0, {1, 2, 3, 4});
    CPPUNIT_ASSERT(*mesh1 == *mesh2 && !(*mesh1 != *mesh2));

    mesh1->resize(1, 1, data::mesh::cell_type_t::quad, data::mesh::attribute::point_colors);
    CPPUNIT_ASSERT(*mesh1 != *mesh2 && !(*mesh1 == *mesh2));
    mesh2->resize(1, 1, data::mesh::cell_type_t::quad, data::mesh::attribute::point_colors);
    TEST(set_point_color(0, {1, 2, 3}));

    TEST(resize(1, 1, data::mesh::cell_type_t::tetra, data::mesh::attribute::point_colors));
    TEST(resize(1, 1, data::mesh::cell_type_t::quad, data::mesh::attribute::none));

    #undef TEST
}

} // namespace sight::data::ut
