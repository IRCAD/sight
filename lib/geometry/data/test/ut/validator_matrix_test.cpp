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
#include <data/validator/factory/new.hpp>

#include <geometry/data/validator/matrix.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::geometry::data::matrix::validator")
{
    using validator_t = sight::geometry::data::matrix::validator;
    namespace factory = sight::data::validator::factory;

    TEST_CASE("registered")
    {
        const auto validator = factory::make("sight::geometry::data::matrix::validator");

        CHECK(std::dynamic_pointer_cast<validator_t>(validator));
    }

    TEST_CASE("basic")
    {
        auto validator = std::make_shared<validator_t>();
        auto matrix    = std::make_shared<sight::data::matrix4>();

        const auto validation = validator->validate(matrix);

        CHECK(validation.first);
    }

    TEST_CASE("orthogonal")
    {
        auto validator                          = std::make_shared<validator_t>();
        auto matrix                             = std::make_shared<sight::data::matrix4>();
        sight::data::matrix4::container_t coefs = {
            0.001, 1., 0., .12,
            1., 0., 0., 12.,
            0., 0., -0.9999, 40.,
            0., 0., 0., 1.
        };

        *matrix = coefs;

        auto validation = validator->validate(matrix);

        CHECK_FALSE(validation.first);

        coefs = {
            0., 1., 0., .12,
            1., 0., 0., 12.,
            0., 0., -1.0, 40.,
            0., 0., 0., 1.
        };

        *matrix    = coefs;
        validation = validator->validate(matrix);

        CHECK(validation.first);
    }

    TEST_CASE("homogeneous")
    {
        auto validator                          = std::make_shared<validator_t>();
        auto matrix                             = std::make_shared<sight::data::matrix4>();
        sight::data::matrix4::container_t coefs = {
            1., 0., 0., 10.,
            0., 1., 0., 20.,
            0., 0., 1., -30.,
            1., 0.1, 0., 4.
        };

        *matrix = coefs;

        auto validation = validator->validate(matrix);

        CHECK_FALSE(validation.first);

        coefs = {
            1., 0., 0., 10.,
            0., 1., 0., 20.,
            0., 0., 1., -30.,
            0., 0., 0., 1.
        };

        *matrix    = coefs;
        validation = validator->validate(matrix);

        CHECK(validation.first);
    }

    TEST_CASE("empty_mat")
    {
        auto validator = std::make_shared<validator_t>();
        sight::data::matrix4::csptr matrix;

        const auto validation = validator->validate(matrix);

        CHECK_FALSE(validation.first);
    }

    TEST_CASE("wrong_object_type")
    {
        auto validator = std::make_shared<validator_t>();
        auto string    = std::make_shared<sight::data::string>();

        const auto validation = validator->validate(string);

        CHECK_FALSE(validation.first);
    }
}
