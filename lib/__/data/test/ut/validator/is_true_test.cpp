/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <data/boolean.hpp>
#include <data/integer.hpp>
#include <data/string.hpp>
#include <data/validator/base.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::validator::ut")
{
    namespace factory = sight::data::validator::factory;

//------------------------------------------------------------------------------

    TEST_CASE("boolean")
    {
        auto validator = factory::make("sight::data::validator::is_true");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;
        sight::data::boolean::sptr b = std::make_shared<sight::data::boolean>();
        b->set_value(false);
        validation = obj_validator->validate(b);
        CHECK_EQ(false, validation.first);

        b->set_value(true);
        validation = obj_validator->validate(b);
        CHECK_EQ(true, validation.first);
    }

//------------------------------------------------------------------------------

    TEST_CASE("integer")
    {
        auto validator = factory::make("sight::data::validator::is_true");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;
        sight::data::integer::sptr i = std::make_shared<sight::data::integer>();

        i->set_value(0);
        validation = obj_validator->validate(i);
        CHECK_EQ(false, validation.first);

        i->set_value(-1);
        validation = obj_validator->validate(i);
        CHECK_EQ(true, validation.first);

        i->set_value(1);
        validation = obj_validator->validate(i);
        CHECK_EQ(true, validation.first);

        i->set_value(10);
        validation = obj_validator->validate(i);
        CHECK_EQ(true, validation.first);

        i->from_string("1");
        validation = obj_validator->validate(i);
        CHECK_EQ(true, validation.first);

        i->from_string("-1");
        validation = obj_validator->validate(i);
        CHECK_EQ(true, validation.first);

        i->from_string("10");
        validation = obj_validator->validate(i);
        CHECK_EQ(true, validation.first);

        i->from_string("0");
        validation = obj_validator->validate(i);
        CHECK_EQ(false, validation.first);
    }

//------------------------------------------------------------------------------

    TEST_CASE("string")
    {
        auto validator = factory::make("sight::data::validator::is_true");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;
        sight::data::string::sptr s = std::make_shared<sight::data::string>();

        s->set_value("test");
        validation = obj_validator->validate(s);
        CHECK_EQ(false, validation.first);

        s->set_value("true");
        validation = obj_validator->validate(s);
        CHECK_EQ(true, validation.first);

        s->set_value("1");
        validation = obj_validator->validate(s);
        CHECK_EQ(true, validation.first);

        s->set_value("0");
        validation = obj_validator->validate(s);
        CHECK_EQ(false, validation.first);

        s->set_value("True");
        validation = obj_validator->validate(s);
        CHECK_EQ(true, validation.first);

        s->set_value("TRUE");
        validation = obj_validator->validate(s);
        CHECK_EQ(true, validation.first);

        s->set_value("trUe");
        validation = obj_validator->validate(s);
        CHECK_EQ(true, validation.first);

        // Some twisted mind values
        s->set_value("0 true"); // --> not "true" but "0" will be converted to false
        validation = obj_validator->validate(s);
        CHECK_EQ(false, validation.first);

        s->set_value("1 false"); // --> not "true" but "1" will be converted to true
        validation = obj_validator->validate(s);
        CHECK_EQ(true, validation.first);

        // Not converted to integers
        s->set_value("one");
        validation = obj_validator->validate(s);
        CHECK_EQ(false, validation.first);

        s->set_value("zero");
        validation = obj_validator->validate(s);
        CHECK_EQ(false, validation.first);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::validator::ut")
