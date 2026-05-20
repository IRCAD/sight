/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <io/igtl/client.hpp>
#include <io/igtl/detail/message_factory.hpp>
#include <io/igtl/exception.hpp>
#include <io/igtl/server.hpp>

#include <doctest/doctest.h>

#include <igtlStringMessage.h>

#include <thread>

namespace
{

struct fixture
{
    fixture()
    {
        // Setup
        s_server = std::make_shared<sight::io::igtl::server>();
        s_server->start(0);
        s_server->add_authorized_device("Sight_Tests_Client");
        s_server->set_receive_timeout(30);
        s_server_future = std::async(
            std::launch::async,
            [object_ptr = s_server](auto&& ...)
            {
                object_ptr->run_server();
            });
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        s_client = std::make_shared<sight::io::igtl::client>();
        s_client->connect("127.0.0.1", s_server->get_port());
        s_client->set_device_name_out("Sight_Tests_Client");
        s_client->add_authorized_device("Sight_Tests_Server");
        std::uint32_t timeout = 0;
        while(s_server->num_clients() == 0 || timeout >= 1000)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            timeout += 100;
        }

        s_server->set_message_device_name("Sight_Tests_Server");
    }

    ~fixture()
    {
        // Teardown
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

    sight::io::igtl::client::sptr s_client;
    sight::io::igtl::server::sptr s_server;
    std::future<void> s_server_future;
};

} // namespace

namespace sight::io::igtl
{

TEST_SUITE("sight::io::igtl")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "client_to_server")
    {
        ::igtl::StringMessage::Pointer string_msg = ::igtl::StringMessage::New();
        string_msg->SetString("Hello world!");

        s_client->send_msg(static_cast< ::igtl::MessageBase::Pointer>(string_msg));

        CHECK_MESSAGE(s_server->started(), "Server is started");
        CHECK_MESSAGE(s_client->is_connected(), "Client connected");
        CHECK_MESSAGE(s_server->num_clients() == 1, "Number of connected client");

        std::vector< ::igtl::MessageHeader::Pointer> headers;
        CHECK_NOTHROW(headers = s_server->receive_headers());

        for(const auto& header : headers)
        {
            CHECK_MESSAGE(std::string(header->GetDeviceName()) == "Sight_Tests_Client", "Device Name");
            CHECK_MESSAGE(std::string(header->GetDeviceType()) == string_msg->GetDeviceType(), "Device Type");

            ::igtl::MessageBase::Pointer msg;
            CHECK_NOTHROW(msg = s_server->receive_body(header, 0));

            CHECK_MESSAGE(msg, "Body message");

            ::igtl::StringMessage::Pointer received_string_msg =
                ::igtl::StringMessage::Pointer(
                    dynamic_cast<::igtl::StringMessage*>(msg.GetPointer())
                );

            CHECK_MESSAGE(received_string_msg, "Received IGTL Message");
            CHECK_MESSAGE(
                std::string(received_string_msg->GetString())
                == std::string(string_msg->GetString()),
                "Value of message"
            );
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "client_to_server_timeout")
    {
        CHECK_MESSAGE(s_server->started(), "Server is started");
        CHECK_MESSAGE(s_client->is_connected(), "Client connected");
        CHECK_MESSAGE(s_server->num_clients() == 1, "Number of connected client");

        CHECK_MESSAGE(s_server->get_receive_timeout().has_value(), "Timeout");
        CHECK_MESSAGE(s_server->get_receive_timeout().value() == 30, "Timeout value");

        // Reach the timeout
        std::vector< ::igtl::MessageHeader::Pointer> headers;
        CHECK_THROWS_AS(headers = s_server->receive_headers(), sight::io::igtl::exception);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "server_to_client")
    {
        ::igtl::StringMessage::Pointer string_msg = ::igtl::StringMessage::New();
        string_msg->SetString("Hello from server!");

        CHECK_MESSAGE(s_server->started(), "Server is started");
        CHECK_MESSAGE(s_client->is_connected(), "Client connected");
        CHECK_MESSAGE(s_server->num_clients() == 1, "Number of connected client");

        s_server->broadcast(static_cast< ::igtl::MessageBase::Pointer>(string_msg));

        ::igtl::MessageHeader::Pointer header;
        CHECK_NOTHROW(header = s_client->receive_header());

        CHECK_MESSAGE(header, "Received header");

        CHECK_MESSAGE(std::string(header->GetDeviceName()) == "Sight_Tests_Server", "Device Name");
        CHECK_MESSAGE(std::string(header->GetDeviceType()) == string_msg->GetDeviceType(), "Device Type");

        ::igtl::MessageBase::Pointer msg;
        CHECK_NOTHROW(msg = s_client->receive_body(header));

        CHECK_MESSAGE(msg, "Body message");

        ::igtl::StringMessage::Pointer received_string_msg =
            ::igtl::StringMessage::Pointer(
                dynamic_cast<::igtl::StringMessage*>(msg.GetPointer())
            );

        CHECK_MESSAGE(received_string_msg, "Received IGTL Message");
        CHECK_MESSAGE(
            std::string(received_string_msg->GetString())
            == std::string(string_msg->GetString()),
            "Value of message"
        );
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "server_header_exception")
    {
        CHECK_MESSAGE(s_server->started(), "Server is started");
        CHECK_MESSAGE(s_client->is_connected(), "Client connected");
        CHECK_MESSAGE(s_server->num_clients() == 1, "Number of connected client");

        s_server->get_socket()->SetTimeout(200);
        s_client->get_socket()->SetTimeout(200);

        // disconnect client.
        s_client->disconnect();

        CHECK_THROWS_AS(s_server->receive_headers(), sight::io::igtl::exception);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "client_header_exception")
    {
        CHECK_MESSAGE(s_server->started(), "Server is started");
        CHECK_MESSAGE(s_client->is_connected(), "Client connected");
        CHECK_MESSAGE(s_server->num_clients() == 1, "Number of connected client");

        s_server->get_socket()->SetTimeout(200);
        s_client->get_socket()->SetTimeout(200);

        // stop server.
        s_server->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        s_server_future.wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Client still connected.
        CHECK_MESSAGE(s_client->is_connected(), "Client connected");
        CHECK_MESSAGE(s_server->num_clients() == 0, "Number of connected client");

        // Timeout exception.
        CHECK_THROWS_AS(s_client->receive_header(), sight::io::igtl::exception);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "client_body_exception")
    {
        ::igtl::StringMessage::Pointer string_msg = ::igtl::StringMessage::New();
        string_msg->SetString("Hello from server!");

        CHECK_MESSAGE(s_server->started(), "Server is started");
        CHECK_MESSAGE(s_client->is_connected(), "Client connected");
        CHECK_MESSAGE(s_server->num_clients() == 1, "Number of connected client");

        s_server->get_socket()->SetTimeout(10);
        s_client->get_socket()->SetTimeout(10);

        s_server->broadcast(static_cast< ::igtl::MessageBase::Pointer>(string_msg));

        ::igtl::MessageHeader::Pointer header;
        CHECK_NOTHROW(header = s_client->receive_header());

        CHECK_MESSAGE(header, "Received header");

        CHECK_MESSAGE(std::string(header->GetDeviceName()) == "Sight_Tests_Server", "Device Name");
        CHECK_MESSAGE(std::string(header->GetDeviceType()) == string_msg->GetDeviceType(), "Device Type");

        header = ::igtl::MessageHeader::New();
        // stop server.
        s_server->stop();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        s_server_future.wait();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        ::igtl::MessageBase::Pointer msg;
        CHECK_THROWS_AS(msg = s_client->receive_body(nullptr), sight::io::igtl::exception);
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(fixture, "server_body_exception")
    {
        ::igtl::StringMessage::Pointer string_msg = ::igtl::StringMessage::New();
        string_msg->SetString("Hello world!");

        s_client->send_msg(static_cast< ::igtl::MessageBase::Pointer>(string_msg));

        CHECK_MESSAGE(s_server->started(), "Server is started");
        CHECK_MESSAGE(s_client->is_connected(), "Client connected");
        CHECK_MESSAGE(s_server->num_clients() == 1, "Number of connected client");

        s_server->get_socket()->SetTimeout(10);
        s_client->get_socket()->SetTimeout(10);

        std::vector< ::igtl::MessageHeader::Pointer> headers;
        CHECK_NOTHROW(headers = s_server->receive_headers());

        // disconnect client.
        s_client->disconnect();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        for(const auto& header : headers)
        {
            CHECK_MESSAGE(std::string(header->GetDeviceName()) == "Sight_Tests_Client", "Device Name");
            CHECK_MESSAGE(std::string(header->GetDeviceType()) == string_msg->GetDeviceType(), "Device Type");

            ::igtl::MessageBase::Pointer msg;
            CHECK_THROWS_AS(msg = s_server->receive_body(nullptr, 0), sight::io::igtl::exception);
        }
    }

//------------------------------------------------------------------------------
} // TEST_SUITE

} // namespace sight::io::igtl
