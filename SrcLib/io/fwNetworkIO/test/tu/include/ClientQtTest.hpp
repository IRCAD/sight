/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwNetworkIO/http/ClientQt.hpp>

#include <fwThread/Worker.hpp>

#include <boost/filesystem.hpp>

#include <cppunit/extensions/HelperMacros.h>

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
