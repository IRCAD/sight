/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "core/compound_types.hpp"

#include <data/matrix4.hpp>

#include <geometry/data/matrix4.hpp>
#include <geometry/data/mesh.hpp>

#include <utest_data/generator/mesh.hpp>

#include <doctest/doctest.h>

#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <numbers>

TEST_SUITE("sight::geometry::data::mesh")
{
    namespace point = sight::data::iterator::point;
    namespace cell  = sight::data::iterator::cell;

    const double EPSILON = 10e-6;

    TEST_CASE("colorize_points")
    {
        {
            const std::uint8_t r = 214;
            const std::uint8_t g = 25;
            const std::uint8_t b = 33;
            const std::uint8_t a = 63;

            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_mesh(mesh);

            mesh->resize(
                mesh->num_points(),
                mesh->num_cells(),
                mesh->cell_type(),
                sight::data::mesh::attribute::point_colors
            );

            sight::geometry::data::mesh::colorize_mesh_points(mesh, r, g, b, a);

            const auto dump_lock = mesh->dump_lock();

            std::size_t count = 0;
            for(const auto& color : mesh->crange<point::rgba>())
            {
                ++count;
                CHECK_MESSAGE(static_cast<int>(color.r) == static_cast<int>(r), "idx ", count);
                CHECK_MESSAGE(static_cast<int>(color.g) == static_cast<int>(g), "idx ", count);
                CHECK_MESSAGE(static_cast<int>(color.b) == static_cast<int>(b), "idx ", count);
                CHECK_MESSAGE(static_cast<int>(color.a) == static_cast<int>(a), "idx ", count);
            }
        }

        // Check to colorize few points with RGBA
        {
            const std::uint8_t r = 214;
            const std::uint8_t g = 155;
            const std::uint8_t b = 34;
            const std::uint8_t a = 124;

            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_mesh(mesh);

            mesh->resize(
                mesh->num_points(),
                mesh->num_cells(),
                mesh->cell_type(),
                sight::data::mesh::attribute::point_colors
            );

            std::vector<std::size_t> vector_num_triangle = {{0, 12, 1, 3, 21}};

            // fill color with 0
            sight::geometry::data::mesh::colorize_mesh_points(mesh, 0, 0, 0, 0);

            sight::geometry::data::mesh::colorize_mesh_points(mesh, vector_num_triangle, r, g, b, a);

            const auto dump_lock = mesh->dump_lock();

            const auto cell_iter_begin = mesh->cbegin<cell::triangle>();

            // get the 3 points of each triangles
            std::set<std::size_t> vertex_indices;
            for(std::size_t i : vector_num_triangle)
            {
                auto cell = cell_iter_begin + std::int64_t(i);
                vertex_indices.insert(cell->pt[0]);
                vertex_indices.insert(cell->pt[1]);
                vertex_indices.insert(cell->pt[2]);
            }

            std::size_t count = 0;
            for(const auto& color : mesh->range<point::rgba>())
            {
                auto iter = vertex_indices.find(count);
                if(iter != vertex_indices.end())
                {
                    CHECK_EQ(color.r, r);
                    CHECK_EQ(color.g, g);
                    CHECK_EQ(color.b, b);
                    CHECK_EQ(color.a, a);
                }
                else
                {
                    CHECK_EQ(color.r, static_cast<std::uint8_t>(0));
                    CHECK_EQ(color.g, static_cast<std::uint8_t>(0));
                    CHECK_EQ(color.b, static_cast<std::uint8_t>(0));
                    CHECK_EQ(color.a, static_cast<std::uint8_t>(0));
                }

                ++count;
            }
        }
    }

    TEST_CASE("colorize_cells")
    {
        // Check to colorize all the mesh with RGB
        {
            const std::uint8_t r = 24;
            const std::uint8_t g = 55;
            const std::uint8_t b = 3;

            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_mesh(mesh);

            mesh->resize(
                mesh->num_points(),
                mesh->num_cells(),
                mesh->cell_type(),
                sight::data::mesh::attribute::cell_colors
            );

            sight::geometry::data::mesh::colorize_mesh_cells(mesh, r, g, b);

            const auto dump_lock = mesh->dump_lock();

            std::size_t count = 0;
            for(const auto& color : mesh->crange<cell::rgba>())
            {
                ++count;
                CHECK_MESSAGE(static_cast<int>(color.r) == static_cast<int>(r), "idx ", count);
                CHECK_MESSAGE(static_cast<int>(color.g) == static_cast<int>(g), "idx ", count);
                CHECK_MESSAGE(static_cast<int>(color.b) == static_cast<int>(b), "idx ", count);
                CHECK_MESSAGE(static_cast<int>(color.a) == 255, "idx ", count);
            }
        }

        // Check to colorize all the mesh with RGBA
        {
            const std::uint8_t r = 214;
            const std::uint8_t g = 25;
            const std::uint8_t b = 33;
            const std::uint8_t a = 63;

            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_mesh(mesh);

            mesh->resize(
                mesh->num_points(),
                mesh->num_cells(),
                mesh->cell_type(),
                sight::data::mesh::attribute::cell_colors
            );

            sight::geometry::data::mesh::colorize_mesh_cells(mesh, r, g, b, a);

            const auto dump_lock = mesh->dump_lock();

            std::size_t count = 0;
            for(const auto& color : mesh->crange<cell::rgba>())
            {
                ++count;
                CHECK_MESSAGE(static_cast<int>(color.r) == static_cast<int>(r), "idx ", count);
                CHECK_MESSAGE(static_cast<int>(color.g) == static_cast<int>(g), "idx ", count);
                CHECK_MESSAGE(static_cast<int>(color.b) == static_cast<int>(b), "idx ", count);
                CHECK_MESSAGE(static_cast<int>(color.a) == static_cast<int>(a), "idx ", count);
            }
        }

        // Check to colorize few cells with RGB
        {
            const std::uint8_t r = 24;
            const std::uint8_t g = 55;
            const std::uint8_t b = 3;

            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_mesh(mesh);

            mesh->resize(
                mesh->num_points(),
                mesh->num_cells(),
                mesh->cell_type(),
                sight::data::mesh::attribute::cell_colors
            );

            std::vector<std::size_t> vector_num_triangle = {{2, 3, 18, 23, 6}};

            // fill color with 0
            sight::geometry::data::mesh::colorize_mesh_cells(mesh, 0, 0, 0, 0);

            sight::geometry::data::mesh::colorize_mesh_cells(mesh, vector_num_triangle, r, g, b);

            const auto dump_lock = mesh->dump_lock();

            std::size_t count = 0;
            for(const auto& color : mesh->crange<cell::rgba>())
            {
                auto iter = std::ranges::find(vector_num_triangle, count);
                if(iter != vector_num_triangle.end())
                {
                    CHECK_EQ(color.r, r);
                    CHECK_EQ(color.g, g);
                    CHECK_EQ(color.b, b);
                    CHECK_EQ(color.a, static_cast<std::uint8_t>(255));
                }
                else
                {
                    CHECK_EQ(color.r, static_cast<std::uint8_t>(0));
                    CHECK_EQ(color.g, static_cast<std::uint8_t>(0));
                    CHECK_EQ(color.b, static_cast<std::uint8_t>(0));
                    CHECK_EQ(color.a, static_cast<std::uint8_t>(0));
                }

                ++count;
            }
        }

        // Check to colorize few cells with RGBA
        {
            const std::uint8_t r = 2;
            const std::uint8_t g = 125;
            const std::uint8_t b = 75;
            const std::uint8_t a = 55;

            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_mesh(mesh);

            mesh->resize(
                mesh->num_points(),
                mesh->num_cells(),
                mesh->cell_type(),
                sight::data::mesh::attribute::cell_colors
            );

            std::vector<std::size_t> vector_num_triangle = {{2, 3, 18, 23, 6, 5}};

            // fill color with 0
            sight::geometry::data::mesh::colorize_mesh_cells(mesh, 0, 0, 0, 0);

            sight::geometry::data::mesh::colorize_mesh_cells(mesh, vector_num_triangle, r, g, b, a);

            const auto dump_lock = mesh->dump_lock();

            std::size_t count = 0;
            for(const auto& color : mesh->crange<cell::rgba>())
            {
                auto iter = std::ranges::find(vector_num_triangle, count);
                if(iter != vector_num_triangle.end())
                {
                    CHECK_EQ(color.r, r);
                    CHECK_EQ(color.g, g);
                    CHECK_EQ(color.b, b);
                    CHECK_EQ(color.a, a);
                }
                else
                {
                    CHECK_EQ(color.r, static_cast<std::uint8_t>(0));
                    CHECK_EQ(color.g, static_cast<std::uint8_t>(0));
                    CHECK_EQ(color.b, static_cast<std::uint8_t>(0));
                    CHECK_EQ(color.a, static_cast<std::uint8_t>(0));
                }

                ++count;
            }
        }
    }

    TEST_CASE("transform")
    {
        auto vec3_length = [](auto _v) -> float
                           {
                               return static_cast<float>(
                                   std::sqrt(std::pow(_v[0], 2) + std::pow(_v[1], 2) + std::pow(_v[2], 2))
                               );
                           };
        const std::array<std::array<float, 3>, 3> expected_points {
            std::array {3.F, -5.F, 5.F},
            std::array {9.F, -11.F, 11.F},
            std::array {15.F, -17.F, 17.F}
        };
        const std::array<std::array<float, 3>, 3> expected_normals {
            std::array {2.F, -6.F, 4.F},
            std::array {8.F, -12.F, 10.F},
            std::array {14.F, -18.F, 16.F}
        };
        const std::array<float, 3> expected_normal_lengths {
            vec3_length(expected_normals[0]),
            vec3_length(expected_normals[1]),
            vec3_length(expected_normals[2])
        };
        const glm::mat4 translation = glm::translate(glm::identity<glm::mat4>(), glm::vec3(1, 1, 1));
        const glm::mat4 rotation    = glm::rotate(
            glm::identity<glm::mat4>(),
            glm::pi<float>() / 2,
            glm::vec3(1, 0, 0)
        );
        const glm::mat4 scale          = glm::scale(glm::identity<glm::mat4>(), glm::vec3(2, 2, 2));
        const glm::mat4 transformation = translation * rotation * scale;
        sight::data::matrix4 trans;
        sight::geometry::data::from_glm_mat(trans, transformation);

        // Points only
        {
            sight::data::mesh::sptr in = std::make_shared<sight::data::mesh>();
            const auto in_lock         = in->dump_lock();
            in->push_point(1, 2, 3);
            in->push_point(4, 5, 6);
            in->push_point(7, 8, 9);
            sight::data::mesh::sptr out = sight::data::mesh::copy(in);
            const auto out_lock         = out->dump_lock();
            sight::geometry::data::mesh::transform(in, out, trans);
            std::size_t i = 0;
            for(const auto& point : out->crange<point::xyz>())
            {
                CHECK(point.x == doctest::Approx(expected_points[i][0]).epsilon(EPSILON));
                CHECK(point.y == doctest::Approx(expected_points[i][1]).epsilon(EPSILON));
                CHECK(point.z == doctest::Approx(expected_points[i][2]).epsilon(EPSILON));
                i++;
            }
        }

        // Points + Point normals
        {
            sight::data::mesh::sptr in = std::make_shared<sight::data::mesh>();
            in->reserve(3, 1, sight::data::mesh::cell_type_t::point, sight::data::mesh::attribute::point_normals);
            const auto in_lock = in->dump_lock();
            auto id            = in->push_point(1, 2, 3);
            in->set_point_normal(id, 1, 2, 3);
            id = in->push_point(4, 5, 6);
            in->set_point_normal(id, 4, 5, 6);
            id = in->push_point(7, 8, 9);
            in->set_point_normal(id, 7, 8, 9);
            sight::data::mesh::sptr out = sight::data::mesh::copy(in);
            const auto out_lock         = out->dump_lock();
            sight::geometry::data::mesh::transform(in, out, trans);
            std::size_t i = 0;
            for(const auto& [p, n] : out->czip_range<point::xyz, point::nxyz>())
            {
                CHECK(p.x == doctest::Approx(expected_points[i][0]).epsilon(EPSILON));
                CHECK(p.y == doctest::Approx(expected_points[i][1]).epsilon(EPSILON));
                CHECK(p.z == doctest::Approx(expected_points[i][2]).epsilon(EPSILON));

                CHECK(n.nx == doctest::Approx(expected_normals[i][0] / expected_normal_lengths[i]).epsilon(EPSILON));
                CHECK(n.ny == doctest::Approx(expected_normals[i][1] / expected_normal_lengths[i]).epsilon(EPSILON));
                CHECK(n.nz == doctest::Approx(expected_normals[i][2] / expected_normal_lengths[i]).epsilon(EPSILON));

                i++;
            }
        }

        // Points + Cell normals
        {
            sight::data::mesh::sptr in = std::make_shared<sight::data::mesh>();
            in->reserve(3, 3, sight::data::mesh::cell_type_t::point, sight::data::mesh::attribute::cell_normals);
            const auto in_lock = in->dump_lock();
            auto id_p          = in->push_point(1, 2, 3);
            auto id_c          = in->push_cell(id_p);
            in->set_cell_normal(id_c, 1, 2, 3);
            id_p = in->push_point(4, 5, 6);
            id_c = in->push_cell(id_p);
            in->set_cell_normal(id_c, 4, 5, 6);
            id_p = in->push_point(7, 8, 9);
            id_c = in->push_cell(id_p);
            in->set_cell_normal(id_c, 7, 8, 9);
            sight::data::mesh::sptr out = sight::data::mesh::copy(in);
            const auto out_lock         = out->dump_lock();
            sight::geometry::data::mesh::transform(in, out, trans);
            std::size_t i = 0;
            for(const auto& [p, n] : out->czip_range<point::xyz, cell::nxyz>())
            {
                CHECK(p.x == doctest::Approx(expected_points[i][0]).epsilon(EPSILON));
                CHECK(p.y == doctest::Approx(expected_points[i][1]).epsilon(EPSILON));
                CHECK(p.z == doctest::Approx(expected_points[i][2]).epsilon(EPSILON));

                CHECK(n.nx == doctest::Approx(expected_normals[i][0] / expected_normal_lengths[i]).epsilon(EPSILON));
                CHECK(n.ny == doctest::Approx(expected_normals[i][1] / expected_normal_lengths[i]).epsilon(EPSILON));
                CHECK(n.nz == doctest::Approx(expected_normals[i][2] / expected_normal_lengths[i]).epsilon(EPSILON));

                i++;
            }
        }

        // Points + All normals
        {
            sight::data::mesh::sptr in = std::make_shared<sight::data::mesh>();
            in->reserve(
                3,
                3,
                sight::data::mesh::cell_type_t::point,
                sight::data::mesh::attribute::point_normals | sight::data::mesh::attribute::cell_normals
            );
            const auto in_lock = in->dump_lock();
            auto id_p          = in->push_point(1, 2, 3);
            in->set_point_normal(id_p, 1, 2, 3);
            auto id_c = in->push_cell(id_p);
            in->set_cell_normal(id_c, 1, 2, 3);
            id_p = in->push_point(4, 5, 6);
            in->set_point_normal(id_p, 4, 5, 6);
            id_c = in->push_cell(id_p);
            in->set_cell_normal(id_c, 4, 5, 6);
            id_p = in->push_point(7, 8, 9);
            in->set_point_normal(id_p, 7, 8, 9);
            id_c = in->push_cell(id_p);
            in->set_cell_normal(id_c, 7, 8, 9);
            sight::data::mesh::sptr out = sight::data::mesh::copy(in);
            const auto out_lock         = out->dump_lock();
            sight::geometry::data::mesh::transform(in, out, trans);
            std::size_t i = 0;
            for(const auto& [p, pn, cn] : out->czip_range<point::xyz, point::nxyz, cell::nxyz>())
            {
                CHECK(p.x == doctest::Approx(expected_points[i][0]).epsilon(EPSILON));
                CHECK(p.y == doctest::Approx(expected_points[i][1]).epsilon(EPSILON));
                CHECK(p.z == doctest::Approx(expected_points[i][2]).epsilon(EPSILON));

                CHECK(pn.nx == doctest::Approx(expected_normals[i][0] / expected_normal_lengths[i]).epsilon(EPSILON));
                CHECK(pn.ny == doctest::Approx(expected_normals[i][1] / expected_normal_lengths[i]).epsilon(EPSILON));
                CHECK(pn.nz == doctest::Approx(expected_normals[i][2] / expected_normal_lengths[i]).epsilon(EPSILON));

                CHECK(cn.nx == doctest::Approx(expected_normals[i][0] / expected_normal_lengths[i]).epsilon(EPSILON));
                CHECK(cn.ny == doctest::Approx(expected_normals[i][1] / expected_normal_lengths[i]).epsilon(EPSILON));
                CHECK(cn.nz == doctest::Approx(expected_normals[i][2] / expected_normal_lengths[i]).epsilon(EPSILON));

                i++;
            }
        }
    }

    TEST_CASE("is_closed")
    {
        {
            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_mesh(mesh);

            const bool is_closed = sight::geometry::data::mesh::is_closed(mesh);

            CHECK_EQ(is_closed, false);
        }

        {
            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_quad_mesh(mesh);

            const bool is_closed = sight::geometry::data::mesh::is_closed(mesh);
            CHECK_EQ(is_closed, false);
        }

        {
            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            sight::utest_data::generator::mesh::generate_triangle_quad_mesh(mesh);

            const bool is_closed = sight::geometry::data::mesh::is_closed(mesh);
            CHECK_EQ(is_closed, false);
        }

        {
            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            const auto dump_lock         = mesh->dump_lock();

            mesh->push_point(0.F, 0.F, 0.F);
            mesh->push_point(1.F, 0.F, 0.F);
            mesh->push_point(1.F, 1.F, 0.F);
            mesh->push_point(0.F, 1.F, 0.F);
            mesh->push_point(0.F, 0.F, 1.F);
            mesh->push_point(1.F, 0.F, 1.F);
            mesh->push_point(1.F, 1.F, 1.F);
            mesh->push_point(0.F, 1.F, 1.F);

            mesh->push_cell(0, 1, 2, 3);
            mesh->push_cell(0, 4, 5, 1);
            mesh->push_cell(1, 5, 6, 2);
            mesh->push_cell(4, 5, 6, 7);
            mesh->push_cell(0, 4, 7, 3);
            mesh->push_cell(3, 7, 6, 2);

            const bool is_closed = sight::geometry::data::mesh::is_closed(mesh);
            CHECK_EQ(is_closed, true);
        }

        {
            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            const auto dump_lock         = mesh->dump_lock();

            mesh->push_point(0.F, 0.F, 0.F);
            mesh->push_point(1.F, 0.F, 0.F);
            mesh->push_point(1.F, 1.F, 0.F);
            mesh->push_point(0.F, 1.F, 0.F);
            mesh->push_point(0.F, 0.F, 1.F);
            mesh->push_point(1.F, 0.F, 1.F);
            mesh->push_point(1.F, 1.F, 1.F);
            mesh->push_point(0.F, 1.F, 1.F);

            mesh->push_cell(0, 1, 3);
            mesh->push_cell(3, 1, 2);
            mesh->push_cell(0, 4, 1);
            mesh->push_cell(1, 4, 5);
            mesh->push_cell(1, 5, 2);
            mesh->push_cell(5, 6, 2);
            mesh->push_cell(4, 5, 7);
            mesh->push_cell(5, 6, 7);
            mesh->push_cell(0, 4, 3);
            mesh->push_cell(4, 7, 3);
            mesh->push_cell(3, 7, 2);
            mesh->push_cell(7, 6, 2);

            const bool is_closed = sight::geometry::data::mesh::is_closed(mesh);
            CHECK_EQ(is_closed, true);
        }

        {
            sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
            const auto dump_lock         = mesh->dump_lock();

            mesh->push_point(0.F, 0.F, 0.F);
            mesh->push_point(1.F, 0.F, 0.F);
            mesh->push_point(1.F, 1.F, 0.F);
            mesh->push_point(0.F, 1.F, 0.F);
            mesh->push_point(0.F, 0.F, 1.F);
            mesh->push_point(1.F, 0.F, 1.F);
            mesh->push_point(1.F, 1.F, 1.F);
            mesh->push_point(0.F, 1.F, 1.F);

            mesh->push_cell(0, 1, 3);
            mesh->push_cell(3, 1, 2);
            mesh->push_cell(0, 4, 1);
            mesh->push_cell(1, 4, 5);
            mesh->push_cell(1, 5, 2);
            mesh->push_cell(5, 6, 2);
            mesh->push_cell(4, 5, 7);
            mesh->push_cell(5, 6, 7);
            mesh->push_cell(0, 4, 3);
            // removed this cell mesh->pushCell(4, 7, 3);
            mesh->push_cell(3, 7, 2);
            mesh->push_cell(7, 6, 2);

            const bool is_closed = sight::geometry::data::mesh::is_closed(mesh);
            CHECK_EQ(is_closed, false);
        }
    }

    TEST_CASE("cell_normal")
    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto dump_lock         = mesh->dump_lock();

        mesh->push_point(0.F, 0.F, 0.F);
        mesh->push_point(1.F, 0.F, 0.F);
        mesh->push_point(1.F, 1.F, 0.F);
        mesh->push_point(0.F, 1.F, 0.F);
        mesh->push_point(0.F, 0.F, 1.F);
        mesh->push_point(1.F, 0.F, 1.F);
        mesh->push_point(1.F, 1.F, 1.F);
        mesh->push_point(0.F, 1.F, 1.F);

        mesh->push_cell(0, 3, 2, 1);
        mesh->push_cell(0, 4, 5, 1);
        mesh->push_cell(1, 2, 6, 5);
        mesh->push_cell(4, 5, 6, 7);
        mesh->push_cell(0, 4, 7, 3);
        mesh->push_cell(3, 2, 6, 7);

        CHECK_NOTHROW(sight::geometry::data::mesh::generate_cell_normals(mesh));

        auto cell_iter = mesh->begin<cell::nxyz>();

        // check first cell normal = {0, 0, -1}
        std::array<float, 3> n = {0.F, 0.F, -1.F};
        CHECK_EQ(cell_iter->nx, n[0]);
        CHECK_EQ(cell_iter->ny, n[1]);
        CHECK_EQ(cell_iter->nz, n[2]);

        // check cell 2 normal = {0, 1, 0}
        ++cell_iter;
        n = {0.F, 1.F, 0.F};
        CHECK_EQ(cell_iter->nx, n[0]);
        CHECK_EQ(cell_iter->ny, n[1]);
        CHECK_EQ(cell_iter->nz, n[2]);

        // check cell 3 normal = {1, 0, 0}
        ++cell_iter;
        n = {1.F, 0.F, 0.F};
        CHECK_EQ(cell_iter->nx, n[0]);
        CHECK_EQ(cell_iter->ny, n[1]);
        CHECK_EQ(cell_iter->nz, n[2]);

        // check cell 4 normal = {0, 0, 1}
        ++cell_iter;
        n = {0.F, 0.F, 1.F};
        CHECK_EQ(cell_iter->nx, n[0]);
        CHECK_EQ(cell_iter->ny, n[1]);
        CHECK_EQ(cell_iter->nz, n[2]);

        // check cell 5 normal = {-1, 0, 0}
        ++cell_iter;
        n = {-1.F, 0.F, 0.F};
        CHECK_EQ(cell_iter->nx, n[0]);
        CHECK_EQ(cell_iter->ny, n[1]);
        CHECK_EQ(cell_iter->nz, n[2]);

        // check cell 6 normal = {0, -1, 0}
        ++cell_iter;
        n = {0.F, -1.F, 0.F};
        CHECK_EQ(cell_iter->nx, n[0]);
        CHECK_EQ(cell_iter->ny, n[1]);
        CHECK_EQ(cell_iter->nz, n[2]);
    }

    TEST_CASE("point_normal")
    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto dump_lock         = mesh->dump_lock();

        mesh->push_point(0.F, 0.F, 0.F);
        mesh->push_point(1.F, 0.F, 0.F);
        mesh->push_point(1.F, 1.F, 0.F);
        mesh->push_point(0.F, 1.F, 0.F);
        mesh->push_point(0.F, 0.F, 1.F);
        mesh->push_point(1.F, 0.F, 1.F);
        mesh->push_point(1.F, 1.F, 1.F);
        mesh->push_point(0.F, 1.F, 1.F);

        mesh->push_cell(0, 3, 2, 1);
        mesh->push_cell(0, 4, 5, 1);
        mesh->push_cell(1, 2, 6, 5);
        mesh->push_cell(4, 5, 6, 7);
        mesh->push_cell(0, 4, 7, 3);
        mesh->push_cell(3, 2, 6, 7);

        CHECK_NOTHROW(sight::geometry::data::mesh::generate_point_normals(mesh));

        auto point_iter = mesh->begin<point::nxyz>();

        // check first point normal = {-0.57735, 0.57735, -0.57735}
        const float epsilon    = 0.00001F;
        std::array<float, 3> n = {-std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>,
                                  -std::numbers::inv_sqrt3_v<float>
        };
        CHECK(point_iter->nx == doctest::Approx(n[0]).epsilon(epsilon));
        CHECK(point_iter->ny == doctest::Approx(n[1]).epsilon(epsilon));
        CHECK(point_iter->nz == doctest::Approx(n[2]).epsilon(epsilon));

        // check point 2 normal = {0.57735, 0.57735, -0.57735}
        ++point_iter;
        n = {std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>};
        CHECK(point_iter->nx == doctest::Approx(n[0]).epsilon(epsilon));
        CHECK(point_iter->ny == doctest::Approx(n[1]).epsilon(epsilon));
        CHECK(point_iter->nz == doctest::Approx(n[2]).epsilon(epsilon));

        // check point 3 normal = {0.57735, -0.57735, -0.57735}
        ++point_iter;
        n = {std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>};
        CHECK(point_iter->nx == doctest::Approx(n[0]).epsilon(epsilon));
        CHECK(point_iter->ny == doctest::Approx(n[1]).epsilon(epsilon));
        CHECK(point_iter->nz == doctest::Approx(n[2]).epsilon(epsilon));

        // check point 4 normal = {-0.57735, -0.57735, -0.57735}
        ++point_iter;
        n = {-std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>};
        CHECK(point_iter->nx == doctest::Approx(n[0]).epsilon(epsilon));
        CHECK(point_iter->ny == doctest::Approx(n[1]).epsilon(epsilon));
        CHECK(point_iter->nz == doctest::Approx(n[2]).epsilon(epsilon));

        // check point 5 normal = {-0.57735, 0.57735, 0.57735}
        ++point_iter;
        n = {-std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>};
        CHECK(point_iter->nx == doctest::Approx(n[0]).epsilon(epsilon));
        CHECK(point_iter->ny == doctest::Approx(n[1]).epsilon(epsilon));
        CHECK(point_iter->nz == doctest::Approx(n[2]).epsilon(epsilon));

        // check point 6 normal = {0.57735, 0.57735, 0.57735}
        ++point_iter;
        n = {std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>};
        CHECK(point_iter->nx == doctest::Approx(n[0]).epsilon(epsilon));
        CHECK(point_iter->ny == doctest::Approx(n[1]).epsilon(epsilon));
        CHECK(point_iter->nz == doctest::Approx(n[2]).epsilon(epsilon));

        // check point 7 normal = {0.57735, -0.57735, 0.57735}
        ++point_iter;
        n = {std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>};
        CHECK(point_iter->nx == doctest::Approx(n[0]).epsilon(epsilon));
        CHECK(point_iter->ny == doctest::Approx(n[1]).epsilon(epsilon));
        CHECK(point_iter->nz == doctest::Approx(n[2]).epsilon(epsilon));

        // check point 8 normal = {-0.57735, -0.57735, 0.57735}
        ++point_iter;
        n = {-std::numbers::inv_sqrt3_v<float>, -std::numbers::inv_sqrt3_v<float>, std::numbers::inv_sqrt3_v<float>};
        CHECK_EQ(n[0], doctest::Approx(point_iter->nx).epsilon(epsilon));
        CHECK_EQ(n[1], doctest::Approx(point_iter->ny).epsilon(epsilon));
        CHECK_EQ(n[2], doctest::Approx(point_iter->nz).epsilon(epsilon));
    }

    TEST_CASE("check_bounding_box_point")
    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto dump_lock         = mesh->dump_lock();
        sight::vec3d_t point_a       = {3., 4., 3.};
        sight::vec3d_t point_b       = {8., 9., 10.};

        sight::data::mesh::axis_aligned_box_t bbox_test = mesh->get_bounding_box();

        bbox_test.min[0] = 1.F;
        bbox_test.min[1] = 1.F;
        bbox_test.min[2] = 1.F;
        bbox_test.max[0] = 5.F;
        bbox_test.max[1] = 5.F;
        bbox_test.max[2] = 5.F;

        const bool is_inside_bounding_box_a = sight::geometry::data::mesh::is_inside_bounding_box(point_a, bbox_test);
        const bool is_inside_bounding_box_b = sight::geometry::data::mesh::is_inside_bounding_box(point_b, bbox_test);
        CHECK_EQ(is_inside_bounding_box_a, true);
        CHECK_EQ(is_inside_bounding_box_b, false);
    }

    TEST_CASE("merge_bounding_box")
    {
        sight::data::mesh::sptr mesh = std::make_shared<sight::data::mesh>();
        const auto dump_lock         = mesh->dump_lock();

        sight::data::mesh::axis_aligned_box_t bbox;
        bbox.min = {1.F, 1.F, 1.F};
        bbox.max = {5.F, 5.F, 5.F};

        sight::vec3f_t point = {8.F, 8.F, 8.F};

        auto bbox_merged = sight::geometry::data::mesh::merge_bounding_box(point, bbox);

        CHECK_EQ(bbox_merged.min[0], 1.F);
        CHECK_EQ(bbox_merged.min[1], 1.F);
        CHECK_EQ(bbox_merged.min[2], 1.F);
        CHECK_EQ(bbox_merged.max[0], 8.F);
        CHECK_EQ(bbox_merged.max[1], 8.F);
        CHECK_EQ(bbox_merged.max[2], 8.F);

        point       = {-2.F, 2.F, -4.F};
        bbox_merged = sight::geometry::data::mesh::merge_bounding_box(point, bbox_merged);

        CHECK_EQ(bbox_merged.min[0], -2.F);
        CHECK_EQ(bbox_merged.min[1], 1.F);
        CHECK_EQ(bbox_merged.min[2], -4.F);
        CHECK_EQ(bbox_merged.max[0], 8.F);
        CHECK_EQ(bbox_merged.max[1], 8.F);
        CHECK_EQ(bbox_merged.max[2], 8.F);
    }
}
