/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TagIdValidatorTest.hpp"

#include <fwActivities/IValidator.hpp>
#include <fwActivities/IObjectValidator.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Vector.hpp>

#include <cstdint>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::calibrationActivity::ut::TagIdValidatorTest );

namespace calibrationActivity
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
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::calibrationActivity::validator::TagId");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IObjectValidator::sptr objValidator =
        ::fwActivities::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    ::fwActivities::IValidator::ValidationType validation;

    {
        ::fwData::String::csptr str = ::fwData::String::New();
        validation                  = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty String should not be valid", false, validation.first);
    }
    {
        ::fwData::String::csptr str = ::fwData::String::New("101");
        validation                  = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with one tag should be valid", true, validation.first);
    }
    {
        ::fwData::String::csptr str = ::fwData::String::New("101,102,103");
        validation                  = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with several tags should be valid", true, validation.first);
    }
    {
        ::fwData::String::csptr str = ::fwData::String::New("bad");
        validation                  = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with value not a number should not be valid", false, validation.first);
    }
    {
        ::fwData::String::csptr str = ::fwData::String::New("1565");
        validation                  = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with value > 1023 should not be valid", false, validation.first);
    }
    {
        ::fwData::String::csptr str = ::fwData::String::New("101, bad, 103");
        validation                  = objValidator->validate(str);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("String with a wrong id should not be valid", false, validation.first);
    }
}

//------------------------------------------------------------------------------

void TagIdValidatorTest::testValidatorVector()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::calibrationActivity::validator::TagId");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IObjectValidator::sptr objValidator =
        ::fwActivities::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    ::fwActivities::IValidator::ValidationType validation;

    {
        ::fwData::Vector::sptr vector = ::fwData::Vector::New();
        validation                    = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector should not be valid", false, validation.first);
    }
    {
        ::fwData::Vector::sptr vector = ::fwData::Vector::New();
        vector->getContainer().push_back(::fwData::String::New("101, 102"));

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one valid String should be valid", true, validation.first);
    }
    {
        ::fwData::Vector::sptr vector = ::fwData::Vector::New();
        vector->getContainer().push_back(::fwData::String::New("101, 102"));
        vector->getContainer().push_back(::fwData::String::New("101, bad"));

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one non valid String should not be valid",
                                     false, validation.first);
    }
    {
        ::fwData::Vector::sptr vector = ::fwData::Vector::New();
        vector->getContainer().push_back(::fwData::String::New("101, 102"));
        vector->getContainer().push_back(::fwData::Integer::New(125));

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one non valid Integer should not be valid",
                                     false, validation.first);
    }
    {
        ::fwData::Vector::sptr vector = ::fwData::Vector::New();
        vector->getContainer().push_back(::fwData::String::New("101, 102"));
        vector->getContainer().push_back(::fwData::String::New("105, 1012"));

        validation = objValidator->validate(vector);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with two valid String should be valid",
                                     true, validation.first);
    }
}

//------------------------------------------------------------------------------

void TagIdValidatorTest::testValidatorComposite()
{
    ::fwActivities::IValidator::sptr validator =
        ::fwActivities::validator::factory::New("::calibrationActivity::validator::TagId");
    CPPUNIT_ASSERT(validator);

    ::fwActivities::IObjectValidator::sptr objValidator =
        ::fwActivities::IObjectValidator::dynamicCast(validator);
    CPPUNIT_ASSERT(objValidator);

    ::fwActivities::IValidator::ValidationType validation;

    {
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        validation                          = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector should not be valid", false, validation.first);
    }
    {
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        (*composite)["key1"]                = ::fwData::String::New("101, 102");
        validation                          = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one valid String should be valid", true, validation.first);
    }
    {
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        (*composite)["key1"]                = ::fwData::String::New("101, 102");
        (*composite)["key2"]                = ::fwData::String::New("101, bad");
        validation                          = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one non valid String should not be valid",
                                     false, validation.first);
    }
    {
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        (*composite)["key1"]                = ::fwData::String::New("101, 102");
        (*composite)["key2"]                = ::fwData::Integer::New(125);
        validation                          = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with one non valid Integer should not be valid",
                                     false, validation.first);
    }
    {
        ::fwData::Composite::sptr composite = ::fwData::Composite::New();
        (*composite)["key1"]                = ::fwData::String::New("101, 102");
        (*composite)["key2"]                = ::fwData::String::New("105, 1012");

        validation = objValidator->validate(composite);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Empty Vector with two valid String should be valid",
                                     true, validation.first);
    }

}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace calibrationActivity
