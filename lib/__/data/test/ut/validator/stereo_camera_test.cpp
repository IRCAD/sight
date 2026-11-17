/************************************************************************
 *
 * Copyright (C) 2020-2026 IRCAD France
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

#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/matrix4.hpp>
#include <data/validator/base.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::validator")
{
    namespace factory = sight::data::validator::factory;
    using sight::data::validator::base;

//------------------------------------------------------------------------------

    TEST_CASE("stereo_camera")
    {
        auto validator = factory::make("sight::data::validator::camera_set::stereo_camera");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;

        sight::data::camera_set::sptr camera_set = std::make_shared<sight::data::camera_set>();
        sight::data::matrix4::sptr matrix        = std::make_shared<sight::data::matrix4>();

        sight::data::camera::sptr camera1 = std::make_shared<sight::data::camera>();
        sight::data::camera::sptr camera2 = std::make_shared<sight::data::camera>();
        sight::data::camera::sptr camera3 = std::make_shared<sight::data::camera>();

        {
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(false == validation.first, "CameraSet without camera should be valid");
        }
        {
            camera_set->add_camera(camera1);
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(false == validation.first, "CameraSet with a non-calibrated camera should NOT be valid");
        }
        {
            validation = obj_validator->validate(camera1);
            CHECK_MESSAGE(false == validation.first, "Validator on other object should not be valid");
        }
        {
            camera1->set_is_calibrated(true);
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(false == validation.first, "CameraSet with one calibrated camera should NOT be valid");
        }
        {
            camera2->set_is_calibrated(true);
            camera_set->add_camera(camera2);
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(
                false == validation.first,
                "CameraSet with two calibrated cameras and no extrinsic matrix should NOT be "
                "valid"
            );
        }
        {
            camera_set->set_extrinsic_matrix(1, matrix);
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(
                true == validation.first,
                "CameraSet with two calibrated cameras and an extrinsic matrix should be valid"
            );
        }
        {
            camera2->set_is_calibrated(false);
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(
                false == validation.first,
                "CameraSet with two cameras (first calibrated and second not calibrated) "
                "should NOT be valid"
            );
        }
        {
            camera1->set_is_calibrated(false);
            camera2->set_is_calibrated(true);
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(
                false == validation.first,
                "CameraSet with two cameras (first not calibrated and second calibrated) "
                "should NOT be valid"
            );
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::validator")
