/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

namespace sight::module::io::session::ut
{

/**
 * @brief Test basic reader/writer for series_set.
 */
class session_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(session_test);
CPPUNIT_TEST(basic_archive_test);
CPPUNIT_TEST(basic_raw_test);
CPPUNIT_TEST(reader_bad_dialog_policy_test);
CPPUNIT_TEST(reader_bad_password_policy_test);
CPPUNIT_TEST(reader_bad_password_encryption_test);
CPPUNIT_TEST(reader_bad_file);
CPPUNIT_TEST(writer_bad_dialog_policy_test);
CPPUNIT_TEST(writer_bad_password_policy_test);
CPPUNIT_TEST(writer_bad_password_encryption_test);
CPPUNIT_TEST(file_dialog_test);
CPPUNIT_TEST(password_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void basic_archive_test();
    static void basic_raw_test();

    static void reader_bad_dialog_policy_test();
    static void reader_bad_password_policy_test();
    static void reader_bad_password_encryption_test();
    static void reader_bad_file();
    static void writer_bad_dialog_policy_test();
    static void writer_bad_password_policy_test();
    static void writer_bad_password_encryption_test();

    static void file_dialog_test();
    static void password_test();
};

} // namespace sight::module::io::session::ut
