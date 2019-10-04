/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include <fwNetworkIO/http/ClientQt.hpp>

#include <fwThread/Worker.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <filesystem>
#include <QCoreApplication>
#include <QTcpServer>

namespace fwNetworkIO
{
namespace ut
{

class ClientQtTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ClientQtTest );
CPPUNIT_TEST( get );
CPPUNIT_TEST( post );
CPPUNIT_TEST_SUITE_END();

public:

    // Interface
    // Set up the application and the threads
    void setUp();
    // Clean up the application and the threads
    void tearDown();

    // Test functions
    // Simulates a GET request on Orthanc /instances route
    void get();
    // Simulates a POST request on Orthanc /tools/find route
    void post();

private:
    // Application thread
    ::fwThread::Worker::sptr m_worker;
    // HTTP client
    ::fwNetworkIO::http::ClientQt m_client;
    // Local server that will communicate with the client
    QTcpServer m_server;
    // Server thread
    QThread m_thread;
};

} // namespace ut
} // namespace fwNetworkIO
