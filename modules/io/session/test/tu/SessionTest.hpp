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
 * @brief Test basic reader/writer for SeriesSet.
 */
class SessionTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(SessionTest);
CPPUNIT_TEST(basicArchiveTest);
CPPUNIT_TEST(basicRawTest);
CPPUNIT_TEST(readerBadDialogPolicyTest);
CPPUNIT_TEST(readerBadPasswordPolicyTest);
CPPUNIT_TEST(readerBadPasswordEncryptionTest);
CPPUNIT_TEST(readerBadFile);
CPPUNIT_TEST(writerBadDialogPolicyTest);
CPPUNIT_TEST(writerBadPasswordPolicyTest);
CPPUNIT_TEST(writerBadPasswordEncryptionTest);
CPPUNIT_TEST(fileDialogTest);
CPPUNIT_TEST(passwordTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void basicArchiveTest();
    static void basicRawTest();

    static void readerBadDialogPolicyTest();
    static void readerBadPasswordPolicyTest();
    static void readerBadPasswordEncryptionTest();
    static void readerBadFile();
    static void writerBadDialogPolicyTest();
    static void writerBadPasswordPolicyTest();
    static void writerBadPasswordEncryptionTest();

    static void fileDialogTest();
    static void passwordTest();
};

} // namespace sight::module::io::session::ut
