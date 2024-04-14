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
 * @brief Test many methods to read csv directory.
 */
class read_csv_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(read_csv_test);
CPPUNIT_TEST(test_valid_crlf_csv);
CPPUNIT_TEST(test_valid_lf_csv);
CPPUNIT_TEST(test_invalid_csv);
CPPUNIT_TEST(test_inexistence_csv);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    // test for inexistence of csv file
    void test_inexistence_csv();

    // test for valid CRLF csv file
    void test_valid_crlf_csv();

    // test for valid LF csv file
    void test_valid_lf_csv();

    // test for invalid csv file
    void test_invalid_csv();

protected:

    std::filesystem::path m_valid_crlf_csv_directory_path;
    std::filesystem::path m_valid_lf_csv_directory_path;
    std::filesystem::path m_invalid_csv_directory_path;
    std::filesystem::path m_wrong_filepath_directory;
};

} // namespace sight::io::ut
