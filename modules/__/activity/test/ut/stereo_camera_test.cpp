/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

#include "stereo_camera_test.hpp"

#include <activity/validator/base.hpp>
#include <activity/validator/object.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/matrix4.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::activity::validator::ut::stereo_camera_test);

namespace sight::module::activity::validator::ut
{

namespace factory = sight::activity::validator::factory;
using sight::activity::validator::object;
using sight::activity::validator::base;

//------------------------------------------------------------------------------

void stereo_camera_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void stereo_camera_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void stereo_camera_test::test_validator()
{
    auto validator = factory::make("sight::module::activity::validator::camera_set::stereo_camera");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::activity::validator::return_t validation;

    data::camera_set::sptr camera_set = std::make_shared<data::camera_set>();
    data::matrix4::sptr matrix        = std::make_shared<data::matrix4>();

    data::camera::sptr camera1 = std::make_shared<data::camera>();
    data::camera::sptr camera2 = std::make_shared<data::camera>();
    data::camera::sptr camera3 = std::make_shared<data::camera>();

    {
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("CameraSet without camera should be valid", false, validation.first);
    }
    {
        camera_set->add_camera(camera1);
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with a non-calibrated camera should NOT be valid",
            false,
            validation.first
        );
    }
    {
        validation = obj_validator->validate(camera1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Validator on other object should not be valid",
            false,
            validation.first
        );
    }
    {
        camera1->set_is_calibrated(true);
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with one calibrated camera should NOT be valid",
            false,
            validation.first
        );
    }
    {
        camera2->set_is_calibrated(true);
        camera_set->add_camera(camera2);
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with two calibrated cameras and no extrinsic matrix should NOT be "
            "valid",
            false,
            validation.first
        );
    }
    {
        camera_set->set_extrinsic_matrix(1, matrix);
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with two calibrated cameras and an extrinsic matrix should be valid",
            true,
            validation.first
        );
    }
    {
        camera2->set_is_calibrated(false);
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with two cameras (first calibrated and second not calibrated) "
            "should NOT be valid",
            false,
            validation.first
        );
    }
    {
        camera1->set_is_calibrated(false);
        camera2->set_is_calibrated(true);
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with two cameras (first not calibrated and second calibrated) "
            "should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::activity::validator::ut
