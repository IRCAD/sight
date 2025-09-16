/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/crypto/password_keeper.hpp>

#include <doctest/doctest.h>

TEST_SUITE("sight::core::crypto::password")
{
//------------------------------------------------------------------------------

    TEST_CASE("global")
    {
        // Clean up before test
        sight::core::crypto::password_keeper::set_global_password("");

        {
            // Set a dummy "abc" global password
            const sight::core::crypto::password_keeper global_keeper;
            sight::core::crypto::password_keeper::set_global_password("abc");

            // Verify the hash as "abc" have a well known sha256 hash (so never use abc as a password..)
            const sight::core::crypto::secure_string sha256_abc(
                "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
            CHECK_EQ(global_keeper.get_global_password_hash(), sha256_abc);
            CHECK_EQ(global_keeper.get_global_password(), sight::core::crypto::secure_string("abc"));
            CHECK(global_keeper.check_global_password("abc"));
        }

        {
            // Test that we can get the "abc" password back, even with an other password_keeper
            const sight::core::crypto::password_keeper global_keeper;
            CHECK_EQ(global_keeper.get_global_password(), sight::core::crypto::secure_string("abc"));
            CHECK(global_keeper.check_global_password("abc"));
        }

        // Clean up after test
        sight::core::crypto::password_keeper::set_global_password("");
    }

//------------------------------------------------------------------------------

    TEST_CASE("local")
    {
        {
            // Set a dummy "abc" local password
            sight::core::crypto::password_keeper local_keeper;
            local_keeper.set_password("abc");

            // Verify the hash as "abc" have a well known sha256 hash (so never use abc as a password..)
            const sight::core::crypto::secure_string sha256_abc(
                "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
            CHECK_EQ(local_keeper.get_password_hash(), sha256_abc);
            CHECK_EQ(local_keeper.get_password(), sight::core::crypto::secure_string("abc"));
            CHECK(local_keeper.check_password("abc"));
        }

        {
            // Test that we can NOT get the "abc" password back with an other password_keeper
            const sight::core::crypto::password_keeper local_keeper;
            CHECK(!local_keeper.check_password("abc"));
        }
    }
} // TEST_SUITE
