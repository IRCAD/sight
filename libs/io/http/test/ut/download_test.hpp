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

#include <io/http/download.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::http::ut
{

/// Unit test of download_file function (io/http/download.hpp)

class download_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(download_test);
CPPUNIT_TEST(download_test_success);
CPPUNIT_TEST(download_test_failure);
CPPUNIT_TEST(download_test_wrong_inputs);
CPPUNIT_TEST(download_test_overwrites_file);
CPPUNIT_TEST_SUITE_END();

public:

    /// Does nothing.
    void setUp() final;
    /// Does nothing.
    void tearDown() final;

    /// Tests success downloads.
    static void download_test_success();
    /// Tests failing download. (Exception)
    static void download_test_failure();
    /// Tests input of function. (Exception)
    static void download_test_wrong_inputs();
    /// Test if overwriting existing file. (Exception)
    static void download_test_overwrites_file();
};

} // namespace sight::io::http::ut
