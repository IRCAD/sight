/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

namespace sight::io::igtl
{

namespace ut
{

class ClientServerTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ClientServerTest);
CPPUNIT_TEST(clientToServer);
CPPUNIT_TEST(serverToClient);
CPPUNIT_TEST(clientToServerTimeout);
CPPUNIT_TEST(clientHeaderExceptionTest);
CPPUNIT_TEST(serverHeaderExceptionTest);
CPPUNIT_TEST(clientBodyExceptionTest);
CPPUNIT_TEST(serverBodyExceptionTest);
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();
    void clientToServer();
    void clientToServerTimeout();
    void serverToClient();
    void clientHeaderExceptionTest();
    void serverHeaderExceptionTest();
    void clientBodyExceptionTest();
    void serverBodyExceptionTest();
};

} // namespace ut

} // namespace sight::io::igtl
