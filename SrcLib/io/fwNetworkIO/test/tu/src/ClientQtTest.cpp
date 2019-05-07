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

#include "ClientQtTest.hpp"

#include <fwNetworkIO/helper/Series.hpp>

#include <fwGuiQt/App.hpp>

#include <fwQt/WorkerQt.hpp>

#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwTest/Exception.hpp>

#include <cppunit/Exception.h>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwNetworkIO::ut::ClientQtTest );

namespace fwNetworkIO
{
namespace ut
{

std::uint8_t getAnswer[] = { /* Packet 3145 */
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

std::uint8_t postAnswer[] = { /* Packet 196 */
    0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x03, 0x8b, 0x56, 0x50, 0x4a, 0x4b, 0x4b,
    0x35, 0x4c, 0x4c, 0x35, 0x33, 0xd7, 0xb5, 0xb0,
    0x30, 0x4f, 0x31, 0x49, 0x4b, 0x36, 0xd5, 0x35,
    0x31, 0x37, 0x37, 0x37, 0x4e, 0x4e, 0x35, 0xd0,
    0x35, 0x4c, 0x32, 0xb4, 0x34, 0x49, 0x35, 0x48,
    0xd5, 0x4d, 0xb6, 0x48, 0x4a, 0x33, 0x37, 0x33,
    0x31, 0x52, 0x52, 0x88, 0xe5, 0x02, 0x00, 0xe2,
    0x39, 0xc0, 0x49, 0x33, 0x00, 0x00, 0x00
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
    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback = [this](int& argc, char** argv)
                                                                             {
                                                                                 return QSharedPointer< QApplication > ( new ::fwGuiQt::App(
                                                                                                                             argc,
                                                                                                                             argv,
                                                                                                                             false) );
                                                                             };
    m_worker = ::fwQt::getQtWorker(argc, argv, callback);

    m_server.moveToThread(&m_thread);
    m_thread.connect(&m_thread, &QThread::started, [ = ] {m_server.listen(); });
    m_thread.connect(&m_thread, &QThread::finished, [ = ] {m_server.close(); });
}

//------------------------------------------------------------------------------

void ClientQtTest::tearDown()
{
    // Clean up after the test run.
    m_thread.quit();
    m_thread.wait();

    m_thread.disconnect();
    m_server.disconnect();

    m_worker->post( std::bind( &QCoreApplication::quit ) );
    m_worker->getFuture().wait();
    m_worker.reset();

    ::fwServices::registry::ActiveWorkers::getDefault()->clearRegistry();
    CPPUNIT_ASSERT(qApp == NULL);
}

//------------------------------------------------------------------------------

void ClientQtTest::get()
{
    m_server.connect( &m_server, &QTcpServer::newConnection, [ = ]
            {
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
            });

    m_thread.start();

    for(int i = 0; !m_server.isListening() && i < 10; ++i)
    {
        QThread::sleep(1);
    }

    CPPUNIT_ASSERT(m_server.isListening());

    const int port = m_server.serverPort();
    ::fwNetworkIO::http::Request::sptr request =
        ::fwNetworkIO::http::Request::New("http://localhost:" + std::to_string(port) + "/instances");

    const QByteArray& answer = m_client.get(request);

    QString expected("[\n"
                     "   \"845ff5b6-26dc4deb-c703a430-2a164b55-d95941d6\",\n"
                     "   \"61dcca18-afba3df6-269b0216-cb783c8f-2855677c\",\n"
                     "   \"f5b1480d-78129b93-e66ee613-bcc2f1e6-8bcf51ca\",\n"
                     "   \"e3b7fef4-975ba90d-0fb2a825-9ef507ee-d486d0d4\"\n"
                     "]\n");
    CPPUNIT_ASSERT_MESSAGE("Test get", QString(answer) == expected);
}

//------------------------------------------------------------------------------

void ClientQtTest::post()
{
    m_server.connect( &m_server, &QTcpServer::newConnection, [ = ]
            {
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
                while( socket->isOpen() && socket->waitForReadyRead() )
                {
                    data += socket->readAll();

                    if(data.endsWith("}\n}\n"))
                    {
                        break;
                    }
                }
                socket->write( "HTTP/1.1 200 OK\n"
                               "Content-Type: application/json; charset=utf-8\n"
                               "Content-Encoding: gzip\n"
                               "Content-Length: 71\r\n\r\n" );
                socket->write(reinterpret_cast<char*>(postAnswer), sizeof(postAnswer));
                socket->waitForBytesWritten();

                delete socket;
            });

    m_thread.start();

    for(int i = 0; !m_server.isListening() && i < 10; ++i)
    {
        QThread::sleep(1);
    }

    CPPUNIT_ASSERT(m_server.isListening());

    const int port = m_server.serverPort();

    QJsonObject query;
    query.insert("SeriesInstanceUID", "1.2.392.200036.9116.2.6.1.48.1211418863.1225184516.765855");

    QJsonObject body;
    body.insert("Level", "Series");
    body.insert("Query", query);
    body.insert("Limit", 0);

    ::fwNetworkIO::http::Request::sptr request =
        ::fwNetworkIO::http::Request::New("http://localhost:" + std::to_string(port) + "/tools/find");

    const QByteArray& answer = m_client.post(request, QJsonDocument(body).toJson());
    QString expected("[ \"ffe1ae67-887d4fc5-47773ce0-1b194e0e-c8bf7642\" ]\n");
    CPPUNIT_ASSERT_MESSAGE("Test post", QString(answer) == expected);
}

//------------------------------------------------------------------------------

} // namespace ut

} // namespace fwNetworkIO
