/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

namespace sight::io::base
{

namespace ut
{

/**
 * @brief Test many methods to read csv directory.
 */
class ReadCsvTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ReadCsvTest);
CPPUNIT_TEST(test_validCRLFCsv);
CPPUNIT_TEST(test_validLFCsv);
CPPUNIT_TEST(test_invalidCsv);
CPPUNIT_TEST(test_inexistenceCsv);

CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    // test for inexistence of csv file
    void test_inexistenceCsv();

    // test for valid CRLF csv file
    void test_validCRLFCsv();

    // test for valid LF csv file
    void test_validLFCsv();

    // test for invalid csv file
    void test_invalidCsv();

protected:

    std::filesystem::path m_validCRLFCsvDirectoryPath;
    std::filesystem::path m_validLFCsvDirectoryPath;
    std::filesystem::path m_invalidCsvDirectoryPath;
    std::filesystem::path m_wrongFilepathDirectory;
};

} //namespace ut

} //namespace sight::io::base
