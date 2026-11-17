/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <data/matrix4.hpp>
#include <data/string.hpp>
#include <data/validator/base.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::data::validator::is_identity")
{
    namespace factory = sight::data::validator::factory;

//------------------------------------------------------------------------------

    TEST_CASE("identity_matrix")
    {
        auto validator = factory::make("sight::data::validator::is_identity");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        sight::data::validator::return_t validation;
        sight::data::matrix4::sptr matrix = std::make_shared<sight::data::matrix4>();

        {
            // Default matrix4 is identity
            validation = obj_validator->validate(matrix);
            CHECK_EQ(true, validation.first);
        }
        {
            // Modify the matrix so it is no longer identity
            (*matrix)(0, 3) = 42.0;
            validation      = obj_validator->validate(matrix);
            CHECK_EQ(false, validation.first);
        }
        {
            // Reset to identity
            *matrix    = sight::data::matrix4::identity();
            validation = obj_validator->validate(matrix);
            CHECK_EQ(true, validation.first);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("wrong_object_type")
    {
        auto validator = factory::make("sight::data::validator::is_identity");
        CHECK(validator);

        auto obj_validator = std::dynamic_pointer_cast<sight::data::validator::base>(validator);
        CHECK(obj_validator);

        auto wrong_object     = std::make_shared<sight::data::string>();
        const auto validation = obj_validator->validate(wrong_object);
        CHECK_EQ(false, validation.first);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE("sight::data::validator::is_identity")
