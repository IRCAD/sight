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

#include "ClientServerTest.hpp"

#include <io/igtl/Client.hpp>
#include <io/igtl/detail/MessageFactory.hpp>
#include <io/igtl/Exception.hpp>
#include <io/igtl/Server.hpp>

#include <igtlStringMessage.h>

#include <thread>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::igtl::ut::ClientServerTest);

namespace sight::io::igtl::ut
{

// Global variable for client.
static sight::io::igtl::Client::sptr s_client;
// Global variable for server.
static sight::io::igtl::Server::sptr s_server;
// Global variable that stores the runServer future.
static std::future<void> s_serverFuture;

//------------------------------------------------------------------------------

void ClientServerTest::setUp()
{
    // Create server
    s_server = std::make_shared<sight::io::igtl::Server>();
    // Using 0 as port number will check for the first available port number.
    s_server->start(0);
    s_server->addAuthorizedDevice("Sight_Tests_Client");
    s_server->setReceiveTimeout(30);

    // Run server (waitForConnection on a thread).
    s_serverFuture = std::async(std::launch::async, [ObjectPtr = s_server](auto&& ...){ObjectPtr->runServer();});

    // Wait for server to be initialized properly.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Create client.
    s_client = std::make_shared<sight::io::igtl::Client>();

    // Connect client.
    s_client->connect("127.0.0.1", s_server->getPort());
    s_client->setDeviceNameOut("Sight_Tests_Client");
    s_client->addAuthorizedDevice("Sight_Tests_Server");

    // Hang until we have a client or we reach the timeout.
    std::uint32_t timeout = 0;
    while(s_server->numClients() == 0 || timeout >= 1000)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout += 100;
    }

    // Set Name to connected client.
    s_server->setMessageDeviceName("Sight_Tests_Server");
}

//------------------------------------------------------------------------------

void ClientServerTest::tearDown()
{
    if(s_client->isConnected())
    {
        s_client->disconnect();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if(s_server->isStarted())
    {
        s_server->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        s_serverFuture.wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

//------------------------------------------------------------------------------

void ClientServerTest::clientToServer()
{
    ::igtl::StringMessage::Pointer stringMsg = ::igtl::StringMessage::New();
    stringMsg->SetString("Hello world!");

    s_client->sendMsg(static_cast< ::igtl::MessageBase::Pointer>(stringMsg));

    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->isStarted());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->isConnected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->numClients() == 1);

    std::vector< ::igtl::MessageHeader::Pointer> headers;
    CPPUNIT_ASSERT_NO_THROW(headers = s_server->receiveHeaders());

    for(const auto& header : headers)
    {
        CPPUNIT_ASSERT_MESSAGE("Device Name", std::string(header->GetDeviceName()) == "Sight_Tests_Client");
        CPPUNIT_ASSERT_MESSAGE("Device Type", std::string(header->GetDeviceType()) == stringMsg->GetDeviceType());

        ::igtl::MessageBase::Pointer msg;
        CPPUNIT_ASSERT_NO_THROW(msg = s_server->receiveBody(header, 0));

        CPPUNIT_ASSERT_MESSAGE("Body message", msg);

        ::igtl::StringMessage::Pointer receivedStringMsg =
            ::igtl::StringMessage::Pointer(
                dynamic_cast<::igtl::StringMessage*>(msg.
                                                     GetPointer())
            );

        CPPUNIT_ASSERT_MESSAGE("Received IGTL Message", receivedStringMsg);
        CPPUNIT_ASSERT_MESSAGE(
            "Value of message",
            std::string(receivedStringMsg->GetString())
            == std::string(stringMsg->GetString())
        );
    }
}

//------------------------------------------------------------------------------

void ClientServerTest::clientToServerTimeout()
{
    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->isStarted());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->isConnected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->numClients() == 1);

    CPPUNIT_ASSERT_MESSAGE("Timeout", s_server->getReceiveTimeout().has_value());
    CPPUNIT_ASSERT_MESSAGE("Timeout value", s_server->getReceiveTimeout().value() == 30);

    // Reach the timeout
    std::vector< ::igtl::MessageHeader::Pointer> headers;
    CPPUNIT_ASSERT_THROW(headers = s_server->receiveHeaders(), sight::io::igtl::Exception);
}

//------------------------------------------------------------------------------

void ClientServerTest::serverToClient()
{
    ::igtl::StringMessage::Pointer stringMsg = ::igtl::StringMessage::New();
    stringMsg->SetString("Hello from server!");

    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->isStarted());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->isConnected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->numClients() == 1);

    s_server->broadcast(static_cast< ::igtl::MessageBase::Pointer>(stringMsg));

    ::igtl::MessageHeader::Pointer header;
    CPPUNIT_ASSERT_NO_THROW(header = s_client->receiveHeader());

    CPPUNIT_ASSERT_MESSAGE("Received header", header);

    CPPUNIT_ASSERT_MESSAGE("Device Name", std::string(header->GetDeviceName()) == "Sight_Tests_Server");
    CPPUNIT_ASSERT_MESSAGE("Device Type", std::string(header->GetDeviceType()) == stringMsg->GetDeviceType());

    ::igtl::MessageBase::Pointer msg;
    CPPUNIT_ASSERT_NO_THROW(msg = s_client->receiveBody(header));

    CPPUNIT_ASSERT_MESSAGE("Body message", msg);

    ::igtl::StringMessage::Pointer receivedStringMsg =
        ::igtl::StringMessage::Pointer(
            dynamic_cast<::igtl::StringMessage*>(msg.
                                                 GetPointer())
        );

    CPPUNIT_ASSERT_MESSAGE("Received IGTL Message", receivedStringMsg);
    CPPUNIT_ASSERT_MESSAGE(
        "Value of message",
        std::string(receivedStringMsg->GetString())
        == std::string(stringMsg->GetString())
    );
}

//------------------------------------------------------------------------------

void ClientServerTest::serverHeaderExceptionTest()
{
    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->isStarted());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->isConnected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->numClients() == 1);

    s_server->getSocket()->SetTimeout(200);
    s_client->getSocket()->SetTimeout(200);

    // disconnect client.
    s_client->disconnect();

    CPPUNIT_ASSERT_THROW(s_server->receiveHeaders(), sight::io::igtl::Exception);
}

//------------------------------------------------------------------------------

void ClientServerTest::clientHeaderExceptionTest()
{
    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->isStarted());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->isConnected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->numClients() == 1);

    s_server->getSocket()->SetTimeout(200);
    s_client->getSocket()->SetTimeout(200);

    // stop server.
    s_server->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    s_serverFuture.wait();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Client still connected.
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->isConnected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->numClients() == 0);

    // Timeout exception.
    CPPUNIT_ASSERT_THROW(s_client->receiveHeader(), sight::io::igtl::Exception);
}

//------------------------------------------------------------------------------

void ClientServerTest::clientBodyExceptionTest()
{
    ::igtl::StringMessage::Pointer stringMsg = ::igtl::StringMessage::New();
    stringMsg->SetString("Hello from server!");

    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->isStarted());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->isConnected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->numClients() == 1);

    s_server->getSocket()->SetTimeout(10);
    s_client->getSocket()->SetTimeout(10);

    s_server->broadcast(static_cast< ::igtl::MessageBase::Pointer>(stringMsg));

    ::igtl::MessageHeader::Pointer header;
    CPPUNIT_ASSERT_NO_THROW(header = s_client->receiveHeader());

    CPPUNIT_ASSERT_MESSAGE("Received header", header);

    CPPUNIT_ASSERT_MESSAGE("Device Name", std::string(header->GetDeviceName()) == "Sight_Tests_Server");
    CPPUNIT_ASSERT_MESSAGE("Device Type", std::string(header->GetDeviceType()) == stringMsg->GetDeviceType());

    header = ::igtl::MessageHeader::New();
    // stop server.
    s_server->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    s_serverFuture.wait();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::igtl::MessageBase::Pointer msg;
    CPPUNIT_ASSERT_THROW(msg = s_client->receiveBody(nullptr), sight::io::igtl::Exception);
}

//------------------------------------------------------------------------------

void ClientServerTest::serverBodyExceptionTest()
{
    ::igtl::StringMessage::Pointer stringMsg = ::igtl::StringMessage::New();
    stringMsg->SetString("Hello world!");

    s_client->sendMsg(static_cast< ::igtl::MessageBase::Pointer>(stringMsg));

    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->isStarted());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->isConnected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->numClients() == 1);

    s_server->getSocket()->SetTimeout(10);
    s_client->getSocket()->SetTimeout(10);

    std::vector< ::igtl::MessageHeader::Pointer> headers;
    CPPUNIT_ASSERT_NO_THROW(headers = s_server->receiveHeaders());

    // disconnect client.
    s_client->disconnect();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    for(const auto& header : headers)
    {
        CPPUNIT_ASSERT_MESSAGE("Device Name", std::string(header->GetDeviceName()) == "Sight_Tests_Client");
        CPPUNIT_ASSERT_MESSAGE("Device Type", std::string(header->GetDeviceType()) == stringMsg->GetDeviceType());

        ::igtl::MessageBase::Pointer msg;
        CPPUNIT_ASSERT_THROW(msg = s_server->receiveBody(nullptr, 0), sight::io::igtl::Exception);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::igtl::ut
