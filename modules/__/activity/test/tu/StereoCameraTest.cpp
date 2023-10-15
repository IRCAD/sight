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

#include "StereoCameraTest.hpp"

#include <activity/validator/base.hpp>
#include <activity/validator/object.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>
#include <data/matrix4.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::activity::validator::ut::StereoCameraTest);

namespace sight::module::activity::validator::ut
{

namespace factory = sight::activity::validator::factory;
using sight::activity::validator::object;
using sight::activity::validator::base;

//------------------------------------------------------------------------------

void StereoCameraTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void StereoCameraTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void StereoCameraTest::testValidator()
{
    auto validator = factory::make("sight::module::activity::validator::CameraSet::StereoCamera");
    CPPUNIT_ASSERT(validator);

    auto objValidator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(objValidator);

    sight::activity::validator::return_t validation;

    data::camera_set::sptr camera_set = std::make_shared<data::camera_set>();
    data::matrix4::sptr matrix        = std::make_shared<data::matrix4>();

    data::camera::sptr camera1 = std::make_shared<data::camera>();
    data::camera::sptr camera2 = std::make_shared<data::camera>();
    data::camera::sptr camera3 = std::make_shared<data::camera>();

    {
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("CameraSet without camera should be valid", false, validation.first);
    }
    {
        camera_set->add_camera(camera1);
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with a non-calibrated camera should NOT be valid",
            false,
            validation.first
        );
    }
    {
        validation = objValidator->validate(camera1);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Validator on other object should not be valid",
            false,
            validation.first
        );
    }
    {
        camera1->setIsCalibrated(true);
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with one calibrated camera should NOT be valid",
            false,
            validation.first
        );
    }
    {
        camera2->setIsCalibrated(true);
        camera_set->add_camera(camera2);
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with two calibrated cameras and no extrinsic matrix should NOT be "
            "valid",
            false,
            validation.first
        );
    }
    {
        camera_set->set_extrinsic_matrix(1, matrix);
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with two calibrated cameras and an extrinsic matrix should be valid",
            true,
            validation.first
        );
    }
    {
        camera2->setIsCalibrated(false);
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with two cameras (first calibrated and second not calibrated) "
            "should NOT be valid",
            false,
            validation.first
        );
    }
    {
        camera1->setIsCalibrated(false);
        camera2->setIsCalibrated(true);
        validation = objValidator->validate(camera_set);
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
