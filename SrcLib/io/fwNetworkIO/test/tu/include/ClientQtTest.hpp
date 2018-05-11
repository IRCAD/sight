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
CPPUNIT_TEST_SUITE_END();

public:

    // Interface
    void setUp();
    void tearDown();
    void waitForConnection();

    // Test functions
    void get();

private:
    ::fwThread::Worker::sptr m_worker;
    ::fwNetworkIO::http::ClientQt m_client;
    QTcpServer m_server;
};

} // namespace ut
} // namespace fwNetworkIO
