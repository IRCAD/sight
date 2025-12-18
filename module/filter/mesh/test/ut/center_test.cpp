/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <data/matrix4.hpp>
#include <data/mesh.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>

//------------------------------------------------------------------------------

TEST_SUITE("sight::module::filter::mesh::center")
{
    class center_fixture
    {
    public:

        center_fixture()
        {
            srv = sight::service::add("sight::module::filter::mesh::center");
            REQUIRE(srv);
            REQUIRE(srv->is_a("sight::module::filter::mesh::center"));

            mesh    = std::make_shared<sight::data::mesh>();
            matrix4 = std::make_shared<sight::data::matrix4>();

            srv->set_inout(mesh, "mesh");
            srv->set_inout(matrix4, "transform");
        }

        ~center_fixture()
        {
            if(srv->started())
            {
                srv->stop().get();
            }

            sight::service::remove(srv);
        }

        // Utility to set points quickly
        void set_points(const std::vector<glm::dvec3>& _pts)
        {
            const auto lock = mesh->dump_lock();
            mesh->clear();
            mesh->reserve(
                static_cast<sight::data::mesh::size_t>(_pts.size()),
                1,
                sight::data::mesh::cell_type_t::point
            );
            for(const auto& p : _pts)
            {
                mesh->push_point({static_cast<float>(p[0]), static_cast<float>(p[1]), static_cast<float>(p[2])});
            }
        }

        sight::service::base::sptr srv;
        sight::data::mesh::sptr mesh;
        sight::data::matrix4::sptr matrix4;
    };

    TEST_CASE_FIXTURE(center_fixture, "empty_mesh")
    {
        srv->start().get();
        srv->update().get();

        const auto pos = matrix4->position();
        CHECK(pos[0] == doctest::Approx(0.0));
        CHECK(pos[1] == doctest::Approx(0.0));
        CHECK(pos[2] == doctest::Approx(0.0));
    }

    TEST_CASE_FIXTURE(center_fixture, "single_point")
    {
        set_points({{2.0, -1.0, 5.0}});

        srv->start().get();
        srv->update().get();

        const auto pos = matrix4->position();
        CHECK(pos[0] == doctest::Approx(2.0));
        CHECK(pos[1] == doctest::Approx(-1.0));
        CHECK(pos[2] == doctest::Approx(5.0));
    }

    TEST_CASE_FIXTURE(center_fixture, "box_corners")
    {
        set_points({{1.0, 2.0, 3.0}, {5.0, 6.0, 7.0}});

        srv->start().get();
        srv->update().get();

        const auto pos = matrix4->position();
        CHECK(pos[0] == doctest::Approx((1.0 + 5.0) * 0.5));
        CHECK(pos[1] == doctest::Approx((2.0 + 6.0) * 0.5));
        CHECK(pos[2] == doctest::Approx((3.0 + 7.0) * 0.5));
    }

    TEST_CASE_FIXTURE(center_fixture, "mixed_points")
    {
        set_points({{-10.0, 0.0, 3.0}, {2.0, -4.0, 9.0}, {7.0, 8.0, -1.0}});

        srv->start().get();
        srv->update().get();

        const auto pos = matrix4->position();
        CHECK(pos[0] == doctest::Approx((-10.0 + 7.0) * 0.5));
        CHECK(pos[1] == doctest::Approx((-4.0 + 8.0) * 0.5));
        CHECK(pos[2] == doctest::Approx((-1.0 + 9.0) * 0.5));
    }
}
