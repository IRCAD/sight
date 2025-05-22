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

#include "chessboard_creator_test.hpp"

#include <data/integer.hpp>
#include <data/point_list.hpp>
#include <data/real.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <memory>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::filter::image::ut::chessboard_creator_test);

namespace sight::module::filter::image::ut
{

//------------------------------------------------------------------------------

void chessboard_creator_test::setUp()
{
}

//------------------------------------------------------------------------------

void chessboard_creator_test::tearDown()
{
}

//------------------------------------------------------------------------------

void chessboard_creator_test::creation_test()
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
    CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::vision::chessboard_creator"));
    CPPUNIT_ASSERT(srv);

    srv->set_worker(sight::core::thread::get_default_worker());
    srv->set_inout(chessboard, "chessboard_model");
    srv->set_inout(board_width_prop, "board_width");
    srv->set_inout(board_height_prop, "board_height");
    srv->set_inout(board_square_size_prop, "board_square_size");
    srv->configure();
    srv->start().wait();
    srv->update().wait();

    CPPUNIT_ASSERT_EQUAL(std::size_t(45), chessboard->get_points().size());

    // Check the first point
    auto pt = chessboard->get_points()[0];
    CPPUNIT_ASSERT_EQUAL(0., (*pt)[0]);
    CPPUNIT_ASSERT_EQUAL(0., (*pt)[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*pt)[2]);

    // Check the last point
    pt = chessboard->get_points()[chessboard->get_points().size() - 1];
    CPPUNIT_ASSERT_EQUAL(4., (*pt)[0]);
    CPPUNIT_ASSERT_EQUAL(8., (*pt)[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*pt)[2]);

    // Update the chessboard properties
    board_width_prop->set_value(5);
    board_height_prop->set_value(3);
    board_square_size_prop->set_value(2.0);
    srv->update().wait();
    CPPUNIT_ASSERT_EQUAL(std::size_t(8), chessboard->get_points().size());
    // Check the first point
    pt = chessboard->get_points()[0];
    CPPUNIT_ASSERT_EQUAL(0., (*pt)[0]);
    CPPUNIT_ASSERT_EQUAL(0., (*pt)[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*pt)[2]);
    // Check the last point
    pt = chessboard->get_points()[chessboard->get_points().size() - 1];
    CPPUNIT_ASSERT_EQUAL(2., (*pt)[0]);
    CPPUNIT_ASSERT_EQUAL(6., (*pt)[1]);
    CPPUNIT_ASSERT_EQUAL(0., (*pt)[2]);

    // Test with negative values
    board_width_prop->set_value(-5);
    board_height_prop->set_value(-3);
    board_square_size_prop->set_value(-2.0);
    srv->update().wait();
    CPPUNIT_ASSERT_EQUAL(std::size_t(0), chessboard->get_points().size());

    srv->stop().wait();
    sight::service::remove(srv);
}

} // namespace sight::module::filter::image::ut
