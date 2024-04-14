/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "password_keeper_test.hpp"

#include <core/crypto/password_keeper.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::crypto::ut::password_keeper_test);

namespace sight::core::crypto::ut
{

//------------------------------------------------------------------------------

void password_keeper_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void password_keeper_test::tearDown()
{
    // Clean up after the test run.
    // Reset global static password
    core::crypto::password_keeper::set_global_password("");
}

//------------------------------------------------------------------------------

void password_keeper_test::global_test()
{
    {
        // Set a dummy "abc" global password
        const core::crypto::password_keeper global_keeper;
        core::crypto::password_keeper::set_global_password("abc");

        // Verify the hash as "abc" have a well known sha256 hash (so never use abc as a password..)
        const core::crypto::secure_string sha256_abc("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
        CPPUNIT_ASSERT_EQUAL(sha256_abc, global_keeper.get_global_password_hash());
        CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), global_keeper.get_global_password());
        CPPUNIT_ASSERT(global_keeper.check_global_password("abc"));
    }

    {
        // Test that we can get the "abc" password back, even with an other password_keeper
        const core::crypto::password_keeper global_keeper;
        CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), global_keeper.get_global_password());
        CPPUNIT_ASSERT(global_keeper.check_global_password("abc"));
    }
}

//------------------------------------------------------------------------------

void password_keeper_test::local_test()
{
    {
        // Set a dummy "abc" global password
        core::crypto::password_keeper local_keeper;
        local_keeper.set_password("abc");

        // Verify the hash as "abc" have a well known sha256 hash (so never use abc as a password..)
        const core::crypto::secure_string sha256_abc("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
        CPPUNIT_ASSERT_EQUAL(sha256_abc, local_keeper.get_password_hash());
        CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), local_keeper.get_password());
        CPPUNIT_ASSERT(local_keeper.check_password("abc"));
    }

    {
        // Test that we can NOT get the "abc" password back with an other password_keeper
        const core::crypto::password_keeper local_keeper;
        CPPUNIT_ASSERT(!local_keeper.check_password("abc"));
    }
}

} // namespace sight::core::crypto::ut
