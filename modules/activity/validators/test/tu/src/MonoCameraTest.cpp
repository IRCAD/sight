/************************************************************************
 *
 * Copyright (C) 2016 IRCAD France
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

#include <arData/CameraSeries.hpp>
#include <arData/Camera.hpp>

#include <fwActivities/IValidator.hpp>
#include <fwActivities/IObjectValidator.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::validators::ut::MonoCameraTest );

namespace validators
{
namespace ut
{

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
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::validators::CameraSeries::MonoCamera");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IObjectValidator::sptr objValidator = ::fwActivities::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    ::fwActivities::IValidator::ValidationType validation;

    ::arData::CameraSeries::sptr cameraSeries = ::arData::CameraSeries::New();
    ::arData::Camera::sptr camera             = ::arData::Camera::New();
    ::arData::Camera::sptr camera2            = ::arData::Camera::New();

    {
        validation = objValidator->validate(cameraSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("CameraSeries without camera should NOT be valid", false, validation.first);
    }
    {
        cameraSeries->addCamera(camera);
        validation = objValidator->validate(cameraSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("CameraSeries with a non-calibrated camera should NOT be valid",
                                     false, validation.first);
    }
    {
        validation = objValidator->validate(camera);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Validator on other object should not be valid",
                                     false, validation.first);
    }
    {
        camera->setIsCalibrated(true);
        validation = objValidator->validate(cameraSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("CameraSeries with a calibrated camera should be valid",
                                     true, validation.first);
    }
    {
        camera2->setIsCalibrated(true);
        cameraSeries->addCamera(camera2);
        validation = objValidator->validate(cameraSeries);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("CameraSeries with two cameras should NOT be valid",
                                     false, validation.first);
    }
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace validators

