/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/profiling.hpp>

#include <data/mesh.hpp>

#include <utest_data/generator/mesh.hpp>

#include <boost/range/algorithm.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <iterator>
#include <vector>

static constexpr float EPSILON = std::numeric_limits<float>::epsilon();

TEST_SUITE("sight::data::mesh")
{
//------------------------------------------------------------------------------

    TEST_CASE("insertion")
    {
        {
            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            const auto lock              = mesh->dump_lock();
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

            std::array<sight::data::mesh::point_t, 3> p = {3, 6, 5};
            mesh->push_cell(std::vector(p.begin(), p.end()));

            std::array<sight::data::mesh::point_t, 3> p2 = {1, 3, 5};
            mesh->push_cell(std::vector(p2.begin(), p2.end()));

            std::array<sight::data::mesh::point_t, 3> p3 = {7, 2, 5};
            mesh->push_cell(std::vector(p3.begin(), p3.end()));

            CHECK_EQ(static_cast<sight::data::mesh::size_t>(8), mesh->num_points());
            CHECK_EQ(static_cast<sight::data::mesh::size_t>(6), mesh->num_cells());

            auto it = mesh->cbegin<sight::data::iterator::point::xyz>();
            CHECK_EQ(10.F, it->x);
            CHECK_EQ(20.F, it->y);
            CHECK_EQ(30.F, it->z);
            it += 2;
            CHECK_EQ(20.F, it->x);
            CHECK_EQ(21.F, it->y);
            CHECK_EQ(10.F, it->z);
            it += 5;
            CHECK_EQ(27.F, it->x);
            CHECK_EQ(83.F, it->y);
            CHECK_EQ(52.F, it->z);
            --it;
            CHECK_EQ(20.F, it->x);
            CHECK_EQ(63.F, it->y);
            CHECK_EQ(17.F, it->z);
            it -= 2;
            CHECK_EQ(15.F, it->x);
            CHECK_EQ(20.F, it->y);
            CHECK_EQ(35.F, it->z);

            auto cell_it = mesh->cbegin<sight::data::iterator::cell::triangle>();
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(1), cell_it->pt[0]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(2), cell_it->pt[1]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(3), cell_it->pt[2]);
            ++cell_it;

            CHECK_EQ(static_cast<sight::data::mesh::point_t>(1), cell_it->pt[0]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(3), cell_it->pt[1]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(4), cell_it->pt[2]);

            cell_it += 2;
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(3), cell_it->pt[0]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(6), cell_it->pt[1]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(5), cell_it->pt[2]);

            cell_it++;
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(1), cell_it->pt[0]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(3), cell_it->pt[1]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(5), cell_it->pt[2]);

            cell_it += 1;
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(7), cell_it->pt[0]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(2), cell_it->pt[1]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(5), cell_it->pt[2]);

            --cell_it;
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(1), cell_it->pt[0]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(3), cell_it->pt[1]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(5), cell_it->pt[2]);

            cell_it--;
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(3), cell_it->pt[0]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(6), cell_it->pt[1]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(5), cell_it->pt[2]);
            cell_it -= 2;
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(1), cell_it->pt[0]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(3), cell_it->pt[1]);
            CHECK_EQ(static_cast<sight::data::mesh::point_t>(4), cell_it->pt[2]);

            mesh->set_point(4, 45, 59, 48);
            auto it_pt = mesh->begin<sight::data::iterator::point::xyz>();
            it_pt += 4;
            CHECK_EQ(static_cast<float>(45), it_pt->x);
            CHECK_EQ(static_cast<float>(59), it_pt->y);
            CHECK_EQ(static_cast<float>(48), it_pt->z);
        }

        {
            const sight::data::mesh::size_t nb_points      = 60;
            const sight::data::mesh::size_t nb_cells       = 59;
            const sight::data::mesh::cell_type_t cell_type = sight::data::mesh::cell_type_t::triangle;
            const sight::data::mesh::attribute extra_array =
                sight::data::mesh::attribute::point_normals
                | sight::data::mesh::attribute::point_colors
                | sight::data::mesh::attribute::point_tex_coords
                | sight::data::mesh::attribute::cell_normals
                | sight::data::mesh::attribute::cell_colors
                | sight::data::mesh::attribute::cell_tex_coords;

            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

            mesh->reserve(nb_points, nb_cells, cell_type, extra_array);
            const auto lock = mesh->dump_lock();

            for(std::size_t i = 0 ; i < nb_points ; ++i)
            {
                const auto val                                                = static_cast<uint8_t>(i);
                const std::array<sight::data::mesh::color_t, 4> color         = {val, val, val, val};
                const auto float_val                                          = static_cast<float>(i);
                const std::array<sight::data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
                const std::array<sight::data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
                const std::size_t value                                       = 3 * i;
                const auto id                                                 = mesh->push_point(
                    static_cast<float>(value),
                    static_cast<float>(value + 1),
                    static_cast<float>(value + 2)
                );
                mesh->set_point_color(id, color);
                mesh->set_point_normal(id, normal);
                mesh->set_point_tex_coord(id, tex_coords);
            }

            for(sight::data::mesh::size_t i = 0 ; i < nb_cells ; ++i)
            {
                const auto id = mesh->push_cell(i, i + 1, i + 2);

                const auto val =
                    static_cast<sight::data::mesh::color_t>(i);
                const std::array<sight::data::mesh::color_t, 4> color         = {val, val, val, val};
                const auto float_val                                          = static_cast<float>(i);
                const std::array<sight::data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
                const std::array<sight::data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
                mesh->set_cell_color(id, color);
                mesh->set_cell_normal(id, normal);
                mesh->set_cell_tex_coord(id, tex_coords);
            }

            CHECK_EQ(nb_points, mesh->num_points());
            CHECK_EQ(nb_cells, mesh->num_cells());
            CHECK_EQ(mesh->allocated_size_in_bytes(), mesh->size_in_bytes());
            const bool resize_memory = mesh->shrink_to_fit();
            CHECK_EQ(false, resize_memory);
        }
        {
            const sight::data::mesh::size_t nb_points = 1;
            const sight::data::mesh::size_t nb_cells  = 1;

            const sight::data::mesh::cell_type_t cell_type = sight::data::mesh::cell_type_t::point;
            const sight::data::mesh::attribute extra_array =
                sight::data::mesh::attribute::point_normals
                | sight::data::mesh::attribute::point_colors
                | sight::data::mesh::attribute::point_tex_coords;

            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

            mesh->reserve(nb_points, nb_cells, cell_type, extra_array);
            const auto lock                                   = mesh->dump_lock();
            const std::size_t point_normals_allocated_size    = nb_points * 3LL * sizeof(sight::data::mesh::normal_t);
            const std::size_t point_colors_allocated_size     = nb_points * 4LL * sizeof(sight::data::mesh::color_t);
            const std::size_t point_tex_coords_allocated_size = nb_points * 2LL * sizeof(sight::data::mesh::texcoord_t);
            const std::size_t point_position_allocated_size   = nb_points * 3LL * sizeof(sight::data::mesh::position_t);

            // sight::data::mesh::cell_type_t::POINT;
            const std::size_t cell_data_allocated_size = nb_cells * 1LL * sizeof(sight::data::mesh::cell_t);

            const std::size_t point_size = point_position_allocated_size + cell_data_allocated_size
                                           + point_normals_allocated_size + point_colors_allocated_size
                                           + point_tex_coords_allocated_size;

            CHECK_EQ(
                point_size,
                mesh->allocated_size_in_bytes()
            );
            for(std::size_t i = 0 ; i < 1001 ; ++i)
            {
                const auto val                                                = static_cast<uint8_t>(i);
                const std::array<sight::data::mesh::color_t, 4> color         = {val, val, val, val};
                const auto float_val                                          = static_cast<float>(i);
                const std::array<sight::data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
                const std::array<sight::data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
                const std::size_t value                                       = 3 * i;
                const auto id                                                 = mesh->push_point(
                    static_cast<float>(value),
                    static_cast<float>(value + 1),
                    static_cast<float>(value + 2)
                );
                mesh->set_point_color(id, color);
                mesh->set_point_normal(id, normal);
                mesh->set_point_tex_coord(id, tex_coords);
            }

            namespace point = sight::data::iterator::point;
            {
                auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

                sight::data::mesh::size_t count = 0;
                for(const auto& [p, n, c, uv] : range)
                {
                    const auto f_value = static_cast<float>(3 * count);
                    CHECK_EQ(f_value, doctest::Approx(p.x).epsilon(EPSILON));
                    CHECK_EQ(f_value + 1, doctest::Approx(p.y).epsilon(EPSILON));
                    CHECK_EQ(f_value + 2, doctest::Approx(p.z).epsilon(EPSILON));
                    const auto c_val = static_cast<sight::data::mesh::color_t>(count);
                    CHECK_EQ(c_val, c.r);
                    CHECK_EQ(c_val, c.g);
                    CHECK_EQ(c_val, c.b);
                    CHECK_EQ(c_val, c.a);
                    const auto f_val = static_cast<sight::data::mesh::normal_t>(count);
                    CHECK_EQ(f_val, doctest::Approx(n.nx).epsilon(EPSILON));
                    CHECK_EQ(f_val, doctest::Approx(n.ny).epsilon(EPSILON));
                    CHECK_EQ(f_val, doctest::Approx(n.nz).epsilon(EPSILON));

                    CHECK_EQ(f_val, doctest::Approx(uv.u).epsilon(EPSILON));
                    CHECK_EQ(f_val, doctest::Approx(uv.v).epsilon(EPSILON));

                    ++count;
                }

                CHECK_EQ(mesh->num_points(), count);
            }

            CHECK_EQ(static_cast<std::size_t>(36040), mesh->allocated_size_in_bytes());
            CHECK_EQ(static_cast<std::size_t>(36036), mesh->size_in_bytes());
            CHECK_EQ(static_cast<sight::data::mesh::size_t>(0), mesh->num_cells());
            const auto idx = mesh->push_point(0.0, 0.0, 0.0);
            mesh->set_point_color(idx, 255, 0, 0, 255);
            mesh->set_point_normal(idx, 2.0, 1.0, 3.0);
            mesh->set_point_tex_coord(idx, 3.0, 1.0);
            CHECK_EQ(static_cast<std::size_t>(72040), mesh->allocated_size_in_bytes());
            CHECK_EQ(static_cast<std::size_t>(36072), mesh->size_in_bytes());
            CHECK_EQ(static_cast<sight::data::mesh::size_t>(1002), mesh->num_points());
            CHECK_EQ(extra_array, mesh->attributes());
            CHECK(mesh->has<sight::data::mesh::attribute::point_colors>());
            CHECK(mesh->has<sight::data::mesh::attribute::point_normals>());
            CHECK(mesh->has<sight::data::mesh::attribute::point_tex_coords>());
        }

        {
            const sight::data::mesh::size_t nb_points           = 1;
            const sight::data::mesh::size_t nb_cells            = 1;
            const sight::data::mesh::cell_type_t cell_typ_e1    = sight::data::mesh::cell_type_t::triangle;
            const sight::data::mesh::attribute attributes_array =
                sight::data::mesh::attribute::cell_normals
                | sight::data::mesh::attribute::cell_colors
                | sight::data::mesh::attribute::cell_tex_coords;
            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            mesh->reserve(nb_points, nb_cells, cell_typ_e1, attributes_array);
            const auto lock                                   = mesh->dump_lock();
            const std::size_t point_normals_allocated_size    = nb_points * 3LL * sizeof(sight::data::mesh::normal_t);
            const std::size_t point_colors_allocated_size     = nb_points * 4LL * sizeof(sight::data::mesh::color_t);
            const std::size_t point_tex_coords_allocated_size = nb_points * 2LL * sizeof(sight::data::mesh::texcoord_t);
            const std::size_t point_position_allocated_size   = nb_points * 3LL * sizeof(sight::data::mesh::position_t);

            // sight::data::mesh::cell_type_t::triangle;
            const std::size_t cell_data_allocated_size = nb_cells * 3LL * sizeof(sight::data::mesh::cell_t);

            const std::size_t point_size = point_position_allocated_size + cell_data_allocated_size
                                           + point_normals_allocated_size + point_colors_allocated_size
                                           + point_tex_coords_allocated_size;

            CHECK_EQ(
                point_size,
                mesh->allocated_size_in_bytes()
            );

            for(sight::data::mesh::size_t i = 0 ; i < 1001 ; ++i)
            {
                const auto id = mesh->push_cell(i, i + 1, i + 2);

                const auto val =
                    static_cast<sight::data::mesh::color_t>(i);
                const std::array<sight::data::mesh::color_t, 4> color         = {val, val, val, val};
                const auto float_val                                          = static_cast<float>(i);
                const std::array<sight::data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
                const std::array<sight::data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
                mesh->set_cell_color(id, color);
                mesh->set_cell_normal(id, normal);
                mesh->set_cell_tex_coord(id, tex_coords);
            }

            namespace cell = sight::data::iterator::cell;
            {
                const auto range = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

                sight::data::mesh::size_t count = 0;
                for(const auto& [p, n, c, uv] : range)
                {
                    CHECK_EQ(static_cast<sight::data::mesh::size_t>(count), p.pt[0]);
                    CHECK_EQ(static_cast<sight::data::mesh::size_t>(count + 1), p.pt[1]);
                    CHECK_EQ(static_cast<sight::data::mesh::size_t>(count + 2), p.pt[2]);

                    const auto c_val = static_cast<sight::data::mesh::color_t>(count);
                    CHECK_EQ(c_val, c.r);
                    CHECK_EQ(c_val, c.g);
                    CHECK_EQ(c_val, c.b);
                    CHECK_EQ(c_val, c.a);

                    const auto f_val = static_cast<sight::data::mesh::normal_t>(count);
                    CHECK_EQ(f_val, doctest::Approx(n.nx).epsilon(EPSILON));
                    CHECK_EQ(f_val, doctest::Approx(n.ny).epsilon(EPSILON));
                    CHECK_EQ(f_val, doctest::Approx(n.nz).epsilon(EPSILON));

                    CHECK_EQ(f_val, doctest::Approx(uv.u).epsilon(EPSILON));
                    CHECK_EQ(f_val, doctest::Approx(uv.v).epsilon(EPSILON));

                    ++count;
                }

                CHECK_EQ(mesh->num_cells(), count);
            }

            CHECK_EQ(static_cast<std::size_t>(36048), mesh->allocated_size_in_bytes());
            CHECK_EQ(static_cast<std::size_t>(36036), mesh->size_in_bytes());
            CHECK_EQ(static_cast<sight::data::mesh::size_t>(0), mesh->num_points());

            CHECK_EQ(
                sight::data::mesh::attribute::cell_normals
                | sight::data::mesh::attribute::cell_colors
                | sight::data::mesh::attribute::cell_tex_coords,
                mesh->attributes()
            );
            CHECK_EQ(attributes_array, mesh->attributes());
            CHECK(mesh->has<sight::data::mesh::attribute::cell_colors>());
            CHECK(mesh->has<sight::data::mesh::attribute::cell_normals>());
            CHECK(mesh->has<sight::data::mesh::attribute::cell_tex_coords>());
        }
    }

//-----------------------------------------------------------------------------

    TEST_CASE("copy")
    {
        sight::data::mesh::sptr mesh              = std::make_shared<sight::data::mesh>();
        sight::data::mesh::sptr shallow_copy_mesh = std::make_shared<sight::data::mesh>();
        CHECK(*mesh == *shallow_copy_mesh);

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

        std::array<sight::data::mesh::cell_t, 4> pt1 = {3, 6, 5, 1};
        mesh->push_cell(std::vector(pt1.begin(), pt1.end()));

        std::array<sight::data::mesh::cell_t, 4> pt2 = {1, 3, 5, 7};
        mesh->push_cell(std::vector(pt2.begin(), pt2.end()));

        std::array<sight::data::mesh::cell_t, 4> pt3 = {7, 2, 5, 4};
        mesh->push_cell(std::vector(pt3.begin(), pt3.end()));

        mesh->resize(
            mesh->num_points(),
            mesh->num_cells(),
            sight::data::mesh::cell_type_t::quad,
            sight::data::mesh::attribute::point_colors | sight::data::mesh::attribute::point_tex_coords
        );

        for(sight::data::mesh::size_t i = 0 ; i < mesh->num_points() ; ++i)
        {
            mesh->set_point_color(
                sight::data::mesh::point_t(i),
                sight::data::mesh::color_t(i + 1),
                sight::data::mesh::color_t(i + 2),
                sight::data::mesh::color_t(i + 3),
                sight::data::mesh::color_t(i + 4)
            );

            mesh->set_point_tex_coord(
                sight::data::mesh::point_t(i),
                1.0F / (sight::data::mesh::texcoord_t(i) + 1.0F),
                1.0F / (sight::data::mesh::texcoord_t(i) + 2.0F)
            );
        }

        namespace point = sight::data::iterator::point;
        namespace cell  = sight::data::iterator::cell;

        // check deep copy
        {
            sight::data::mesh::sptr deep_copy_mesh = sight::data::object::copy(mesh);
            const auto copy_dump_lock              = deep_copy_mesh->dump_lock();

            CHECK(*mesh == *deep_copy_mesh);

            CHECK_EQ(mesh->num_points(), deep_copy_mesh->num_points());
            CHECK_EQ(mesh->num_cells(), deep_copy_mesh->num_cells());
            CHECK_EQ(mesh->size_in_bytes(), deep_copy_mesh->size_in_bytes());

            auto point_itr           = mesh->cbegin<point::xyz>();
            const auto point_end     = mesh->cend<point::xyz>();
            auto point_itr_copy_mesh = deep_copy_mesh->cbegin<point::xyz>();

            for( ; point_itr != point_end ; ++point_itr, ++point_itr_copy_mesh)
            {
                CHECK_EQ(point_itr->x, point_itr_copy_mesh->x);
                CHECK_EQ(point_itr->y, point_itr_copy_mesh->y);
                CHECK_EQ(point_itr->z, point_itr_copy_mesh->z);
            }

            const sight::data::mesh::csptr mesh3 = mesh; // copy into a const pointer to explicit test const begin()
            auto cell_itr                        = mesh3->begin<cell::quad>();
            const auto cell_end                  = mesh3->end<cell::quad>();
            auto cell_itr_copy_mesh              = deep_copy_mesh->cbegin<cell::quad>();

            for( ; cell_itr != cell_end ; ++cell_itr, ++cell_itr_copy_mesh)
            {
                for(std::size_t i = 0 ; i < 4 ; ++i)
                {
                    CHECK_EQ(cell_itr->pt[i], cell_itr_copy_mesh->pt[i]);
                }
            }
        }

        //check shallow copy
        {
            shallow_copy_mesh->shallow_copy(mesh);
            CHECK(*mesh == *shallow_copy_mesh);
            CHECK_EQ(mesh->num_points(), shallow_copy_mesh->num_points());
            CHECK_EQ(mesh->num_cells(), shallow_copy_mesh->num_cells());
            CHECK_EQ(mesh->size_in_bytes(), shallow_copy_mesh->size_in_bytes());

            {
                auto point_itr      = mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>().begin();
                auto point_itr_copy =
                    shallow_copy_mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv>().begin();

                const auto& [p1, n1, c1, u1] = *point_itr;
                const auto& [p2, n2, c2, u2] = *point_itr_copy;
                CHECK_EQ(&p1.x, &p2.x);
                CHECK_EQ(&n1.nx, &n2.nx);
                CHECK_EQ(&c1.r, &c2.r);
                CHECK_EQ(&u1.u, &u2.u);
            }

            {
                const auto cell_itr           = mesh->zip_range<cell::quad, cell::rgba, cell::uv>().begin();
                const auto cell_itr_copy_mesh = shallow_copy_mesh->zip_range<cell::quad, cell::rgba,
                                                                             cell::uv>().begin();

                const auto& [i1, c1, t1] = *cell_itr;
                const auto& [i2, c2, t2] = *cell_itr_copy_mesh;
                CHECK_EQ((i1.pt).data(), (i2.pt).data());
                CHECK_EQ(&c1.r, &c2.r);
                CHECK_EQ(&t1.u, &t2.u);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("allocation")
    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

        const auto lock = mesh->dump_lock();

        mesh->push_point(10, 20, 30);
        mesh->push_point(10, 10, 10);
        mesh->push_point(20, 20, 10);

        mesh->push_cell(0, 1, 2);
        CHECK_EQ(static_cast<sight::data::mesh::size_t>(3), mesh->num_points());
        CHECK_EQ(static_cast<sight::data::mesh::size_t>(1), mesh->num_cells());

        CHECK_EQ(
            std::size_t(3LL * 3 * sizeof(sight::data::mesh::position_t) + 3 * sizeof(sight::data::mesh::cell_t)),
            mesh->size_in_bytes()
        );

        sight::data::mesh::size_t point_size = 3000;
        sight::data::mesh::size_t cell_size  = 2000;
        mesh->reserve(point_size, cell_size, sight::data::mesh::cell_type_t::triangle);

        const std::size_t point_position_allocated_size = point_size * 3LL * sizeof(sight::data::mesh::position_t);
        const std::size_t cell_data_allocated_size      = cell_size * 3LL * sizeof(sight::data::mesh::cell_t);

        CHECK_EQ(
            point_position_allocated_size + cell_data_allocated_size,
            mesh->allocated_size_in_bytes()
        );

        CHECK_EQ(static_cast<sight::data::mesh::size_t>(3), mesh->num_points());
        CHECK_EQ(static_cast<sight::data::mesh::size_t>(1), mesh->num_cells());

        CHECK_EQ(sight::data::mesh::attribute::none, mesh->attributes());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::point_colors>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::cell_colors>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::point_normals>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::cell_normals>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::point_tex_coords>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::cell_tex_coords>());

        mesh->reserve(
            point_size,
            cell_size,
            sight::data::mesh::cell_type_t::triangle,
            sight::data::mesh::attribute::point_normals
            | sight::data::mesh::attribute::point_colors | sight::data::mesh::attribute::point_tex_coords
        );

        const std::size_t point_normals_allocated_size    = point_size * 3LL * sizeof(sight::data::mesh::normal_t);
        const std::size_t point_colors_allocated_size     = point_size * 4LL * sizeof(sight::data::mesh::color_t);
        const std::size_t point_tex_coords_allocated_size = point_size * 2LL * sizeof(sight::data::mesh::texcoord_t);
        CHECK_EQ(
            point_position_allocated_size + cell_data_allocated_size
            + point_normals_allocated_size + point_colors_allocated_size + point_tex_coords_allocated_size,
            mesh->allocated_size_in_bytes()
        );

        CHECK_EQ(
            sight::data::mesh::attribute::point_normals
            | sight::data::mesh::attribute::point_colors | sight::data::mesh::attribute::point_tex_coords,
            mesh->attributes()
        );
        CHECK_EQ(true, mesh->has<sight::data::mesh::attribute::point_colors>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::cell_colors>());
        CHECK_EQ(true, mesh->has<sight::data::mesh::attribute::point_normals>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::cell_normals>());
        CHECK_EQ(true, mesh->has<sight::data::mesh::attribute::point_tex_coords>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::cell_tex_coords>());

        mesh->reserve(
            point_size,
            cell_size,
            sight::data::mesh::cell_type_t::triangle,
            sight::data::mesh::attribute::point_normals
            | sight::data::mesh::attribute::point_colors
            | sight::data::mesh::attribute::point_tex_coords
            | sight::data::mesh::attribute::cell_normals
            | sight::data::mesh::attribute::cell_colors
            | sight::data::mesh::attribute::cell_tex_coords
        );

        std::size_t oldsize  = mesh->allocated_size_in_bytes();
        bool adjusted        = mesh->shrink_to_fit();
        std::size_t new_size = mesh->allocated_size_in_bytes();

        CHECK_EQ(true, adjusted);
        CHECK(oldsize > new_size);

        CHECK_EQ(
            sight::data::mesh::attribute::point_normals
            | sight::data::mesh::attribute::point_colors
            | sight::data::mesh::attribute::point_tex_coords
            | sight::data::mesh::attribute::cell_normals
            | sight::data::mesh::attribute::cell_colors
            | sight::data::mesh::attribute::cell_tex_coords,
            mesh->attributes()
        );
        CHECK_EQ(true, mesh->has<sight::data::mesh::attribute::point_colors>());
        CHECK_EQ(true, mesh->has<sight::data::mesh::attribute::cell_colors>());
        CHECK_EQ(true, mesh->has<sight::data::mesh::attribute::point_normals>());
        CHECK_EQ(true, mesh->has<sight::data::mesh::attribute::cell_normals>());
        CHECK_EQ(true, mesh->has<sight::data::mesh::attribute::point_tex_coords>());
        CHECK_EQ(true, mesh->has<sight::data::mesh::attribute::cell_tex_coords>());

        mesh->clear();
        CHECK_EQ(static_cast<sight::data::mesh::size_t>(0), mesh->num_points());
        CHECK_EQ(static_cast<sight::data::mesh::size_t>(0), mesh->num_cells());
        CHECK_EQ(static_cast<std::size_t>(0), mesh->size_in_bytes());

        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::point_colors>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::cell_colors>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::point_normals>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::cell_normals>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::point_tex_coords>());
        CHECK_EQ(false, mesh->has<sight::data::mesh::attribute::cell_tex_coords>());
    }

//------------------------------------------------------------------------------

    TEST_CASE("iterator")
    {
        auto msg = [](auto& _count){return "iteration: " + std::to_string(_count);};

        const sight::data::mesh::size_t nb_points      = 60;
        const sight::data::mesh::size_t nb_cells       = 59;
        const sight::data::mesh::cell_type_t cell_type = sight::data::mesh::cell_type_t::triangle;
        const sight::data::mesh::attribute extra_array =
            sight::data::mesh::attribute::point_normals
            | sight::data::mesh::attribute::point_colors
            | sight::data::mesh::attribute::point_tex_coords
            | sight::data::mesh::attribute::cell_normals
            | sight::data::mesh::attribute::cell_colors
            | sight::data::mesh::attribute::cell_tex_coords;

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();

        mesh->resize(nb_points, nb_cells, cell_type, extra_array);
        const auto lock = mesh->dump_lock();

        for(sight::data::mesh::size_t i = 0 ; i < nb_points ; ++i)
        {
            const auto val                                                = static_cast<uint8_t>(i);
            const std::array<sight::data::mesh::color_t, 4> color         = {val, val, val, val};
            const auto float_val                                          = static_cast<float>(i);
            const std::array<sight::data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
            const std::array<sight::data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
            const std::size_t value                                       = std::size_t(3) * i;
            mesh->set_point(i, static_cast<float>(value), static_cast<float>(value + 1), static_cast<float>(value + 2));
            mesh->set_point_color(i, color);
            mesh->set_point_normal(i, normal);
            mesh->set_point_tex_coord(i, tex_coords);
        }

        for(sight::data::mesh::size_t i = 0 ; i < nb_cells ; ++i)
        {
            mesh->set_cell(i, i, i + 1, i + 2);

            const auto val                                                = static_cast<sight::data::mesh::color_t>(i);
            const std::array<sight::data::mesh::color_t, 4> color         = {val, val, val, val};
            const auto float_val                                          = static_cast<float>(i);
            const std::array<sight::data::mesh::normal_t, 3> normal       = {float_val, float_val, float_val};
            const std::array<sight::data::mesh::texcoord_t, 2> tex_coords = {float_val, float_val};
            mesh->set_cell_color(i, color);
            mesh->set_cell_normal(i, normal);
            mesh->set_cell_tex_coord(i, tex_coords);
        }

        namespace point = sight::data::iterator::point;
        namespace cell  = sight::data::iterator::cell;
        {
            auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

            sight::data::mesh::size_t count = 0;
            for(const auto& [p, n, c, uv] : range)
            {
                const auto f_value = static_cast<float>(3 * count);
                CHECK_EQ(f_value, doctest::Approx(p.x).epsilon(EPSILON));
                CHECK_EQ(f_value + 1, doctest::Approx(p.y).epsilon(EPSILON));
                CHECK_EQ(f_value + 2, doctest::Approx(p.z).epsilon(EPSILON));

                const auto c_val = static_cast<sight::data::mesh::color_t>(count);
                CHECK_MESSAGE(c_val == c.r, msg(count));
                CHECK_MESSAGE(c_val == c.g, msg(count));
                CHECK_MESSAGE(c_val == c.b, msg(count));
                CHECK_MESSAGE(c_val == c.a, msg(count));

                const auto f_val = static_cast<sight::data::mesh::normal_t>(count);
                CHECK_EQ(f_val, doctest::Approx(n.nx).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.ny).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.nz).epsilon(EPSILON));

                CHECK_EQ(f_val, doctest::Approx(uv.u).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(uv.v).epsilon(EPSILON));

                ++count;
            }

            CHECK_EQ(mesh->num_points(), count);
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
                CHECK_EQ(f_value, doctest::Approx(p.x).epsilon(EPSILON));
                CHECK_EQ(f_value + 1, doctest::Approx(p.y).epsilon(EPSILON));
                CHECK_EQ(f_value + 2, doctest::Approx(p.z).epsilon(EPSILON));

                const auto c_val = static_cast<sight::data::mesh::color_t>(count);
                CHECK_MESSAGE(c_val == c.r, msg(count));
                CHECK_MESSAGE(c_val == c.g, msg(count));
                CHECK_MESSAGE(c_val == c.b, msg(count));
                CHECK_MESSAGE(c_val == c.a, msg(count));

                const auto f_val = static_cast<sight::data::mesh::normal_t>(count);
                CHECK_EQ(f_val, doctest::Approx(n.nx).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.ny).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.nz).epsilon(EPSILON));

                CHECK_EQ(f_val, doctest::Approx(uv.u).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(uv.v).epsilon(EPSILON));
            });

            CHECK_EQ(static_cast<std::size_t>(0), count);
        }

        {
            const auto range = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

            sight::data::mesh::size_t count = 0;
            for(const auto& [p, n, c, uv] : range)
            {
                CHECK_MESSAGE(static_cast<sight::data::mesh::size_t>(count) == p.pt[0], msg(count));
                CHECK_MESSAGE(static_cast<sight::data::mesh::size_t>(count + 1) == p.pt[1], msg(count));
                CHECK_MESSAGE(static_cast<sight::data::mesh::size_t>(count + 2) == p.pt[2], msg(count));

                const auto c_val = static_cast<sight::data::mesh::color_t>(count);
                CHECK_MESSAGE(c_val == c.r, msg(count));
                CHECK_MESSAGE(c_val == c.g, msg(count));
                CHECK_MESSAGE(c_val == c.b, msg(count));
                CHECK_MESSAGE(c_val == c.a, msg(count));

                const auto f_val = static_cast<sight::data::mesh::normal_t>(count);
                CHECK_EQ(f_val, doctest::Approx(n.nx).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.ny).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.nz).epsilon(EPSILON));

                CHECK_EQ(f_val, doctest::Approx(uv.u).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(uv.v).epsilon(EPSILON));

                ++count;
            }

            CHECK_EQ(mesh->num_cells(), count);
        }

        {
            auto range = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();
            auto begin = std::make_reverse_iterator(range.end());
            auto end   = std::make_reverse_iterator(range.begin());

            sight::data::mesh::size_t count = 0;
            std::for_each(
                begin,
                end,
                [&](const auto& _it)
            {
                const auto& [p, n, c, uv] = _it;

                CHECK_MESSAGE(
                    static_cast<sight::data::mesh::size_t>(nb_cells - 1 - count) == p.pt[0],
                    msg(count)
                );
                CHECK_MESSAGE(
                    static_cast<sight::data::mesh::size_t>(nb_cells - 1 - count + 1) == p.pt[1],
                    msg(count)
                );
                CHECK_MESSAGE(
                    static_cast<sight::data::mesh::size_t>(nb_cells - 1 - count + 2) == p.pt[2],
                    msg(count)
                );

                const auto c_val = static_cast<sight::data::mesh::color_t>(nb_cells - 1 - count);
                CHECK_MESSAGE(c_val == c.r, msg(count));
                CHECK_MESSAGE(c_val == c.g, msg(count));
                CHECK_MESSAGE(c_val == c.b, msg(count));
                CHECK_MESSAGE(c_val == c.a, msg(count));

                const auto f_val = static_cast<sight::data::mesh::normal_t>(nb_cells - 1 - count);
                CHECK_EQ(f_val, doctest::Approx(n.nx).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.ny).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.nz).epsilon(EPSILON));

                CHECK_EQ(f_val, doctest::Approx(uv.u).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(uv.v).epsilon(EPSILON));
                ++count;
            });

            CHECK_EQ(mesh->num_cells(), count);
        }

        sight::data::mesh::csptr mesh2 = sight::data::mesh::copy(mesh);
        const auto lock2               = mesh2->dump_lock();
        {
            const auto range = mesh2->crange<point::xyz>();

            float count = 0;
            for(const auto& point : range)
            {
                CHECK_EQ(count++, doctest::Approx(point.x).epsilon(EPSILON));
                CHECK_EQ(count++, doctest::Approx(point.y).epsilon(EPSILON));
                CHECK_EQ(count++, doctest::Approx(point.z).epsilon(EPSILON));
            }
        }

        {
            auto range     = mesh->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();
            auto it        = std::make_reverse_iterator(range.end());
            const auto end = std::make_reverse_iterator(range.begin());

            sight::data::mesh::size_t count = 0;
            for( ; it != end ; ++it)
            {
                const auto& [p, n, c, uv] = *it;
                CHECK_MESSAGE(
                    static_cast<sight::data::mesh::size_t>(nb_cells - 1 - count) == p.pt[0],
                    msg(count)
                );
                CHECK_MESSAGE(
                    static_cast<sight::data::mesh::size_t>(nb_cells - 1 - count + 1) == p.pt[1],
                    msg(count)
                );
                CHECK_MESSAGE(
                    static_cast<sight::data::mesh::size_t>(nb_cells - 1 - count + 2) == p.pt[2],
                    msg(count)
                );

                const auto c_val = static_cast<sight::data::mesh::color_t>(nb_cells - 1 - count);
                CHECK_MESSAGE(c_val == c.r, msg(count));
                CHECK_MESSAGE(c_val == c.g, msg(count));
                CHECK_MESSAGE(c_val == c.b, msg(count));
                CHECK_MESSAGE(c_val == c.a, msg(count));

                const auto f_val =
                    static_cast<sight::data::mesh::normal_t>(nb_cells - 1 - count);
                CHECK_EQ(f_val, doctest::Approx(n.nx).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.ny).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(n.nz).epsilon(EPSILON));

                CHECK_EQ(f_val, doctest::Approx(uv.u).epsilon(EPSILON));
                CHECK_EQ(f_val, doctest::Approx(uv.v).epsilon(EPSILON));

                ++count;
            }

            CHECK_EQ(mesh->num_cells(), count);
        }

        sight::data::mesh::sptr mesh3 = std::make_shared<sight::data::mesh>();
        const auto lock3              = mesh3->dump_lock();
        mesh3->resize(nb_points, nb_cells, sight::data::mesh::cell_type_t::quad, extra_array);

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

            CHECK_EQ(mesh->num_points(), count);
        }

        {
            auto range = mesh3->zip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

            sight::data::mesh::size_t count = 0;
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

            CHECK_EQ(mesh->num_cells(), count);
        }

        {
            auto range = mesh3->czip_range<point::xyz, point::nxyz, point::rgba, point::uv>();

            sight::data::mesh::size_t count = 0;
            for(const auto& [p, n, c, uv] : range)
            {
                const auto f_value = static_cast<float>(3 * count);
                CHECK_EQ(f_value, doctest::Approx(p.x).epsilon(EPSILON));
                CHECK_EQ(f_value + 1, doctest::Approx(p.y).epsilon(EPSILON));
                CHECK_EQ(f_value + 2, doctest::Approx(p.z).epsilon(EPSILON));

                CHECK_MESSAGE(4 * count == static_cast<sight::data::mesh::size_t>(c.r), msg(count));
                CHECK_MESSAGE(4 * count + 1 == static_cast<sight::data::mesh::size_t>(c.g), msg(count));
                CHECK_MESSAGE(4 * count + 2 == static_cast<sight::data::mesh::size_t>(c.b), msg(count));
                CHECK_MESSAGE(4 * count + 3 == static_cast<sight::data::mesh::size_t>(c.a), msg(count));

                const auto n_val = static_cast<sight::data::mesh::normal_t>(3 * count);
                CHECK_EQ(n_val + 1, doctest::Approx(n.nx).epsilon(EPSILON));
                CHECK_EQ(n_val + 2, doctest::Approx(n.ny).epsilon(EPSILON));
                CHECK_EQ(n_val + 3, doctest::Approx(n.nz).epsilon(EPSILON));

                const auto uv_val = static_cast<sight::data::mesh::normal_t>(2 * count);

                CHECK_EQ(uv_val, doctest::Approx(uv.u).epsilon(EPSILON));
                CHECK_EQ(uv_val + 1, doctest::Approx(uv.v).epsilon(EPSILON));

                ++count;
            }

            CHECK_EQ(mesh->num_points(), count);
        }

        {
            auto range = mesh3->czip_range<cell::triangle, cell::nxyz, cell::rgba, cell::uv>();

            sight::data::mesh::size_t count = 0;
            for(const auto& [p, n, c, uv] : range)
            {
                CHECK_MESSAGE(static_cast<sight::data::mesh::size_t>(count) == p.pt[0], msg(count));
                CHECK_MESSAGE(static_cast<sight::data::mesh::size_t>(count + 1) == p.pt[1], msg(count));
                CHECK_MESSAGE(static_cast<sight::data::mesh::size_t>(count + 2) == p.pt[2], msg(count));

                CHECK_MESSAGE(4 * count == static_cast<sight::data::mesh::size_t>(c.r), msg(count));
                CHECK_MESSAGE(4 * count + 1 == static_cast<sight::data::mesh::size_t>(c.g), msg(count));
                CHECK_MESSAGE(4 * count + 2 == static_cast<sight::data::mesh::size_t>(c.b), msg(count));
                CHECK_MESSAGE(4 * count + 3 == static_cast<sight::data::mesh::size_t>(c.a), msg(count));

                const auto n_val = static_cast<sight::data::mesh::normal_t>(3 * count);
                CHECK_EQ(n_val + 1, doctest::Approx(n.nx).epsilon(EPSILON));
                CHECK_EQ(n_val + 2, doctest::Approx(n.ny).epsilon(EPSILON));
                CHECK_EQ(n_val + 3, doctest::Approx(n.nz).epsilon(EPSILON));

                const auto uv_val = static_cast<sight::data::mesh::normal_t>(2 * count);
                CHECK_EQ(uv_val, doctest::Approx(uv.u).epsilon(EPSILON));
                CHECK_EQ(uv_val + 1, doctest::Approx(uv.v).epsilon(EPSILON));

                ++count;
            }

            CHECK_EQ(mesh->num_cells(), count);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("iterator_copy")
    {
        auto msg = [](auto& _count){return "iteration: " + std::to_string(_count);};

        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        mesh->resize(
            10,
            10,
            sight::data::mesh::cell_type_t::point,
            sight::data::mesh::attribute::point_normals
            | sight::data::mesh::attribute::point_colors
            | sight::data::mesh::attribute::point_tex_coords
            | sight::data::mesh::attribute::cell_normals
            | sight::data::mesh::attribute::cell_colors
            | sight::data::mesh::attribute::cell_tex_coords
        );

        const auto lock = mesh->dump_lock();

        namespace point = sight::data::iterator::point;
        namespace cell  = sight::data::iterator::cell;
        {
            //fill the mesh
            auto range = mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point, cell::nxyz,
                                         cell::rgba, cell::uv>();

            sight::data::mesh::size_t count = 0;
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

            CHECK_EQ(mesh->num_points(), count);
        }

        {
            // check the mesh points
            auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point, cell::nxyz,
                                          cell::rgba, cell::uv>();

            sight::data::mesh::size_t count = 0;
            for(const auto& [p, n, c, uv, cell, cn, cc, cuv] : range)
            {
                // point
                CHECK_EQ(static_cast<float>(count), doctest::Approx(p.x).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.6F, doctest::Approx(p.y).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.8F, doctest::Approx(p.z).epsilon(0.00001));

                CHECK_EQ(static_cast<float>(count) + 0.1F, doctest::Approx(n.nx).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.2F, doctest::Approx(n.ny).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.3F, doctest::Approx(n.nz).epsilon(0.00001));

                CHECK_EQ(static_cast<std::uint8_t>(4 * count), c.r);
                CHECK_EQ(static_cast<std::uint8_t>(4 * count + 1), c.g);
                CHECK_EQ(static_cast<std::uint8_t>(4 * count + 2), c.b);
                CHECK_EQ(static_cast<std::uint8_t>(4 * count + 3), c.a);

                CHECK_EQ(static_cast<float>(count) + 0.7, doctest::Approx(uv.u).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.9, doctest::Approx(uv.v).epsilon(0.00001));
                // cell
                CHECK_EQ(count, cell.pt);

                CHECK_EQ(static_cast<float>(count) + 0.11F, doctest::Approx(cn.nx).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.22F, doctest::Approx(cn.ny).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.33F, doctest::Approx(cn.nz).epsilon(0.00001));

                CHECK_EQ(static_cast<std::uint8_t>(4 * count + 1), cc.r);
                CHECK_EQ(static_cast<std::uint8_t>(4 * count + 2), cc.g);
                CHECK_EQ(static_cast<std::uint8_t>(4 * count + 3), cc.b);
                CHECK_EQ(static_cast<std::uint8_t>(4 * count + 4), cc.a);

                CHECK_EQ(static_cast<float>(count) + 0.77, doctest::Approx(cuv.u).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.99, doctest::Approx(cuv.v).epsilon(0.00001));

                ++count;
            }

            CHECK_EQ(mesh->num_points(), count);
        }

        sight::data::mesh::sptr copied_mesh = std::make_shared<sight::data::mesh>();
        CHECK(*mesh != *copied_mesh);

        copied_mesh->resize(
            10,
            10,
            sight::data::mesh::cell_type_t::point,
            sight::data::mesh::attribute::point_normals
            | sight::data::mesh::attribute::point_colors
            | sight::data::mesh::attribute::point_tex_coords
            | sight::data::mesh::attribute::cell_normals
            | sight::data::mesh::attribute::cell_colors
            | sight::data::mesh::attribute::cell_tex_coords
        );
        const auto lock2 = copied_mesh->dump_lock();
        {
            // copy the mesh points and cells with the iterator
            auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                          cell::point, cell::nxyz, cell::rgba, cell::uv>();
            auto range_copy = copied_mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                                     cell::point, cell::nxyz, cell::rgba, cell::uv>();

            boost::copy(range, range_copy.begin());
        }

        CHECK(*mesh == *copied_mesh);

        {
            // check the copied mesh
            auto range = mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point,
                                          cell::nxyz, cell::rgba, cell::uv>();
            auto range_copy = copied_mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv, cell::point,
                                                      cell::nxyz, cell::rgba, cell::uv>();
            auto combined_range = boost::combine(range, range_copy);

            sight::data::mesh::size_t count = 0;
            for(const auto& [pt1, pt2] : combined_range)
            {
                const auto& [p1, n1, c1, uv1, cell1, cn1, cc1, cuv1] = pt1;
                const auto& [p2, n2, c2, uv2, cell2, cn2, cc2, cuv2] = pt2;

                // point
                CHECK_MESSAGE(p1.x == p2.x, msg(count));
                CHECK_MESSAGE(p1.y == p2.y, msg(count));
                CHECK_MESSAGE(p1.z == p2.z, msg(count));

                CHECK_MESSAGE(n1.nx == n2.nx, msg(count));
                CHECK_MESSAGE(n1.ny == n2.ny, msg(count));
                CHECK_MESSAGE(n1.nz == n2.nz, msg(count));
                CHECK_MESSAGE(c1.r == c2.r, msg(count));
                CHECK_MESSAGE(c1.g == c2.g, msg(count));
                CHECK_MESSAGE(c1.b == c2.b, msg(count));
                CHECK_MESSAGE(c1.a == c2.a, msg(count));

                CHECK_MESSAGE(uv1.u == uv2.u, msg(count));
                CHECK_MESSAGE(uv1.v == uv2.v, msg(count));

                // cell
                CHECK_EQ(cell1.pt, cell2.pt);

                CHECK_EQ(cn1.nx, cn2.nx);
                CHECK_EQ(cn1.ny, cn2.ny);
                CHECK_EQ(cn1.nz, cn2.nz);

                CHECK_EQ(cc1.r, cc2.r);
                CHECK_EQ(cc1.g, cc2.g);
                CHECK_EQ(cc1.b, cc2.b);
                CHECK_EQ(cc1.a, cc2.a);

                CHECK_EQ(cuv1.u, cuv2.u);
                CHECK_EQ(cuv1.v, cuv2.v);

                ++count;
            }

            CHECK_EQ(mesh->num_points(), count);
        }

        {
            // check the mesh points are deep copied and not shallow copied

            auto range = mesh->zip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                         cell::point, cell::nxyz, cell::rgba, cell::uv>();

            auto point = boost::make_tuple(
                sight::data::iterator::point::xyz({.x   = 1.0F, .y = 1.1F, .z = 1.2F}),
                sight::data::iterator::point::nxyz({.nx = 1.F, .ny = 0.F, .nz = 0.F}),
                sight::data::iterator::point::rgba({.r  = 25, .g = 15, .b = 18, .a = 32}),
                sight::data::iterator::point::uv({.u    = 0.5F, .v = 1.F}),
                sight::data::iterator::cell::point({0}),
                sight::data::iterator::cell::nxyz({.nx = 0.F, .ny = 1.F, .nz = 0.F}),
                sight::data::iterator::cell::rgba({.r  = 20, .g = 13, .b = 10, .a = 37}),
                sight::data::iterator::cell::uv({.u    = 0.2F, .v = 0.8F})
            );
            boost::fill(range, point);

            // check the mesh points are filled
            sight::data::mesh::size_t count = 0;
            const auto& [p2, n2, c2, uv2, cell2, cn2, cc2, cuv2] = point;

            for(const auto& [p1, n1, c1, uv1, cell1, cn1, cc1, cuv1] : range)
            {
                // point
                CHECK_MESSAGE(p1.x == p2.x, msg(count));
                CHECK_MESSAGE(p1.y == p2.y, msg(count));
                CHECK_MESSAGE(p1.z == p2.z, msg(count));

                CHECK_MESSAGE(n1.nx == n2.nx, msg(count));
                CHECK_MESSAGE(n1.ny == n2.ny, msg(count));
                CHECK_MESSAGE(n1.nz == n2.nz, msg(count));
                CHECK_MESSAGE(c1.r == c2.r, msg(count));
                CHECK_MESSAGE(c1.g == c2.g, msg(count));
                CHECK_MESSAGE(c1.b == c2.b, msg(count));
                CHECK_MESSAGE(c1.a == c2.a, msg(count));

                CHECK_MESSAGE(uv1.u == uv2.u, msg(count));
                CHECK_MESSAGE(uv1.v == uv2.v, msg(count));

                // cell
                CHECK_EQ(cell1.pt, cell2.pt);

                CHECK_EQ(cn1.nx, cn2.nx);
                CHECK_EQ(cn1.ny, cn2.ny);
                CHECK_EQ(cn1.nz, cn2.nz);

                CHECK_EQ(cc1.r, cc2.r);
                CHECK_EQ(cc1.g, cc2.g);
                CHECK_EQ(cc1.b, cc2.b);
                CHECK_EQ(cc1.a, cc2.a);

                CHECK_EQ(cuv1.u, cuv2.u);
                CHECK_EQ(cuv1.v, cuv2.v);

                ++count;
            }

            CHECK_EQ(mesh->num_points(), count);

            auto range_copy = copied_mesh->czip_range<point::xyz, point::nxyz, point::rgba, point::uv,
                                                      cell::point, cell::nxyz, cell::rgba, cell::uv>();

            count = 0;
            for(const auto& [p, n, c, uv, cell, cn, cc, cuv] : range_copy)
            {
                // point
                CHECK_EQ(static_cast<float>(count), doctest::Approx(p.x).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.6F, doctest::Approx(p.y).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.8F, doctest::Approx(p.z).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.1F, doctest::Approx(n.nx).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.2F, doctest::Approx(n.ny).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.3F, doctest::Approx(n.nz).epsilon(0.00001));
                CHECK_MESSAGE(static_cast<std::uint8_t>(4 * count) == c.r, msg(count));
                CHECK_MESSAGE(static_cast<std::uint8_t>(4 * count + 1) == c.g, msg(count));
                CHECK_MESSAGE(static_cast<std::uint8_t>(4 * count + 2) == c.b, msg(count));
                CHECK_MESSAGE(static_cast<std::uint8_t>(4 * count + 3) == c.a, msg(count));
                CHECK_EQ(static_cast<float>(count) + 0.7, doctest::Approx(uv.u).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.9, doctest::Approx(uv.v).epsilon(0.00001));

                // cell
                CHECK_MESSAGE(count == cell.pt, msg(count));
                CHECK_EQ(static_cast<float>(count) + 0.11F, doctest::Approx(cn.nx).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.22F, doctest::Approx(cn.ny).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.33F, doctest::Approx(cn.nz).epsilon(0.00001));
                CHECK_MESSAGE(static_cast<std::uint8_t>(4 * count + 1) == cc.r, msg(count));
                CHECK_MESSAGE(static_cast<std::uint8_t>(4 * count + 2) == cc.g, msg(count));
                CHECK_MESSAGE(static_cast<std::uint8_t>(4 * count + 3) == cc.b, msg(count));
                CHECK_MESSAGE(static_cast<std::uint8_t>(4 * count + 4) == cc.a, msg(count));
                CHECK_EQ(static_cast<float>(count) + 0.77, doctest::Approx(cuv.u).epsilon(0.00001));
                CHECK_EQ(static_cast<float>(count) + 0.99, doctest::Approx(cuv.v).epsilon(0.00001));
                ++count;
            }

            CHECK_EQ(mesh->num_points(), count);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("benchmark_iterator")
    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto lock              = mesh->dump_lock();

        static const std::size_t s_N = 1;

        mesh->resize(
            655360,
            10000,
            sight::data::mesh::cell_type_t::triangle,
            sight::data::mesh::attribute::point_normals
        );

        auto pos  = mesh->begin<sight::data::iterator::point::xyz>();
        auto norm = mesh->begin<sight::data::iterator::point::nxyz>();

        auto pos2  = mesh->end<sight::data::iterator::point::xyz>();
        auto norm2 = mesh->end<sight::data::iterator::point::nxyz>();

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

        auto range = mesh->zip_range<sight::data::iterator::point::xyz, sight::data::iterator::point::nxyz>();
        {
            FW_PROFILE("std::for_each - zip array_iterator");
            for(std::size_t i = 0 ; i < s_N ; ++i)
            {
                std::ranges::for_each(range, fn2);
            }
        }

        std::vector<sight::data::iterator::point::xyz> pos_v(655360);
        std::vector<sight::data::iterator::point::nxyz> norm_v(655360);
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

//-----------------------------------------------------------------------------
    TEST_CASE("bounding_box")
    {
        auto mesh = std::make_shared<sight::data::mesh>();
        auto lock = mesh->dump_lock();

        mesh->push_point(1.F, 2.F, 3.F);
        mesh->push_point(4.F, 4.F, 4.F);
        sight::data::mesh::axis_aligned_box_t bbox = mesh->get_bounding_box();

        CHECK_EQ(1.F, bbox.min[0]);
        CHECK_EQ(2.F, bbox.min[1]);
        CHECK_EQ(3.F, bbox.min[2]);
        CHECK_EQ(4.F, bbox.max[0]);
        CHECK_EQ(4.F, bbox.max[1]);
        CHECK_EQ(4.F, bbox.max[2]);
    }

//------------------------------------------------------------------------------

    TEST_CASE("axis_aligned_box_is_invalid")
    {
        using box = sight::data::mesh::axis_aligned_box_t;

        // Default-constructed box has min = +inf, max = -inf on every axis.
        CHECK(box {}.is_invalid());

        // A proper box with non-degenerate extents is valid.
        box valid;
        valid.min = {0.F, 0.F, 0.F};
        valid.max = {1.F, 1.F, 1.F};
        CHECK_FALSE(valid.is_invalid());

        // A flat box (extent = 0 on one axis) is still valid.
        box flat;
        flat.min = {0.F, 0.F, 0.F};
        flat.max = {0.F, 1.F, 1.F};
        CHECK_FALSE(flat.is_invalid());

        // Inverted on the X axis only → invalid.
        box inv_x;
        inv_x.min = {1.F, 0.F, 0.F};
        inv_x.max = {0.F, 1.F, 1.F};
        CHECK(inv_x.is_invalid());

        // Inverted on the Y axis only → invalid.
        box inv_y;
        inv_y.min = {0.F, 1.F, 0.F};
        inv_y.max = {1.F, 0.F, 1.F};
        CHECK(inv_y.is_invalid());

        // Inverted on the Z axis only → invalid.
        box inv_z;
        inv_z.min = {0.F, 0.F, 1.F};
        inv_z.max = {1.F, 1.F, 0.F};
        CHECK(inv_z.is_invalid());

        // A mesh with no points returns the sentinel default bbox, which is invalid.
        auto empty_mesh = std::make_shared<sight::data::mesh>();
        CHECK(empty_mesh->get_bounding_box().is_invalid());

        // A mesh with points produces a valid bbox.
        auto mesh      = std::make_shared<sight::data::mesh>();
        const auto lck = mesh->dump_lock();
        mesh->push_point(-1.F, -2.F, -3.F);
        mesh->push_point(1.F, 2.F, 3.F);
        CHECK_FALSE(mesh->get_bounding_box().is_invalid());
    }

//------------------------------------------------------------------------------

    TEST_CASE("equality")
    {
        auto mesh1 = std::make_shared<sight::data::mesh>();
        auto mesh2 = std::make_shared<sight::data::mesh>();

        CHECK(*mesh1 == *mesh2);
        CHECK(!(*mesh1 != *mesh2));

        // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
            mesh1->op; \
            CHECK_MESSAGE( \
                *mesh1 != *mesh2, \
                "Meshes should be different when applying " #op " to the first" \
            ); \
            CHECK_MESSAGE( \
                !(*mesh1 == *mesh2), \
                "Meshes should be different when applying " #op " to the first" \
            ); \
            mesh2->op; \
            CHECK_MESSAGE( \
                *mesh1 == *mesh2, \
                "Meshes should be equal when " #op " to both" \
            ); \
            CHECK_MESSAGE( \
                !(*mesh1 != *mesh2), \
                "Meshes should be equal when " #op " to both" \
            );

        mesh1->resize(1, 1, sight::data::mesh::cell_type_t::quad, sight::data::mesh::attribute::none);
        CHECK(*mesh1 != *mesh2);
        CHECK(!(*mesh1 == *mesh2));
        mesh2->resize(1, 1, sight::data::mesh::cell_type_t::quad, sight::data::mesh::attribute::none);
        auto lock1 = mesh1->dump_lock();
        auto lock2 = mesh2->dump_lock();
        mesh1->set_point(0, {1, 2, 3});
        mesh2->set_point(0, {1, 2, 3});
        mesh1->set_cell(0, {1, 2, 3, 4});
        mesh2->set_cell(0, {1, 2, 3, 4});
        CHECK(*mesh1 == *mesh2);
        CHECK(!(*mesh1 != *mesh2));

        mesh1->resize(1, 1, sight::data::mesh::cell_type_t::quad, sight::data::mesh::attribute::point_colors);
        CHECK(*mesh1 != *mesh2);
        CHECK(!(*mesh1 == *mesh2));
        mesh2->resize(1, 1, sight::data::mesh::cell_type_t::quad, sight::data::mesh::attribute::point_colors);
        TEST(set_point_color(0, {1, 2, 3}));

        TEST(resize(1, 1, sight::data::mesh::cell_type_t::tetra, sight::data::mesh::attribute::point_colors));
        TEST(resize(1, 1, sight::data::mesh::cell_type_t::quad, sight::data::mesh::attribute::none));

    #undef TEST
    }
} // TEST_SUITE("sight::data::mesh")
