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

#include <data/integer.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

#include <memory>

TEST_SUITE("sight::module::geometry::vision::chessboard_creator")
{
//------------------------------------------------------------------------------

    TEST_CASE("creation")
    {
        auto chessboard = std::make_shared<sight::data::point_list>();

        auto board_width_prop       = std::make_shared<sight::data::integer>();
        auto board_height_prop      = std::make_shared<sight::data::integer>();
        auto board_square_size_prop = std::make_shared<sight::data::real>();

        board_width_prop->set_value(10);
        board_height_prop->set_value(6);
        board_square_size_prop->set_value(1.0);

        // Create service
        auto srv = sight::service::add("sight::module::geometry::vision::chessboard_creator");
        CHECK(srv->is_a("sight::module::geometry::vision::chessboard_creator"));
        CHECK(srv);

        srv->set_worker(sight::core::thread::get_default_worker());
        srv->set_inout(chessboard, "chessboard_model");
        srv->set_inout(board_width_prop, "board_width");
        srv->set_inout(board_height_prop, "board_height");
        srv->set_inout(board_square_size_prop, "board_square_size");
        srv->configure();
        srv->start().wait();
        srv->update().wait();

        CHECK_EQ(std::size_t(45), chessboard->size());

        // Check the first point
        auto pt = (*chessboard)[0];
        CHECK_EQ(0., (*pt)[0]);
        CHECK_EQ(0., (*pt)[1]);
        CHECK_EQ(0., (*pt)[2]);

        // Check the last point
        pt = (*chessboard)[chessboard->size() - 1];
        CHECK_EQ(4., (*pt)[0]);
        CHECK_EQ(8., (*pt)[1]);
        CHECK_EQ(0., (*pt)[2]);

        // Update the chessboard properties
        board_width_prop->set_value(5);
        board_height_prop->set_value(3);
        board_square_size_prop->set_value(2.0);
        srv->update().wait();
        CHECK_EQ(std::size_t(8), chessboard->size());
        // Check the first point
        pt = (*chessboard)[0];
        CHECK_EQ(0., (*pt)[0]);
        CHECK_EQ(0., (*pt)[1]);
        CHECK_EQ(0., (*pt)[2]);
        // Check the last point
        pt = (*chessboard)[chessboard->size() - 1];
        CHECK_EQ(2., (*pt)[0]);
        CHECK_EQ(6., (*pt)[1]);
        CHECK_EQ(0., (*pt)[2]);

        // Test with negative values
        board_width_prop->set_value(-5);
        board_height_prop->set_value(-3);
        board_square_size_prop->set_value(-2.0);
        srv->update().wait();
        CHECK_EQ(std::size_t(0), chessboard->size());

        srv->stop().wait();
        sight::service::remove(srv);
    }
} // TEST_SUITE
