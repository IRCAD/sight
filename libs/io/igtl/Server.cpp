/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "Server.hpp"

#include "io/igtl/Exception.hpp"

#include <core/spy_log.hpp>

#include <io/igtl/detail/DataConverter.hpp>
#include <io/igtl/detail/MessageFactory.hpp>

#include <boost/lexical_cast.hpp>

namespace sight::io::igtl
{

//------------------------------------------------------------------------------

server::server() :
    m_server_socket(::igtl::ServerSocket::New())
{
    m_socket = m_server_socket;
}

//------------------------------------------------------------------------------

server::~server()
{
    if(m_is_started)
    {
        this->stop();
    }
}

//------------------------------------------------------------------------------

bool server::started() const
{
    core::mt::scoped_lock lock(m_mutex);

    return m_is_started;
}

//------------------------------------------------------------------------------

std::uint16_t server::get_port() const
{
    return m_port;
}

//------------------------------------------------------------------------------

void server::run_server()
{
    client::sptr new_client;

    while(this->started())
    {
        new_client = this->wait_for_connection();
        if(new_client != nullptr)
        {
            core::mt::scoped_lock lock(m_mutex);
            if(m_receive_timeout.has_value())
            {
                new_client->get_socket()->SetReceiveTimeout(static_cast<int>(m_receive_timeout.value()));
            }

            m_clients.push_back(new_client);
        }
    }
}

//------------------------------------------------------------------------------

void server::broadcast(const data::object::csptr& _obj)
{
    std::vector<client::sptr>::iterator it;

    for(it = m_clients.begin() ; it != m_clients.end() ; )
    {
        if(!(*it)->send_object(_obj))
        {
            core::mt::scoped_lock lock(m_mutex);
            (*it)->disconnect();
            it = m_clients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//------------------------------------------------------------------------------

void server::broadcast(::igtl::MessageBase::Pointer _msg)
{
    std::vector<client::sptr>::iterator it;

    for(it = m_clients.begin() ; it != m_clients.end() ; )
    {
        if(!(*it)->send_msg(_msg))
        {
            core::mt::scoped_lock lock(m_mutex);
            (*it)->disconnect();
            it = m_clients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//------------------------------------------------------------------------------

void server::start(std::uint16_t _port)
{
    core::mt::scoped_lock lock(m_mutex);

    if(m_is_started)
    {
        throw exception("Server already started");
    }

    //NOTE: Use the patched version of createServer
    const int result = this->create_server(_port);

    // Uncomment this when patch version isn't needed anymore.
    // m_serverSocket->CreateServer(port);

    // Ask m_serverSocket to give us the real port number (ex: if port is 0, it will use the first available port).
    m_port = std::uint16_t(m_server_socket->GetServerPort());

    if(result != server::SUCCESS)
    {
        throw exception("Cannot create server on port : " + std::to_string(_port));
    }

    m_is_started = true;
}

//------------------------------------------------------------------------------

client::sptr server::wait_for_connection(int _msec)
{
    ::igtl::ClientSocket::Pointer client_socket;
    client::sptr client;

    client_socket = m_server_socket->WaitForConnection(static_cast<std::uint64_t>(_msec));
    if(client_socket.IsNotNull())
    {
        client = std::make_shared<igtl::client>(client_socket);
    }

    return client;
}

//------------------------------------------------------------------------------

void server::stop()
{
    core::mt::scoped_lock lock(m_mutex);
    if(!m_is_started)
    {
        throw io::igtl::exception("Server is already stopped");
    }

    m_is_started = false;
    // Disconnect all clients

    for(auto& client : m_clients)
    {
        client->disconnect();
    }

    m_clients.clear();

    // HACK: patched version of closeSocket
    sight::io::igtl::network::close_socket(m_server_socket->m_SocketDescriptor);
    m_server_socket->m_SocketDescriptor = -1;

    sight::io::igtl::network::close_socket(m_socket->m_SocketDescriptor);
    m_socket->m_SocketDescriptor = -1;

    // Uncomment this when patch isn't needed anymore.
    //m_socket->CloseSocket();
}

//------------------------------------------------------------------------------

std::size_t server::num_clients() const
{
    if(this->started())
    {
        core::mt::scoped_lock lock(m_mutex);

        return m_clients.size();
    }

    return 0;
}

//------------------------------------------------------------------------------

std::vector< ::igtl::MessageHeader::Pointer> server::receive_headers()
{
    std::vector< ::igtl::MessageHeader::Pointer> header_msgs;

    core::mt::scoped_lock lock(m_mutex);
    for(size_t i = 0 ; i < m_clients.size() ; ++i)
    {
        const auto client = m_clients[i];
        //if client is disconnected
        if(client == nullptr)
        {
            m_clients.erase(m_clients.begin() + std::ptrdiff_t(i));
            continue;
        }

        ::igtl::MessageHeader::Pointer header_msg = ::igtl::MessageHeader::New();
        header_msg->InitPack();

        ::igtl::Socket::Pointer socket = client->get_socket();

        if(socket->GetConnected() != 0)
        {
            const int size_receive = socket->Receive(header_msg->GetPackPointer(), header_msg->GetPackSize());

            if(size_receive == -1)
            {
                throw sight::io::igtl::exception("Network timeout");
            }

            if(size_receive == 0)
            {
                // Disconnect this client.
                sight::io::igtl::server::remove_client(client);
                m_clients.erase(m_clients.begin() + std::ptrdiff_t(i));
                throw sight::io::igtl::exception("Network error");
            }

            if(size_receive != header_msg->GetPackSize())
            {
                throw sight::io::igtl::exception("Mismatch in received message size");
            }

            if(header_msg->Unpack() == ::igtl::MessageBase::UNPACK_HEADER)
            {
                const std::string device_name = header_msg->GetDeviceName();

                if(m_filtering_by_device_name)
                {
                    if(m_device_names_in.find(device_name) != m_device_names_in.end())
                    {
                        header_msgs.push_back(header_msg);
                    }
                    else
                    {
                        header_msgs.emplace_back();
                    }
                }
                else
                {
                    header_msgs.push_back(header_msg);
                }
            }
        }
    }

    return header_msgs;
}

//------------------------------------------------------------------------------

::igtl::MessageBase::Pointer server::receive_body(
    ::igtl::MessageHeader::Pointer const _header_msg,
    unsigned int _client
)
{
    ::igtl::MessageBase::Pointer msg;

    if(_header_msg == nullptr)
    {
        throw sight::io::igtl::exception("Invalid header message");
    }

    msg = io::igtl::detail::message_factory::create(_header_msg->GetDeviceType());
    msg->SetMessageHeader(_header_msg);
    msg->AllocatePack();

    core::mt::scoped_lock lock(m_mutex);

    const int result = (m_clients[_client]->get_socket())->Receive(msg->GetPackBodyPointer(), msg->GetPackBodySize());

    if(result == -1) // Timeout
    {
        throw sight::io::igtl::exception("Network timeout");
    }

    if(result == 0) // Error
    {
        // Disconnect this client.
        sight::io::igtl::server::remove_client(m_clients[_client]);
        m_clients.erase(m_clients.begin() + _client);
        throw sight::io::igtl::exception("Network error");
    }

    const auto unpack_result = msg->Unpack();

    if(unpack_result == ::igtl::MessageHeader::UNPACK_UNDEF)
    {
        throw sight::io::igtl::exception("Network error");
    }

    if(unpack_result == ::igtl::MessageHeader::UNPACK_BODY)
    {
        return msg;
    }

    throw exception("Body pack is not valid");
}

//------------------------------------------------------------------------------

std::vector<data::object::sptr> server::receive_objects(std::vector<std::string>& _device_names)
{
    std::vector<data::object::sptr> obj_vect;
    std::vector< ::igtl::MessageHeader::Pointer> header_msg_vect = this->receive_headers();
    unsigned int client                                          = 0;
    for(const auto& header_msg : header_msg_vect)
    {
        if(header_msg.IsNotNull())
        {
            ::igtl::MessageBase::Pointer msg = this->receive_body(header_msg, std::uint32_t(client));
            if(msg.IsNotNull())
            {
                detail::data_converter::sptr converter = detail::data_converter::get_instance();
                obj_vect.push_back(converter->from_igtl_message(msg));
                _device_names.emplace_back(header_msg->GetDeviceName());
            }
        }

        ++client;
    }

    return obj_vect;
}

//------------------------------------------------------------------------------

void server::set_message_device_name(const std::string& _device_name)
{
    for(const auto& client : m_clients)
    {
        if(client)
        {
            client->set_device_name_out(_device_name);
        }
    }
}

//------------------------------------------------------------------------------

int server::create_server(std::uint16_t _port)
{
    if(m_server_socket->m_SocketDescriptor != -1)
    {
        SIGHT_WARN("Server Socket already exists. Closing old socket.");
        sight::io::igtl::network::close_socket(m_server_socket->m_SocketDescriptor);
        m_server_socket->m_SocketDescriptor = -1;
    }

    m_server_socket->m_SocketDescriptor = sight::io::igtl::network::create_socket();

    if(m_server_socket->m_SocketDescriptor < 0)
    {
        SIGHT_ERROR("Cannot create socket");
        return -1;
    }

    if(sight::io::igtl::network::bind_socket(m_server_socket->m_SocketDescriptor, _port) != 0
       || sight::io::igtl::network::listen_socket(m_server_socket->m_SocketDescriptor) != 0)
    {
        // failed to bind or listen.
        sight::io::igtl::network::close_socket(m_server_socket->m_SocketDescriptor);
        m_server_socket->m_SocketDescriptor = -1;
        return -1;
    }

    // Success.
    return 0;
}

//------------------------------------------------------------------------------

void server::remove_client(client::sptr _client)
{
    if(_client)
    {
        _client->disconnect();
        _client.reset();
    }
}

} //namespace sight::io::igtl
