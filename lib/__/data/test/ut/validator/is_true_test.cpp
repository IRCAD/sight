/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "is_true_test.hpp"

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/string.hpp>
#include <data/validator/base.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::validator::ut::is_true_test);

namespace sight::data::validator::ut
{

namespace factory = sight::data::validator::factory;

//------------------------------------------------------------------------------

void is_true_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void is_true_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void is_true_test::boolean()
{
    auto validator = factory::make("sight::data::validator::is_true");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::data::validator::return_t validation;
    data::boolean::sptr b = std::make_shared<data::boolean>();
    b->set_value(false);
    validation = obj_validator->validate(b);
    CPPUNIT_ASSERT_EQUAL(false, validation.first);

    b->set_value(true);
    validation = obj_validator->validate(b);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);
}

//------------------------------------------------------------------------------

void is_true_test::integer()
{
    auto validator = factory::make("sight::data::validator::is_true");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::data::validator::return_t validation;
    data::integer::sptr i = std::make_shared<data::integer>();

    i->set_value(0);
    validation = obj_validator->validate(i);
    CPPUNIT_ASSERT_EQUAL(false, validation.first);

    i->set_value(-1);
    validation = obj_validator->validate(i);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    i->set_value(1);
    validation = obj_validator->validate(i);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    i->set_value(10);
    validation = obj_validator->validate(i);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    i->from_string("1");
    validation = obj_validator->validate(i);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    i->from_string("-1");
    validation = obj_validator->validate(i);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    i->from_string("10");
    validation = obj_validator->validate(i);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    i->from_string("0");
    validation = obj_validator->validate(i);
    CPPUNIT_ASSERT_EQUAL(false, validation.first);
}

//------------------------------------------------------------------------------

void is_true_test::string()
{
    auto validator = factory::make("sight::data::validator::is_true");
    CPPUNIT_ASSERT(validator);

    auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
    CPPUNIT_ASSERT(obj_validator);

    sight::data::validator::return_t validation;
    data::string::sptr s = std::make_shared<data::string>();

    s->set_value("test");
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(false, validation.first);

    s->set_value("true");
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    s->set_value("1");
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    s->set_value("0");
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(false, validation.first);

    s->set_value("True");
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    s->set_value("TRUE");
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    s->set_value("trUe");
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    // Some twisted mind values
    s->set_value("0 true"); // --> not "true" but "0" will be converted to false
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(false, validation.first);

    s->set_value("1 false"); // --> not "true" but "1" will be converted to true
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(true, validation.first);

    // Not converted to integers
    s->set_value("one");
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(false, validation.first);

    s->set_value("zero");
    validation = obj_validator->validate(s);
    CPPUNIT_ASSERT_EQUAL(false, validation.first);
}

//------------------------------------------------------------------------------

} // namespace sight::data::validator::ut
