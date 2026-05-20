/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <data/fiducials_series.hpp>
#include <data/image_series.hpp>

#include <service/op.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::module::data::insert_fiducials")
{
//------------------------------------------------------------------------------

    TEST_CASE("insert")
    {
        auto srv = sight::service::add("sight::module::data::insert_fiducials");

        // Create a fiducials_series with one fiducial set
        auto fiducials = std::make_shared<sight::data::fiducials_series>();

        sight::data::fiducials_series::fiducial_set fiducial_set;
        fiducial_set.group_name = "Test Group";

        // Add a fiducial with a point
        sight::data::fiducials_series::fiducial fiducial;
        fiducial.shape_type          = sight::data::fiducials_series::shape::point;
        fiducial.fiducial_identifier = "Point1";
        fiducial.contour_data        = {{.x = 1.0, .y = 2.0, .z = 3.0}};

        fiducial_set.fiducial_sequence.push_back(fiducial);
        fiducials->append_fiducial_set(fiducial_set);

        // Create an empty image_series
        auto image = std::make_shared<sight::data::image_series>();

        // Configure the service
        srv->set_input(fiducials, "fiducials");
        srv->set_inout(image, "image");

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        // Execute the update
        CHECK_NOTHROW(srv->update().get());

        // Verify that the fiducials were inserted
        auto image_fiducials = image->get_fiducials();
        auto fiducial_sets   = image_fiducials->get_fiducial_sets();

        CHECK_EQ(std::size_t(1), fiducial_sets.size());
        CHECK_EQ(std::string("Test Group"), fiducial_sets[0].group_name.value());
        CHECK_EQ(std::size_t(1), fiducial_sets[0].fiducial_sequence.size());
        CHECK_EQ(std::string("Point1"), fiducial_sets[0].fiducial_sequence[0].fiducial_identifier);
        CHECK_EQ(false, fiducial_sets[0].frame_of_reference_uid->empty());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------

    TEST_CASE("insert_multiple_sets")
    {
        auto srv = sight::service::add("sight::module::data::insert_fiducials");

        // Create a fiducials_series with multiple fiducial sets
        auto fiducials = std::make_shared<sight::data::fiducials_series>();

        // First fiducial set
        sight::data::fiducials_series::fiducial_set fiducial_set1;
        fiducial_set1.group_name = "Group 1";

        sight::data::fiducials_series::fiducial fiducial1;
        fiducial1.shape_type          = sight::data::fiducials_series::shape::point;
        fiducial1.fiducial_identifier = "Point1";
        fiducial1.contour_data        = {{.x = 1.0, .y = 2.0, .z = 3.0}};
        fiducial_set1.fiducial_sequence.push_back(fiducial1);

        fiducials->append_fiducial_set(fiducial_set1);

        // Second fiducial set
        sight::data::fiducials_series::fiducial_set fiducial_set2;
        fiducial_set2.group_name = "Group 2";

        sight::data::fiducials_series::fiducial fiducial2;
        fiducial2.shape_type          = sight::data::fiducials_series::shape::line;
        fiducial2.fiducial_identifier = "Line1";
        fiducial2.contour_data        = {{.x = 4.0, .y = 5.0, .z = 6.0}, {.x = 7.0, .y = 8.0, .z = 9.0}};
        fiducial_set2.fiducial_sequence.push_back(fiducial2);

        fiducials->append_fiducial_set(fiducial_set2);

        // Create an empty image_series
        auto image = std::make_shared<sight::data::image_series>();

        // Configure the service
        srv->set_input(fiducials, "fiducials");
        srv->set_inout(image, "image");

        CHECK_NOTHROW(srv->configure());
        CHECK_NOTHROW(srv->start().get());

        // Execute the update
        CHECK_NOTHROW(srv->update().get());

        // Verify that both fiducial sets were inserted
        auto image_fiducials = image->get_fiducials();
        auto fiducial_sets   = image_fiducials->get_fiducial_sets();

        CHECK_EQ(std::size_t(2), fiducial_sets.size());

        // Check first set
        CHECK_EQ(std::string("Group 1"), fiducial_sets[0].group_name.value());
        CHECK_EQ(std::size_t(1), fiducial_sets[0].fiducial_sequence.size());
        CHECK_EQ(std::string("Point1"), fiducial_sets[0].fiducial_sequence[0].fiducial_identifier);
        CHECK_EQ(false, fiducial_sets[0].frame_of_reference_uid->empty());

        // Check second set
        CHECK_EQ(std::string("Group 2"), fiducial_sets[1].group_name.value());
        CHECK_EQ(std::size_t(1), fiducial_sets[1].fiducial_sequence.size());
        CHECK_EQ(std::string("Line1"), fiducial_sets[1].fiducial_sequence[0].fiducial_identifier);
        CHECK_EQ(std::size_t(2), fiducial_sets[1].fiducial_sequence[0].contour_data.size());
        CHECK_EQ(false, fiducial_sets[1].frame_of_reference_uid->empty());

        CHECK_NOTHROW(srv->stop().get());
        sight::service::remove(srv);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::module::data::insert_fiducials")
