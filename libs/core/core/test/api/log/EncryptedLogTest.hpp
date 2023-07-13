/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::core::log::ut
{

class EncryptedLogTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(EncryptedLogTest);
CPPUNIT_TEST(log_without_sink_test);
CPPUNIT_TEST(nominal_test);
CPPUNIT_TEST(bad_path_test);
CPPUNIT_TEST(basic_decryption_test);
CPPUNIT_TEST(password_change_decryption_test);
CPPUNIT_TEST(relocate_log_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void log_without_sink_test();
    static void nominal_test();
    static void bad_path_test();
    static void basic_decryption_test();
    static void password_change_decryption_test();
    static void relocate_log_test();
};

} // namespace sight::core::log::ut
