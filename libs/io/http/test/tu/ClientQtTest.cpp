/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

// cspell:ignore NOLINT

#include "ClientQtTest.hpp"

#include <io/http/helper/Series.hpp>

#include <ui/qt/App.hpp>
#include <ui/qt/WorkerQt.hpp>

#include <utest/Exception.hpp>

#include <array>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::http::ut::ClientQtTest);

namespace sight::io::http::ut
{

//------------------------------------------------------------------------------

std::uint8_t operator""_hhu(unsigned long long x) // NOLINT(google-runtime-int)
{
    return static_cast<std::uint8_t>(x);
}

std::array getAnswer { /* Packet 3145 */
    0x1f_hhu, 0x8b_hhu, 0x08_hhu, 0x00_hhu, 0x00_hhu, 0x00_hhu, 0x00_hhu, 0x00_hhu,
    0x00_hhu, 0x03_hhu, 0x2d_hhu, 0xcd_hhu, 0xcb_hhu, 0x0d_hhu, 0xc3_hhu, 0x30_hhu,
    0x08_hhu, 0x00_hhu, 0xd0_hhu, 0x7b_hhu, 0xa6_hhu, 0x88_hhu, 0x72_hhu, 0x2e_hhu,
    0x92_hhu, 0xb1_hhu, 0x01_hhu, 0xe3_hhu, 0x59_hhu, 0xaa_hhu, 0x1e_hhu, 0xcc_hhu,
    0x6f_hhu, 0xff_hhu, 0x11_hhu, 0x9a_hhu, 0x56_hhu, 0x19_hhu, 0xe0_hhu, 0xe9_hhu,
    0xbd_hhu, 0x8f_hhu, 0xf3_hhu, 0x3c_hhu, 0x2f_hhu, 0x25_hhu, 0xae_hhu, 0x62_hhu,
    0x13_hhu, 0xe8_hhu, 0x12_hhu, 0x4e_hhu, 0x91_hhu, 0x06_hhu, 0x3e_hhu, 0xdb_hhu,
    0xd8_hhu, 0x34_hhu, 0x1a_hhu, 0xf4_hhu, 0x8d_hhu, 0x42_hhu, 0xc6_hhu, 0x0c_hhu,
    0xb1_hhu, 0x78_hhu, 0x11_hhu, 0x86_hhu, 0x5c_hhu, 0xaf_hhu, 0x3f_hhu, 0x11_hhu,
    0x0c_hhu, 0xf7_hhu, 0x8d_hhu, 0x0a_hhu, 0xbb_hhu, 0x6c_hhu, 0x8f_hhu, 0xa8_hhu,
    0x9f_hhu, 0x5d_hhu, 0xd6_hhu, 0x3a_hhu, 0x0a_hhu, 0xb8_hhu, 0x4d_hhu, 0x1d_hhu,
    0xae_hhu, 0x05_hhu, 0x5d_hhu, 0x99_hhu, 0x65_hhu, 0x4e_hhu, 0x7f_hhu, 0xc8_hhu,
    0x3d_hhu, 0x20_hhu, 0x69_hhu, 0x0b_hhu, 0x98_hhu, 0x8a_hhu, 0x7d_hhu, 0xd9_hhu,
    0x1a_hhu, 0x90_hhu, 0x22_hhu, 0x99_hhu, 0x82_hhu, 0x03_hhu, 0xcc_hhu, 0xbd_hhu,
    0x17_hhu, 0xa6_hhu, 0x80_hhu, 0x9a_hhu, 0x17_hhu, 0xa3_hhu, 0xef_hhu, 0x87_hhu,
    0xe4_hhu, 0xb0_hhu, 0x59_hhu, 0x59_hhu, 0x04_hhu, 0x6b_hhu, 0xb2_hhu, 0xed_hhu,
    0x75_hhu, 0xdb_hhu, 0x56_hhu, 0xd6_hhu, 0xb7_hhu, 0x76_hhu, 0x86_hhu, 0x95_hhu,
    0xc5_hhu, 0x6d_hhu, 0x66_hhu, 0x42_hhu, 0x90_hhu, 0x4a_hhu, 0xb4_hhu, 0xa0_hhu,
    0xeb_hhu, 0xf8_hhu, 0x1c_hhu, 0x5f_hhu, 0x1d_hhu, 0x79_hhu, 0x13_hhu, 0xd0_hhu,
    0xcf_hhu, 0x00_hhu, 0x00_hhu, 0x00_hhu
};

std::array postAnswer { /* Packet 196 */
    0x1f_hhu, 0x8b_hhu, 0x08_hhu, 0x00_hhu, 0x00_hhu, 0x00_hhu, 0x00_hhu, 0x00_hhu,
    0x00_hhu, 0x03_hhu, 0x8b_hhu, 0x56_hhu, 0x50_hhu, 0x4a_hhu, 0x4b_hhu, 0x4b_hhu,
    0x35_hhu, 0x4c_hhu, 0x4c_hhu, 0x35_hhu, 0x33_hhu, 0xd7_hhu, 0xb5_hhu, 0xb0_hhu,
    0x30_hhu, 0x4f_hhu, 0x31_hhu, 0x49_hhu, 0x4b_hhu, 0x36_hhu, 0xd5_hhu, 0x35_hhu,
    0x31_hhu, 0x37_hhu, 0x37_hhu, 0x37_hhu, 0x4e_hhu, 0x4e_hhu, 0x35_hhu, 0xd0_hhu,
    0x35_hhu, 0x4c_hhu, 0x32_hhu, 0xb4_hhu, 0x34_hhu, 0x49_hhu, 0x35_hhu, 0x48_hhu,
    0xd5_hhu, 0x4d_hhu, 0xb6_hhu, 0x48_hhu, 0x4a_hhu, 0x33_hhu, 0x37_hhu, 0x33_hhu,
    0x31_hhu, 0x52_hhu, 0x52_hhu, 0x88_hhu, 0xe5_hhu, 0x02_hhu, 0x00_hhu, 0xe2_hhu,
    0x39_hhu, 0xc0_hhu, 0x49_hhu, 0x33_hhu, 0x00_hhu, 0x00_hhu, 0x00_hhu
};

//------------------------------------------------------------------------------

void ClientQtTest::setUp()
{
    // Set up context before running a test.
    static std::string arg1 = "ClientQtTest";
#if defined(__linux)
    static std::string arg2 = "-platform";
    static std::string arg3 = "offscreen";
    static std::array argv {arg1.data(), arg2.data(), arg3.data(), static_cast<char*>(nullptr)};
#else
    static std::array argv {arg1.data(), static_cast<char*>(nullptr)};
#endif
    static int argc = int(argv.size() - 1);

    CPPUNIT_ASSERT(qApp == nullptr);
    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback =
        [](int& argc, char** argv)
        {
            return QSharedPointer<QApplication>(new ui::qt::App(argc, argv, false));
        };
    m_worker = ui::qt::getQtWorker(argc, argv.data(), callback, "", "");

    m_server.moveToThread(&m_thread);
    QThread::connect(&m_thread, &QThread::started, [ = ]{m_server.listen();});
    QThread::connect(&m_thread, &QThread::finished, [ = ]{m_server.close();});
}

//------------------------------------------------------------------------------

void ClientQtTest::tearDown()
{
    // Clean up after the test run.
    m_thread.quit();
    m_thread.wait();

    m_thread.disconnect();
    m_server.disconnect();

    m_worker->post([]{return QCoreApplication::quit();});
    m_worker->getFuture().wait();
    m_worker.reset();

    CPPUNIT_ASSERT(qApp == nullptr);
}

//------------------------------------------------------------------------------

void ClientQtTest::get()
{
    QTcpServer::connect(
        &m_server,
        &QTcpServer::newConnection,
        [ = ]
        {
            QTcpSocket* socket = m_server.nextPendingConnection();
            QByteArray data;
            while(socket->isOpen() && socket->waitForReadyRead())
            {
                data += socket->readAll();

                if(data.endsWith("\r\n\r\n"))
                {
                    break;
                }
            }

            socket->write(
                "HTTP/1.1 200 OK\n"
                "Content-Type: application/json; charset=utf-8\n"
                "Content-Encoding: gzip\n"
                "Content-Length: 156\r\n\r\n"
            );
            socket->write(reinterpret_cast<char*>(getAnswer.data()), getAnswer.size());
            socket->waitForBytesWritten();

            delete socket;
        });

    m_thread.start();

    for(int i = 0 ; !m_server.isListening() && i < 10 ; ++i)
    {
        QThread::sleep(1);
    }

    CPPUNIT_ASSERT(m_server.isListening());

    const int port                         = m_server.serverPort();
    sight::io::http::Request::sptr request =
        sight::io::http::Request::New("http://localhost:" + std::to_string(port) + "/instances");

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
    QTcpServer::connect(
        &m_server,
        &QTcpServer::newConnection,
        [ = ]
        {
            QTcpSocket* socket = m_server.nextPendingConnection();
            QByteArray data;
            while(socket->isOpen() && socket->waitForReadyRead())
            {
                data += socket->readAll();

                if(data.endsWith("\r\n\r\n"))
                {
                    break;
                }
            }

            while(socket->isOpen() && socket->waitForReadyRead())
            {
                data += socket->readAll();

                if(data.endsWith("}\n}\n"))
                {
                    break;
                }
            }

            socket->write(
                "HTTP/1.1 200 OK\n"
                "Content-Type: application/json; charset=utf-8\n"
                "Content-Encoding: gzip\n"
                "Content-Length: 71\r\n\r\n"
            );
            socket->write(reinterpret_cast<char*>(postAnswer.data()), postAnswer.size());
            socket->waitForBytesWritten();

            delete socket;
        });

    m_thread.start();

    for(int i = 0 ; !m_server.isListening() && i < 10 ; ++i)
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

    auto request = sight::io::http::Request::New("http://localhost:" + std::to_string(port) + "/tools/find");

    const QByteArray& answer = m_client.post(request, QJsonDocument(body).toJson());
    QString expected("[ \"ffe1ae67-887d4fc5-47773ce0-1b194e0e-c8bf7642\" ]\n");
    CPPUNIT_ASSERT_MESSAGE("Test post", QString(answer) == expected);
}

//------------------------------------------------------------------------------

} // namespace sight::io::http::ut
