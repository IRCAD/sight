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

namespace sight::io::igtl::ut
{

class client_server_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(client_server_test);
CPPUNIT_TEST(client_to_server);
CPPUNIT_TEST(server_to_client);
CPPUNIT_TEST(client_to_server_timeout);
CPPUNIT_TEST(client_header_exception_test);
CPPUNIT_TEST(server_header_exception_test);
CPPUNIT_TEST(client_body_exception_test);
CPPUNIT_TEST(server_body_exception_test);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp() override;
    void tearDown() override;
    static void client_to_server();
    static void client_to_server_timeout();
    static void server_to_client();
    static void client_header_exception_test();
    static void server_header_exception_test();
    static void client_body_exception_test();
    static void server_body_exception_test();
};

} // namespace sight::io::igtl::ut
