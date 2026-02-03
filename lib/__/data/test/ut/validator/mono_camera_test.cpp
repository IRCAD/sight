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
#include <data/validator/base.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::validator::ut")
{
    namespace factory = sight::data::validator::factory;
    using sight::data::validator::base;

//------------------------------------------------------------------------------

    TEST_CASE("test_validator")
    {
        auto validator = factory::make("sight::data::validator::camera_set::mono_camera");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;

        sight::data::camera_set::sptr camera_set = std::make_shared<sight::data::camera_set>();
        sight::data::camera::sptr camera         = std::make_shared<sight::data::camera>();
        sight::data::camera::sptr camera2        = std::make_shared<sight::data::camera>();

        {
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(false == validation.first, "CameraSet without camera should NOT be valid");
        }
        {
            camera_set->add_camera(camera);
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(false == validation.first, "CameraSet with a non-calibrated camera should NOT be valid");
        }
        {
            validation = obj_validator->validate(camera);
            CHECK_MESSAGE(false == validation.first, "Validator on other object should not be valid");
        }
        {
            camera->set_is_calibrated(true);
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(true == validation.first, "CameraSet with a calibrated camera should be valid");
        }
        {
            camera2->set_is_calibrated(true);
            camera_set->add_camera(camera2);
            validation = obj_validator->validate(camera_set);
            CHECK_MESSAGE(false == validation.first, "CameraSet with two cameras should NOT be valid");
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::validator::ut")
