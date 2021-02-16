/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "TagIdValidatorTest.hpp"

#include <activity/IObjectValidator.hpp>
#include <activity/IValidator.hpp>

#include <data/Composite.hpp>
#include <data/Integer.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <cstdint>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::activity::navigation::calibration::ut::TagIdValidatorTest );

namespace sight::activity::navigation::calibration
{
namespace ut
{

//------------------------------------------------------------------------------

void TagIdValidatorTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void TagIdValidatorTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void TagIdValidatorTest::testValidatorString()
{
    activity::IValidator::sptr validator =
        activity::validator::factory::New("::activity::navigation::calibration::validator::TagId");
    CPPUNIT_ASSERT(validator);

    activity::IObjectValidator::sptr objValidator =
        activity::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    activity::IValidator::ValidationType validation;

    {
        data::String::csptr str = data::String::New();
        validation = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty String should not be valid", false, validation.first);
    }
    {
        data::String::csptr str = data::String::New("101");
        validation = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with one tag should be valid", true, validation.first);
    }
    {
        data::String::csptr str = data::String::New("101,102,103");
        validation = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with several tags should be valid", true, validation.first);
    }
    {
        data::String::csptr str = data::String::New("bad");
        validation = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with value not a number should not be valid", false, validation.first);
    }
    {
        data::String::csptr str = data::String::New("1565");
        validation = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with value > 1023 should not be valid", false, validation.first);
    }
    {
        data::String::csptr str = data::String::New("101, bad, 103");
        validation = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with a wrong id should not be valid", false, validation.first);
    }
}

//------------------------------------------------------------------------------

void TagIdValidatorTest::testValidatorVector()
{
    activity::IValidator::sptr validator =
        activity::validator::factory::New("::activity::navigation::calibration::validator::TagId");
    CPPUNIT_ASSERT(validator);

    activity::IObjectValidator::sptr objValidator =
        activity::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    activity::IValidator::ValidationType validation;

    {
        data::Vector::sptr vector = data::Vector::New();
        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector should not be valid", false, validation.first);
    }
    {
        data::Vector::sptr vector = data::Vector::New();
        vector->getContainer().push_back(data::String::New("101, 102"));

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one valid String should be valid", true, validation.first);
    }
    {
        data::Vector::sptr vector = data::Vector::New();
        vector->getContainer().push_back(data::String::New("101, 102"));
        vector->getContainer().push_back(data::String::New("101, bad"));

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one non valid String should not be valid",
                                     false, validation.first);
    }
    {
        data::Vector::sptr vector = data::Vector::New();
        vector->getContainer().push_back(data::String::New("101, 102"));
        vector->getContainer().push_back(data::Integer::New(125));

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one non valid Integer should not be valid",
                                     false, validation.first);
    }
    {
        data::Vector::sptr vector = data::Vector::New();
        vector->getContainer().push_back(data::String::New("101, 102"));
        vector->getContainer().push_back(data::String::New("105, 1012"));

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with two valid String should be valid",
                                     true, validation.first);
    }
}

//------------------------------------------------------------------------------

void TagIdValidatorTest::testValidatorComposite()
{
    activity::IValidator::sptr validator =
        activity::validator::factory::New("::activity::navigation::calibration::validator::TagId");
    CPPUNIT_ASSERT(validator);

    activity::IObjectValidator::sptr objValidator =
        activity::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    activity::IValidator::ValidationType validation;

    {
        data::Composite::sptr composite = data::Composite::New();
        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector should not be valid", false, validation.first);
    }
    {
        data::Composite::sptr composite = data::Composite::New();
        (*composite)["key1"] = data::String::New("101, 102");
        validation           = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one valid String should be valid", true, validation.first);
    }
    {
        data::Composite::sptr composite = data::Composite::New();
        (*composite)["key1"] = data::String::New("101, 102");
        (*composite)["key2"] = data::String::New("101, bad");
        validation           = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one non valid String should not be valid",
                                     false, validation.first);
    }
    {
        data::Composite::sptr composite = data::Composite::New();
        (*composite)["key1"] = data::String::New("101, 102");
        (*composite)["key2"] = data::Integer::New(125);
        validation           = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one non valid Integer should not be valid",
                                     false, validation.first);
    }
    {
        data::Composite::sptr composite = data::Composite::New();
        (*composite)["key1"] = data::String::New("101, 102");
        (*composite)["key2"] = data::String::New("105, 1012");

        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with two valid String should be valid",
                                     true, validation.first);
    }

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::activity::navigation::calibration
