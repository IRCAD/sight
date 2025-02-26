/************************************************************************
 *
 * Copyright (C) 2022-2024 IRCAD France
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

class encrypted_log_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(encrypted_log_test);
CPPUNIT_TEST(log_without_sink_test);
CPPUNIT_TEST(bad_path_test);
CPPUNIT_TEST(bad_password_test);
CPPUNIT_TEST(corner_case_test);
CPPUNIT_TEST(nominal_raw_log_test);
CPPUNIT_TEST(nominal_compressed_log_test);
CPPUNIT_TEST(nominal_encrypted_log_test);
CPPUNIT_TEST(multiple_logger_test);
CPPUNIT_TEST(raw_to_encrypted_test);
CPPUNIT_TEST(compressed_to_encrypted_test);
CPPUNIT_TEST(password_change_test);
CPPUNIT_TEST(relocate_test);
CPPUNIT_TEST(crash_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void log_without_sink_test();
    static void bad_path_test();
    static void bad_password_test();
    static void corner_case_test();

    static void nominal_raw_log_test();
    static void nominal_compressed_log_test();
    static void nominal_encrypted_log_test();

    static void multiple_logger_test();

    static void raw_to_encrypted_test();
    static void compressed_to_encrypted_test();
    static void password_change_test();
    static void relocate_test();
    static void crash_test();
};

} // namespace sight::core::log::ut
