/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <core/runtime/runtime.hpp>

#include <data/matrix4.hpp>
#include <data/point.hpp>
#include <data/tools/picking_info.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

namespace
{

class context
{
public:

    context()
    {
        CHECK_NOTHROW(srv = sight::service::add("sight::module::geometry::matrix_to_point"));
        CHECK(srv != nullptr);
        CHECK(srv->is_a("sight::module::geometry::matrix_to_point"));

        point = std::make_shared<sight::data::point>();
        srv->set_inout(point, "point");
    }

    //------------------------------------------------------------------------------

    ~context()
    {
        if(srv->started())
        {
            CHECK_NOTHROW(srv->stop().wait());
        }

        sight::service::remove(srv);
    }

    //------------------------------------------------------------------------------

    void set_config(const std::string& _config)
    {
        srv->set_config(_config);

        srv->configure();
        srv->start().wait();
    }

    sight::service::base::sptr srv;
    sight::data::point::sptr point;
};

} // namespace

TEST_SUITE("sight::module::geometry::matrix_to_point")
{
    TEST_CASE_FIXTURE(context, "update_test")
    {
        auto matrix = std::make_shared<sight::data::matrix4>();
        srv->set_input(matrix, "matrix");
        set_config("");

        srv->update().get();

        CHECK_EQ(0., (*point)[0]);
        CHECK_EQ(0., (*point)[1]);
        CHECK_EQ(0., (*point)[2]);

        sight::data::matrix4::container_t expected;
        expected[3]  = 14.0;
        expected[7]  = 15.0;
        expected[11] = -112.5;
        *matrix      = expected;

        srv->update().get();

        CHECK_EQ(expected[3], (*point)[0]);
        CHECK_EQ(expected[7], (*point)[1]);
        CHECK_EQ(expected[11], (*point)[2]);
    }
} // TEST_SUITE
