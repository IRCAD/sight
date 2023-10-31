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

#include "client_server_test.hpp"

#include <io/igtl/client.hpp>
#include <io/igtl/detail/message_factory.hpp>
#include <io/igtl/exception.hpp>
#include <io/igtl/server.hpp>

#include <igtlStringMessage.h>

#include <thread>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::igtl::ut::client_server_test);

namespace sight::io::igtl::ut
{

// Global variable for client.
static sight::io::igtl::client::sptr s_client;
// Global variable for server.
static sight::io::igtl::server::sptr s_server;
// Global variable that stores the run_server future.
static std::future<void> s_server_future;

//------------------------------------------------------------------------------

void client_server_test::setUp()
{
    // Create server
    s_server = std::make_shared<sight::io::igtl::server>();
    // Using 0 as port number will check for the first available port number.
    s_server->start(0);
    s_server->add_authorized_device("Sight_Tests_Client");
    s_server->set_receive_timeout(30);

    // Run server (waitForConnection on a thread).
    s_server_future = std::async(std::launch::async, [object_ptr = s_server](auto&& ...){object_ptr->run_server();});

    // Wait for server to be initialized properly.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Create client.
    s_client = std::make_shared<sight::io::igtl::client>();

    // Connect client.
    s_client->connect("127.0.0.1", s_server->get_port());
    s_client->set_device_name_out("Sight_Tests_Client");
    s_client->add_authorized_device("Sight_Tests_Server");

    // Hang until we have a client or we reach the timeout.
    std::uint32_t timeout = 0;
    while(s_server->num_clients() == 0 || timeout >= 1000)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        timeout += 100;
    }

    // Set Name to connected client.
    s_server->set_message_device_name("Sight_Tests_Server");
}

//------------------------------------------------------------------------------

void client_server_test::tearDown()
{
    if(s_client->is_connected())
    {
        s_client->disconnect();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if(s_server->started())
    {
        s_server->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        s_server_future.wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

//------------------------------------------------------------------------------

void client_server_test::client_to_server()
{
    ::igtl::StringMessage::Pointer string_msg = ::igtl::StringMessage::New();
    string_msg->SetString("Hello world!");

    s_client->send_msg(static_cast< ::igtl::MessageBase::Pointer>(string_msg));

    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->started());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->is_connected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->num_clients() == 1);

    std::vector< ::igtl::MessageHeader::Pointer> headers;
    CPPUNIT_ASSERT_NO_THROW(headers = s_server->receive_headers());

    for(const auto& header : headers)
    {
        CPPUNIT_ASSERT_MESSAGE("Device Name", std::string(header->GetDeviceName()) == "Sight_Tests_Client");
        CPPUNIT_ASSERT_MESSAGE("Device Type", std::string(header->GetDeviceType()) == string_msg->GetDeviceType());

        ::igtl::MessageBase::Pointer msg;
        CPPUNIT_ASSERT_NO_THROW(msg = s_server->receive_body(header, 0));

        CPPUNIT_ASSERT_MESSAGE("Body message", msg);

        ::igtl::StringMessage::Pointer received_string_msg =
            ::igtl::StringMessage::Pointer(
                dynamic_cast<::igtl::StringMessage*>(msg.
                                                     GetPointer())
            );

        CPPUNIT_ASSERT_MESSAGE("Received IGTL Message", received_string_msg);
        CPPUNIT_ASSERT_MESSAGE(
            "Value of message",
            std::string(received_string_msg->GetString())
            == std::string(string_msg->GetString())
        );
    }
}

//------------------------------------------------------------------------------

void client_server_test::client_to_server_timeout()
{
    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->started());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->is_connected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->num_clients() == 1);

    CPPUNIT_ASSERT_MESSAGE("Timeout", s_server->get_receive_timeout().has_value());
    CPPUNIT_ASSERT_MESSAGE("Timeout value", s_server->get_receive_timeout().value() == 30);

    // Reach the timeout
    std::vector< ::igtl::MessageHeader::Pointer> headers;
    CPPUNIT_ASSERT_THROW(headers = s_server->receive_headers(), sight::io::igtl::exception);
}

//------------------------------------------------------------------------------

void client_server_test::server_to_client()
{
    ::igtl::StringMessage::Pointer string_msg = ::igtl::StringMessage::New();
    string_msg->SetString("Hello from server!");

    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->started());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->is_connected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->num_clients() == 1);

    s_server->broadcast(static_cast< ::igtl::MessageBase::Pointer>(string_msg));

    ::igtl::MessageHeader::Pointer header;
    CPPUNIT_ASSERT_NO_THROW(header = s_client->receive_header());

    CPPUNIT_ASSERT_MESSAGE("Received header", header);

    CPPUNIT_ASSERT_MESSAGE("Device Name", std::string(header->GetDeviceName()) == "Sight_Tests_Server");
    CPPUNIT_ASSERT_MESSAGE("Device Type", std::string(header->GetDeviceType()) == string_msg->GetDeviceType());

    ::igtl::MessageBase::Pointer msg;
    CPPUNIT_ASSERT_NO_THROW(msg = s_client->receive_body(header));

    CPPUNIT_ASSERT_MESSAGE("Body message", msg);

    ::igtl::StringMessage::Pointer received_string_msg =
        ::igtl::StringMessage::Pointer(
            dynamic_cast<::igtl::StringMessage*>(msg.
                                                 GetPointer())
        );

    CPPUNIT_ASSERT_MESSAGE("Received IGTL Message", received_string_msg);
    CPPUNIT_ASSERT_MESSAGE(
        "Value of message",
        std::string(received_string_msg->GetString())
        == std::string(string_msg->GetString())
    );
}

//------------------------------------------------------------------------------

void client_server_test::server_header_exception_test()
{
    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->started());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->is_connected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->num_clients() == 1);

    s_server->get_socket()->SetTimeout(200);
    s_client->get_socket()->SetTimeout(200);

    // disconnect client.
    s_client->disconnect();

    CPPUNIT_ASSERT_THROW(s_server->receive_headers(), sight::io::igtl::exception);
}

//------------------------------------------------------------------------------

void client_server_test::client_header_exception_test()
{
    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->started());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->is_connected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->num_clients() == 1);

    s_server->get_socket()->SetTimeout(200);
    s_client->get_socket()->SetTimeout(200);

    // stop server.
    s_server->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    s_server_future.wait();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Client still connected.
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->is_connected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->num_clients() == 0);

    // Timeout exception.
    CPPUNIT_ASSERT_THROW(s_client->receive_header(), sight::io::igtl::exception);
}

//------------------------------------------------------------------------------

void client_server_test::client_body_exception_test()
{
    ::igtl::StringMessage::Pointer string_msg = ::igtl::StringMessage::New();
    string_msg->SetString("Hello from server!");

    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->started());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->is_connected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->num_clients() == 1);

    s_server->get_socket()->SetTimeout(10);
    s_client->get_socket()->SetTimeout(10);

    s_server->broadcast(static_cast< ::igtl::MessageBase::Pointer>(string_msg));

    ::igtl::MessageHeader::Pointer header;
    CPPUNIT_ASSERT_NO_THROW(header = s_client->receive_header());

    CPPUNIT_ASSERT_MESSAGE("Received header", header);

    CPPUNIT_ASSERT_MESSAGE("Device Name", std::string(header->GetDeviceName()) == "Sight_Tests_Server");
    CPPUNIT_ASSERT_MESSAGE("Device Type", std::string(header->GetDeviceType()) == string_msg->GetDeviceType());

    header = ::igtl::MessageHeader::New();
    // stop server.
    s_server->stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    s_server_future.wait();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ::igtl::MessageBase::Pointer msg;
    CPPUNIT_ASSERT_THROW(msg = s_client->receive_body(nullptr), sight::io::igtl::exception);
}

//------------------------------------------------------------------------------

void client_server_test::server_body_exception_test()
{
    ::igtl::StringMessage::Pointer string_msg = ::igtl::StringMessage::New();
    string_msg->SetString("Hello world!");

    s_client->send_msg(static_cast< ::igtl::MessageBase::Pointer>(string_msg));

    CPPUNIT_ASSERT_MESSAGE("Server is started", s_server->started());
    CPPUNIT_ASSERT_MESSAGE("Client connected", s_client->is_connected());
    CPPUNIT_ASSERT_MESSAGE("Number of connected client", s_server->num_clients() == 1);

    s_server->get_socket()->SetTimeout(10);
    s_client->get_socket()->SetTimeout(10);

    std::vector< ::igtl::MessageHeader::Pointer> headers;
    CPPUNIT_ASSERT_NO_THROW(headers = s_server->receive_headers());

    // disconnect client.
    s_client->disconnect();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    for(const auto& header : headers)
    {
        CPPUNIT_ASSERT_MESSAGE("Device Name", std::string(header->GetDeviceName()) == "Sight_Tests_Client");
        CPPUNIT_ASSERT_MESSAGE("Device Type", std::string(header->GetDeviceType()) == string_msg->GetDeviceType());

        ::igtl::MessageBase::Pointer msg;
        CPPUNIT_ASSERT_THROW(msg = s_server->receive_body(nullptr, 0), sight::io::igtl::exception);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::igtl::ut
