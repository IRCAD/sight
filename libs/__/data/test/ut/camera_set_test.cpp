/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include "camera_set_test.hpp"

#include <core/spy_log.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::camera_set_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void camera_set_test::setUp()
{
}

//------------------------------------------------------------------------------

void camera_set_test::tearDown()
{
}

//------------------------------------------------------------------------------

camera_set::sptr init_camera_set()
{
    auto camera_set = std::make_shared<data::camera_set>();

    // --------------- Camera 1 ----------------------
    auto camera1 = std::make_shared<camera>();
    camera1->set_cx(3.5);
    camera1->set_cy(9.3);
    camera1->set_fx(48.2);
    camera1->set_fy(7.3);
    camera1->set_skew(1.9);
    camera1->set_distortion_coefficient(1.1, 2.2, 3.3, 4.4, 5.5);
    camera_set->add_camera(camera1);

    // --------------- Camera 2 ----------------------
    auto camera2 = std::make_shared<camera>();
    camera2->set_cx(86.5);
    camera2->set_cy(543.);
    camera2->set_fx(4.4);
    camera2->set_fy(5.14);
    camera2->set_skew(0.19);
    camera2->set_distortion_coefficient(2.4, 5.1, 66., 4.1, 6.4);
    camera_set->add_camera(camera2);

    // --------------- Extrinsic matrix ----------------------
    auto mat = std::make_shared<matrix4>();
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            const auto value = static_cast<data::matrix4::value_type>(2 * i + j);
            (*mat)(i, j) = value;
        }
    }

    camera_set->set_extrinsic_matrix(1, mat);
    camera_set->set_calibration_error(0.123);

    return camera_set;
}

//------------------------------------------------------------------------------

void camera_set_test::camera_test()
{
    auto camera_set = std::make_shared<data::camera_set>();
    CPPUNIT_ASSERT(camera_set);

    auto identity = std::make_shared<matrix4>();
    auto mat      = std::make_shared<matrix4>();
    for(std::size_t i = 0 ; i < 4 ; ++i)
    {
        for(std::size_t j = 0 ; j < 4 ; ++j)
        {
            const auto value = static_cast<data::matrix4::value_type>(2 * i + j);
            (*mat)(i, j) = value;
        }
    }

    auto camera1 = std::make_shared<camera>();
    auto camera2 = std::make_shared<camera>();
    auto camera3 = std::make_shared<camera>();

    CPPUNIT_ASSERT_NO_THROW(camera_set->add_camera(camera1));
    CPPUNIT_ASSERT_NO_THROW(camera_set->add_camera(camera2));
    CPPUNIT_ASSERT_THROW(camera_set->add_camera(camera2), core::exception);

    CPPUNIT_ASSERT(camera_set->get_extrinsic_matrix(0));
    CPPUNIT_ASSERT(*identity == *camera_set->get_extrinsic_matrix(0));
    CPPUNIT_ASSERT(!camera_set->get_extrinsic_matrix(1));
    CPPUNIT_ASSERT_NO_THROW(camera_set->set_extrinsic_matrix(1, mat));
    CPPUNIT_ASSERT_THROW(camera_set->set_extrinsic_matrix(2, mat), std::out_of_range);
    CPPUNIT_ASSERT_THROW(camera_set->get_extrinsic_matrix(2), std::out_of_range);
    CPPUNIT_ASSERT(camera_set->get_extrinsic_matrix(1) == mat);

    CPPUNIT_ASSERT_NO_THROW(camera_set->set_calibration_error(0.4587));
    CPPUNIT_ASSERT_EQUAL(0.4587, camera_set->calibration_error());

    CPPUNIT_ASSERT_EQUAL(std::size_t(2), camera_set->size());

    CPPUNIT_ASSERT(camera_set->get_camera(0) == camera1);
    CPPUNIT_ASSERT(camera_set->get_camera(1) == camera2);
    CPPUNIT_ASSERT_THROW(camera_set->get_camera(2), std::out_of_range);

    CPPUNIT_ASSERT_NO_THROW(camera_set->add_camera(camera3));
    CPPUNIT_ASSERT(camera_set->get_camera(2) == camera3);

    CPPUNIT_ASSERT_NO_THROW(camera_set->remove_camera(camera1));
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), camera_set->size());
    CPPUNIT_ASSERT(camera_set->get_camera(0) == camera2);
    CPPUNIT_ASSERT_THROW(camera_set->remove_camera(camera1), core::exception);

    CPPUNIT_ASSERT_NO_THROW(camera_set->remove_camera(camera2));
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), camera_set->size());
    CPPUNIT_ASSERT(camera_set->get_camera(0) == camera3);
    CPPUNIT_ASSERT_NO_THROW(camera_set->remove_camera(camera3));

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), camera_set->size());
}

//------------------------------------------------------------------------------

void camera_set_test::shallow_copy_test()
{
    auto camera_set  = init_camera_set();
    auto camera_set2 = std::make_shared<data::camera_set>();

    camera_set2->shallow_copy(camera_set);

    CPPUNIT_ASSERT_EQUAL(camera_set->size(), camera_set2->size());
    CPPUNIT_ASSERT_EQUAL(camera_set->get_camera(0), camera_set2->get_camera(0));
    CPPUNIT_ASSERT_EQUAL(camera_set->get_camera(1), camera_set2->get_camera(1));
}

//------------------------------------------------------------------------------

void camera_set_test::deep_copy_test()
{
    camera_set::sptr camera_set = init_camera_set();
    camera_set::sptr camera_set2;
    camera_set2 = data::object::copy<data::camera_set>(camera_set);

    CPPUNIT_ASSERT(*camera_set == *camera_set2);
}

} // namespace sight::data::ut
