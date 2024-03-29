/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <filesystem>

namespace sight::io::ut
{

/**
 * @brief Test many methods to read/create sight dictionary file.
 */
class dictionary_reader_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(dictionary_reader_test);
CPPUNIT_TEST(test_1);
CPPUNIT_TEST(test_2);
CPPUNIT_TEST(test_3);
CPPUNIT_TEST(test_4);
CPPUNIT_TEST(test_5);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    void test_1();
    void test_2();
    void test_3();
    void test_4();
    void test_5();

protected:

    static void generate_dictionary_file_with_missing_semi_colon(std::filesystem::path _dictionary_file);
    static void generate_dictionary_file_with_wrong_category(std::filesystem::path _dictionary_file);
    static void generate_dictionary_file_with_wrong_class(std::filesystem::path _dictionary_file);
    static void generate_dictionary_file(std::filesystem::path _dictionary_file);
    std::filesystem::path m_tmp_dictionary_file_path;
};

} // namespace sight::io::ut
