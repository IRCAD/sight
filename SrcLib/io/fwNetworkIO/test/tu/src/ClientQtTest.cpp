/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ClientQtTest.hpp"

#include <fwNetworkIO/helper/Series.hpp>

#include <fwGuiQt/WorkerQt.hpp>

#include <fwTest/Exception.hpp>

#include <cppunit/Exception.h>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwNetworkIO::ut::ClientQtTest );

namespace fwNetworkIO
{
namespace ut
{

unsigned char getAnswer[] = { /* Packet 3145 */
    0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x03, 0x2d, 0xcd, 0xcb, 0x0d, 0xc3, 0x30,
    0x08, 0x00, 0xd0, 0x7b, 0xa6, 0x88, 0x72, 0x2e,
    0x92, 0xb1, 0x01, 0xe3, 0x59, 0xaa, 0x1e, 0xcc,
    0x6f, 0xff, 0x11, 0x9a, 0x56, 0x19, 0xe0, 0xe9,
    0xbd, 0x8f, 0xf3, 0x3c, 0x2f, 0x25, 0xae, 0x62,
    0x13, 0xe8, 0x12, 0x4e, 0x91, 0x06, 0x3e, 0xdb,
    0xd8, 0x34, 0x1a, 0xf4, 0x8d, 0x42, 0xc6, 0x0c,
    0xb1, 0x78, 0x11, 0x86, 0x5c, 0xaf, 0x3f, 0x11,
    0x0c, 0xf7, 0x8d, 0x0a, 0xbb, 0x6c, 0x8f, 0xa8,
    0x9f, 0x5d, 0xd6, 0x3a, 0x0a, 0xb8, 0x4d, 0x1d,
    0xae, 0x05, 0x5d, 0x99, 0x65, 0x4e, 0x7f, 0xc8,
    0x3d, 0x20, 0x69, 0x0b, 0x98, 0x8a, 0x7d, 0xd9,
    0x1a, 0x90, 0x22, 0x99, 0x82, 0x03, 0xcc, 0xbd,
    0x17, 0xa6, 0x80, 0x9a, 0x17, 0xa3, 0xef, 0x87,
    0xe4, 0xb0, 0x59, 0x59, 0x04, 0x6b, 0xb2, 0xed,
    0x75, 0xdb, 0x56, 0xd6, 0xb7, 0x76, 0x86, 0x95,
    0xc5, 0x6d, 0x66, 0x42, 0x90, 0x4a, 0xb4, 0xa0,
    0xeb, 0xf8, 0x1c, 0x5f, 0x1d, 0x79, 0x13, 0xd0,
    0xcf, 0x00, 0x00, 0x00
};

static ::fwTest::Exception e(""); // force link with fwTest

//------------------------------------------------------------------------------

void ClientQtTest::setUp()
{
    // Set up context before running a test.
    char arg1[] = "ClientQtTest";
#if defined(__linux)
    char arg2[]         = "-platform";
    char arg3[]         = "offscreen";
    static char* argv[] = {arg1, arg2, arg3, 0};
    int argc            = 3;
#else
    static char* argv[] = {arg1, 0};
    int argc            = 1;
#endif

    CPPUNIT_ASSERT(qApp == NULL);
    m_worker = ::fwGuiQt::getQtWorker(argc, argv, false);

    m_server.listen();
}

//------------------------------------------------------------------------------

void ClientQtTest::waitForConnection()
{
    m_server.waitForNewConnection(5000);
    QTcpSocket* socket = m_server.nextPendingConnection();
    QByteArray data;
    while( socket->isOpen() && socket->waitForReadyRead() )
    {
        data += socket->readAll();

        if(data.endsWith("\r\n\r\n"))
        {
            break;
        }
    }
    socket->write( "HTTP/1.1 200 OK\n"
                   "Content-Type: application/json; charset=utf-8\n"
                   "Content-Encoding: gzip\n"
                   "Content-Length: 156\r\n\r\n" );
    socket->write(reinterpret_cast<char*>(getAnswer), sizeof(getAnswer));
    socket->waitForBytesWritten();

    delete socket;
}

//------------------------------------------------------------------------------

void ClientQtTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ClientQtTest::get()
{
    int port = m_server.serverPort();
    ::fwNetworkIO::http::Request::sptr request =
        ::fwNetworkIO::http::Request::New("http://localhost:" + std::to_string(port) + "/instances");

    m_worker->post(std::bind(&ClientQtTest::waitForConnection, this));
    QByteArray answer = m_client.get(request);

    QString expected("[\n"
                     "   \"845ff5b6-26dc4deb-c703a430-2a164b55-d95941d6\",\n"
                     "   \"61dcca18-afba3df6-269b0216-cb783c8f-2855677c\",\n"
                     "   \"f5b1480d-78129b93-e66ee613-bcc2f1e6-8bcf51ca\",\n"
                     "   \"e3b7fef4-975ba90d-0fb2a825-9ef507ee-d486d0d4\"\n"
                     "]\n");
    CPPUNIT_ASSERT_MESSAGE("Test get", QString(answer) == expected);

}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace fwNetworkIO
