/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <core/tools/uuid.hpp>

#include <data/helper/fiducials_series.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::helper::ut")
{
//------------------------------------------------------------------------------

    TEST_CASE("to_point_list")
    {
        sight::data::fiducials_series::shape current_shape = sight::data::fiducials_series::shape::point;

        // Failing tests
        sight::data::fiducials_series::fiducial fiducial = sight::data::fiducials_series::fiducial();
        // When no uid is specified, conversion must fail
        CHECK(!sight::data::helper::fiducials_series::to_point_list(fiducial));

        fiducial              = sight::data::fiducials_series::fiducial();
        fiducial.fiducial_uid = sight::core::tools::uuid::generate();
        // When no data is specified -> FAIL
        CHECK(!sight::data::helper::fiducials_series::to_point_list(fiducial));

        // Point test
        current_shape         = sight::data::fiducials_series::shape::point;
        fiducial              = sight::data::fiducials_series::fiducial();
        fiducial.fiducial_uid = sight::core::tools::uuid::generate();
        fiducial.shape_type   = current_shape;
        // When no data is specified -> FAIL
        CHECK(!sight::data::helper::fiducials_series::to_point_list(fiducial));

        fiducial              = sight::data::fiducials_series::fiducial();
        fiducial.fiducial_uid = sight::core::tools::uuid::generate();
        fiducial.shape_type   = current_shape;
        fiducial.contour_data.push_back({0.0, 0.0, 0.0});
        CHECK(sight::data::helper::fiducials_series::to_point_list(fiducial));

        // Ruler test
        current_shape         = sight::data::fiducials_series::shape::ruler;
        fiducial              = sight::data::fiducials_series::fiducial();
        fiducial.fiducial_uid = sight::core::tools::uuid::generate();
        fiducial.shape_type   = current_shape;
        // When no data is specified -> FAIL
        CHECK(!sight::data::helper::fiducials_series::to_point_list(fiducial));

        fiducial              = sight::data::fiducials_series::fiducial();
        fiducial.fiducial_uid = sight::core::tools::uuid::generate();
        fiducial.shape_type   = current_shape;
        fiducial.contour_data.push_back({0.0, 0.0, 0.0});
        // Not enough points, should fail
        CHECK(!sight::data::helper::fiducials_series::to_point_list(fiducial));

        fiducial              = sight::data::fiducials_series::fiducial();
        fiducial.fiducial_uid = sight::core::tools::uuid::generate();
        fiducial.shape_type   = current_shape;
        fiducial.contour_data.push_back({0.0, 0.0, 0.0});
        fiducial.contour_data.push_back({1.0, 1.0, 1.0});
        CHECK(sight::data::helper::fiducials_series::to_point_list(fiducial));

        // Shape test
        current_shape         = sight::data::fiducials_series::shape::shape;
        fiducial              = sight::data::fiducials_series::fiducial();
        fiducial.fiducial_uid = sight::core::tools::uuid::generate();
        fiducial.shape_type   = current_shape;
        // When no data is specified -> FAIL
        CHECK(!sight::data::helper::fiducials_series::to_point_list(fiducial));

        fiducial              = sight::data::fiducials_series::fiducial();
        fiducial.fiducial_uid = sight::core::tools::uuid::generate();
        fiducial.shape_type   = current_shape;
        fiducial.contour_data.push_back({0.0, 0.0, 0.0});
        fiducial.contour_data.push_back({1.0, 1.0, 1.0});
        // Not enough points, should fail
        CHECK(!sight::data::helper::fiducials_series::to_point_list(fiducial));

        fiducial              = sight::data::fiducials_series::fiducial();
        fiducial.fiducial_uid = sight::core::tools::uuid::generate();
        fiducial.shape_type   = current_shape;
        fiducial.contour_data.push_back({0.0, 0.0, 0.0});
        fiducial.contour_data.push_back({1.0, 1.0, 1.0});
        fiducial.contour_data.push_back({0.0, 1.0, 0.0});
        CHECK(sight::data::helper::fiducials_series::to_point_list(fiducial));
    }
} // TEST_SUITE("sight::data::helper::ut")
