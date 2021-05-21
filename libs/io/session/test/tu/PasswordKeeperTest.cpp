/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "PasswordKeeperTest.hpp"

#include <io/session/PasswordKeeper.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::sight::io::session::ut::PasswordKeeperTest);

namespace sight::io::session
{

namespace ut
{

//------------------------------------------------------------------------------

void PasswordKeeperTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void PasswordKeeperTest::tearDown()
{
    // Clean up after the test run.
    // Reset global static password
    io::session::PasswordKeeper::setGlobalPassword("");
}

//------------------------------------------------------------------------------

void PasswordKeeperTest::globalTest()
{
    {
        // Set a dummy "abc" global password
        const io::session::PasswordKeeper globalKeeper;
        globalKeeper.setGlobalPassword("abc");

        // Verify the hash as "abc" have a well known sha256 hash (so never use abc as a password..)
        const core::crypto::secure_string sha256_abc("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
        CPPUNIT_ASSERT_EQUAL(sha256_abc, globalKeeper.getGlobalPasswordHash());
        CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), globalKeeper.getGlobalPassword());
        CPPUNIT_ASSERT(globalKeeper.checkGlobalPassword("abc"));
    }

    {
        // Test that we can get the "abc" password back, even with an other PasswordKeeper
        const io::session::PasswordKeeper globalKeeper;
        CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), globalKeeper.getGlobalPassword());
        CPPUNIT_ASSERT(globalKeeper.checkGlobalPassword("abc"));
    }
}

//------------------------------------------------------------------------------

void PasswordKeeperTest::localTest()
{
    {
        // Set a dummy "abc" global password
        io::session::PasswordKeeper localKeeper;
        localKeeper.setPassword("abc");

        // Verify the hash as "abc" have a well known sha256 hash (so never use abc as a password..)
        const core::crypto::secure_string sha256_abc("ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
        CPPUNIT_ASSERT_EQUAL(sha256_abc, localKeeper.getPasswordHash());
        CPPUNIT_ASSERT_EQUAL(core::crypto::secure_string("abc"), localKeeper.getPassword());
        CPPUNIT_ASSERT(localKeeper.checkPassword("abc"));
    }

    {
        // Test that we can NOT get the "abc" password back with an other PasswordKeeper
        const io::session::PasswordKeeper localKeeper;
        CPPUNIT_ASSERT(!localKeeper.checkPassword("abc"));
    }
}

} // namespace ut

} // namespace sight::io::session
