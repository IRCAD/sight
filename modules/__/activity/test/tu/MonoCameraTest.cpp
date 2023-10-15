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

#include "MonoCameraTest.hpp"

#include <activity/validator/base.hpp>
#include <activity/validator/object.hpp>

#include <data/camera.hpp>
#include <data/camera_set.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::activity::validator::ut::MonoCameraTest);

namespace sight::module::activity::validator::ut
{

namespace factory = sight::activity::validator::factory;
using sight::activity::validator::object;
using sight::activity::validator::base;

//------------------------------------------------------------------------------

void MonoCameraTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MonoCameraTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MonoCameraTest::testValidator()
{
    auto validator = factory::make("sight::module::activity::validator::CameraSet::MonoCamera");
    CPPUNIT_ASSERT(validator);

    auto objValidator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
    CPPUNIT_ASSERT(objValidator);

    sight::activity::validator::return_t validation;

    data::camera_set::sptr camera_set = std::make_shared<data::camera_set>();
    data::camera::sptr camera         = std::make_shared<data::camera>();
    data::camera::sptr camera2        = std::make_shared<data::camera>();

    {
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("CameraSet without camera should NOT be valid", false, validation.first);
    }
    {
        camera_set->add_camera(camera);
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with a non-calibrated camera should NOT be valid",
            false,
            validation.first
        );
    }
    {
        validation = objValidator->validate(camera);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "Validator on other object should not be valid",
            false,
            validation.first
        );
    }
    {
        camera->setIsCalibrated(true);
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with a calibrated camera should be valid",
            true,
            validation.first
        );
    }
    {
        camera2->setIsCalibrated(true);
        camera_set->add_camera(camera2);
        validation = objValidator->validate(camera_set);
        CPPUNIT_ASSERT_EQUAL_MESSAGE(
            "CameraSet with two cameras should NOT be valid",
            false,
            validation.first
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::activity::validator::ut
