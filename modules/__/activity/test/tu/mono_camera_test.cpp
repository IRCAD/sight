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

#include "mono_camera_test.hpp"

#include <activity/validator/base.hpp>
#include <activity/validator/object.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::activity::validator::ut::mono_camera_test);

namespace sight::module::activity::validator::ut
{

namespace factory = sight::activity::validator::factory;
using sight::activity::validator::object;
using sight::activity::validator::base;

//------------------------------------------------------------------------------

void mono_camera_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void mono_camera_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void mono_camera_test::testValidator()
{
    auto validator = factory::make("sight::module::activity::validator::camera_set::mono_camera");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::activity::validator::return_t validation;

    data::camera_set::sptr camera_set = std::make_shared<data::camera_set>();
    data::camera::sptr camera         = std::make_shared<data::camera>();
    data::camera::sptr camera2        = std::make_shared<data::camera>();

    {
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("CameraSet without camera should NOT be valid", false, validation.first);
    }
    {
        camera_set->add_camera(camera);
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with a non-calibrated camera should NOT be valid",
            false,
            validation.first
        );
    }
    {
        validation = obj_validator->validate(camera);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Validator on other object should not be valid",
            false,
            validation.first
        );
    }
    {
        camera->setIsCalibrated(true);
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with a calibrated camera should be valid",
            true,
            validation.first
        );
    }
    {
        camera2->setIsCalibrated(true);
        camera_set->add_camera(camera2);
        validation = obj_validator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with two cameras should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::activity::validator::ut
