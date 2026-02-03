/************************************************************************
 *
 * Copyright (C) 2014-2026 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <core/spy_log.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::camera_set")
{
//------------------------------------------------------------------------------

    static sight::data::camera_set::sptr init_camera_set()
    {
        auto camera_set = std::make_shared<sight::data::camera_set>();

        // --------------- Camera 1 ----------------------
        auto camera1 = std::make_shared<sight::data::camera>();
        camera1->set_cx(3.5);
        camera1->set_cy(9.3);
        camera1->set_fx(48.2);
        camera1->set_fy(7.3);
        camera1->set_skew(1.9);
        camera1->set_distortion_coefficient(1.1, 2.2, 3.3, 4.4, 5.5);
        camera_set->add_camera(camera1);

        // --------------- Camera 2 ----------------------
        auto camera2 = std::make_shared<sight::data::camera>();
        camera2->set_cx(86.5);
        camera2->set_cy(543.);
        camera2->set_fx(4.4);
        camera2->set_fy(5.14);
        camera2->set_skew(0.19);
        camera2->set_distortion_coefficient(2.4, 5.1, 66., 4.1, 6.4);
        camera_set->add_camera(camera2);

        // --------------- Extrinsic matrix ----------------------
        auto mat = std::make_shared<sight::data::matrix4>();
        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                const auto value = static_cast<sight::data::matrix4::value_type>(2 * i + j);
                (*mat)(i, j) = value;
            }
        }

        camera_set->set_extrinsic_matrix(1, mat);
        camera_set->set_calibration_error(0.123);

        return camera_set;
    }

//------------------------------------------------------------------------------

    TEST_CASE("camera")
    {
        auto camera_set = std::make_shared<sight::data::camera_set>();
        CHECK(camera_set);

        auto identity = std::make_shared<sight::data::matrix4>();
        auto mat      = std::make_shared<sight::data::matrix4>();
        for(std::size_t i = 0 ; i < 4 ; ++i)
        {
            for(std::size_t j = 0 ; j < 4 ; ++j)
            {
                const auto value = static_cast<sight::data::matrix4::value_type>(2 * i + j);
                (*mat)(i, j) = value;
            }
        }

        auto camera1 = std::make_shared<sight::data::camera>();
        auto camera2 = std::make_shared<sight::data::camera>();
        auto camera3 = std::make_shared<sight::data::camera>();

        CHECK_NOTHROW(camera_set->add_camera(camera1));
        CHECK_NOTHROW(camera_set->add_camera(camera2));
        CHECK_THROWS_AS(camera_set->add_camera(camera2), sight::core::exception);

        CHECK(camera_set->get_extrinsic_matrix(0));
        CHECK(*identity == *camera_set->get_extrinsic_matrix(0));
        CHECK(!camera_set->get_extrinsic_matrix(1));
        CHECK_NOTHROW(camera_set->set_extrinsic_matrix(1, mat));
        CHECK_THROWS_AS(camera_set->set_extrinsic_matrix(2, mat), std::out_of_range);
        CHECK_THROWS_AS(camera_set->get_extrinsic_matrix(2), std::out_of_range);
        CHECK(camera_set->get_extrinsic_matrix(1) == mat);

        CHECK_NOTHROW(camera_set->set_calibration_error(0.4587));
        CHECK_EQ(0.4587, camera_set->calibration_error());

        CHECK_EQ(std::size_t(2), camera_set->size());

        CHECK(camera_set->get_camera(0) == camera1);
        CHECK(camera_set->get_camera(1) == camera2);
        CHECK_THROWS_AS(camera_set->get_camera(2), std::out_of_range);

        CHECK_NOTHROW(camera_set->add_camera(camera3));
        CHECK(camera_set->get_camera(2) == camera3);

        CHECK_NOTHROW(camera_set->remove_camera(camera1));
        CHECK_EQ(std::size_t(2), camera_set->size());
        CHECK(camera_set->get_camera(0) == camera2);
        CHECK_THROWS_AS(camera_set->remove_camera(camera1), sight::core::exception);

        CHECK_NOTHROW(camera_set->remove_camera(camera2));
        CHECK_EQ(std::size_t(1), camera_set->size());
        CHECK(camera_set->get_camera(0) == camera3);
        CHECK_NOTHROW(camera_set->remove_camera(camera3));

        CHECK_EQ(std::size_t(0), camera_set->size());
    }

//------------------------------------------------------------------------------

    TEST_CASE("shallow_copy")
    {
        auto camera_set  = init_camera_set();
        auto camera_set2 = std::make_shared<sight::data::camera_set>();

        camera_set2->shallow_copy(camera_set);

        CHECK_EQ(camera_set->size(), camera_set2->size());
        CHECK_EQ(camera_set->get_camera(0), camera_set2->get_camera(0));
        CHECK_EQ(camera_set->get_camera(1), camera_set2->get_camera(1));
    }

//------------------------------------------------------------------------------

    TEST_CASE("deep_copy")
    {
        sight::data::camera_set::sptr camera_set = init_camera_set();
        sight::data::camera_set::sptr camera_set2;
        camera_set2 = sight::data::object::copy<sight::data::camera_set>(camera_set);

        CHECK(*camera_set == *camera_set2);
    }
} // TEST_SUITE("sight::data::camera_set")
