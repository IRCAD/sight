/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "igtlMessageBase.h"
#include "igtlMessageHeader.h"
#include "igtlOSUtil.h"

#include <core/factory_registry.hpp>
#include <core/spy_log.hpp>
#include <core/thread/worker.hpp>

#include <io/igtl/server.hpp>

#include <boost/date_time.hpp>
#include <boost/type.hpp>

#include <fstream>
#include <functional>
#include <map>
#include <sstream>
#include <vector>

/**
 * @brief Utility app to redirect IGTL message with specific device name to a specific port
 *
 * In Sight each type of message has it's own port, but it's not the case in other system
 * so if we wanted to communicate with this kind of system we need to redirect messages.
 * Example : A system send a TRANSFORM and a POLYDATA respectively named Matrix1 and Polydata1 on port 4242
 * In Order to receive them in Sight we configure the proxy to redirect Matrix1 on 4243 and PolyData1 on 4244
 * The configuration is handle in a file described below.
 *
 * Configuration file should be written as "Type" "DeviceNameOut" "DeviceNameOut" "port" on each line
 * TRANSFORM Matrix Matrix1 4243
 * POLYDATA PolyData PolyData1 4244
 *
 * An example of file is given in ./config/config-proxy.cfg
 *
 * Usage : network_proxy "port" "configuration file"
 *
 */

/**
 * @brief The configuration struct to handle message parameters
 */
struct configuration
{
    sight::io::igtl::server::sptr server;
    std::string device_in;
    std::string device_out;
    std::string device_type;
    std::uint16_t port {};
    sight::core::thread::worker::sptr worker;
};

/**
 * @brief Read the configuration file and initialize re-sending servers
 * @param _config_file  path to configFile
 * @return  a map
 */
std::map<std::string, configuration> initialize(std::string _config_file)
{
    SIGHT_INFO("Reading parameters...");

    std::vector<std::string> message_type;
    std::vector<std::string> device_in_tab;
    std::vector<std::string> device_out_tab;
    std::vector<std::uint16_t> port_tab;
    std::map<std::string, configuration> association;

    std::ifstream file_stream;
    file_stream.open(_config_file.c_str(), std::ios::in);
    if(file_stream.is_open())
    {
        std::istringstream line_stream;
        std::string line;
        std::vector<std::string> words;

        unsigned int i = 0;

        while(std::getline(file_stream, line))
        {
            line_stream.str(line);
            // iterate all word and push it in words vector
            std::copy(
                std::istream_iterator<std::string>(line_stream),
                std::istream_iterator<std::string>(),
                std::back_inserter<std::vector<std::string> >(words)
            );

            SIGHT_FATAL_IF("Configuration file empty ", words.empty());

            if(words[0].find('#') == std::string::npos)
            {
                SIGHT_FATAL_IF("Error in configuration file line: '" << i << "'.", words.size() != 4);

                SIGHT_INFO(
                    "Type : " << words[0] << ", device In : " << words[1] << ", device Out : " << words[2] << ", port : "
                    << words[3]
                );

                message_type.push_back(words[0]);
                device_in_tab.push_back(words[1]);
                device_out_tab.push_back(words[2]);
                port_tab.push_back(boost::lexical_cast<std::uint16_t>(words[3]));
            }

            line_stream.clear();
            words.clear();
            ++i;
        }
    }

    for(unsigned int i = 0 ; i < message_type.size() ; i++)
    {
        //check if port num for this config isn't used
        std::map<std::string, configuration>::iterator it;
        auto server                 = std::make_shared<sight::io::igtl::server>();
        auto worker                 = sight::core::thread::worker::make();
        bool server_already_started = false;

        for(it = association.begin() ; it != association.end() ; ++it)
        {
            //if port is already used we use the same server and worker
            if(it->second.port == port_tab[i])
            {
                SIGHT_INFO(
                    "Found that " << it->second.device_in << " and " << device_in_tab[i]
                    << " have the same port (" << it->second.port << ")."
                );
                server                 = it->second.server;
                worker                 = it->second.worker;
                server_already_started = true;
            }
        }

        configuration config;
        config.device_in   = device_in_tab[i];
        config.device_out  = device_out_tab[i];
        config.device_type = message_type[i];
        config.port        = port_tab[i];
        config.worker      = worker;

        config.server = server;
        if(!server_already_started)
        {
            config.server->start(config.port);

            std::function<void()> task = [object_ptr = config.server](auto&& ...){object_ptr->run_server();};
            config.worker->post(task);
        }

        association[device_in_tab[i]] = config;
    }

    return association;
}

/**
 * @brief main
 * @param argc number of arguments
 * @param argv network_proxy port configuration file
 */
int main(int argc, char** argv)
{
    //------------------------------------------------------------
    // Parse Arguments

    if(argc != 3) // check number of arguments
    {
        // If not correct, print usage
        std::cerr << "Usage: " << argv[0] << " <port> <configuration file>" << std::endl;
        exit(0);
    }

    auto port = boost::lexical_cast<std::uint16_t>(argv[1]);
    std::string config_file(argv[2]);

    //Initialization of parameters
    std::map<std::string, configuration> association_device_server = initialize(config_file);

    auto receive_server = std::make_shared<sight::io::igtl::server>();
    auto worker         = sight::core::thread::worker::make();
    try
    {
        receive_server->start(port);
        std::function<void()> task = [receive_server](auto&& ...){receive_server->run_server();};
        worker->post(task);
    }
    catch(std::exception const& err)
    {
        SIGHT_INFO(err.what());
    }

    SIGHT_INFO("server started on port: " << port);

    //main loop
    while(true)
    {
        // Create a message buffer to receive header
        ::igtl::MessageHeader::Pointer header_msg;
        ::igtl::MessageBase::Pointer msg;

        std::vector< ::igtl::MessageBase::Pointer> header_msgs;
        // Initialize receive buffer
        header_msgs = receive_server->receive_headers();

        for(unsigned int i = 0 ; i < header_msgs.size() ; ++i)
        {
            header_msg = header_msgs[i];

            if(header_msg.IsNotNull())
            {
                // Deserialize the header
                header_msg->Unpack();
                std::string device_name = header_msg->GetDeviceName();
                std::string device_type = header_msg->GetDeviceType();

                sight::io::igtl::server::sptr sending_server;

                if(association_device_server.find(device_name) != association_device_server.end())
                {
                    configuration config = association_device_server.find(device_name)->second;
                    SIGHT_INFO("Received a '" << device_type << "' named '" << device_name);

                    sending_server = config.server;

                    if(config.device_type == device_type)
                    {
                        if(sending_server)
                        {
                            msg = receive_server->receive_body(header_msg, i);

                            if(msg.IsNotNull())
                            {
                                SIGHT_DEBUG(
                                    "Resending the message '" << device_name << "' with name '" << config.device_out
                                    << " to port : '" << config.port << "'."
                                );

                                //re-send the message with the correct server
                                sending_server->set_message_device_name(config.device_out);
                                sending_server->broadcast(msg);
                            }
                        }
                        else
                        {
                            SIGHT_WARN("This message is not handled ... message skipped");
                        }
                    }
                    else
                    {
                        SIGHT_WARN(
                            "No corresponding between type " << device_type << " and name " << device_name << " message skipped"
                        );
                    }
                }
                else
                {
                    SIGHT_WARN("This message is not handled... message skipped.");
                }
            }
        }

        header_msgs.clear();
    }

    receive_server->stop();
    worker->stop();

    for(const auto& s : association_device_server)
    {
        s.second.server->stop();
        s.second.worker->stop();
    }

    return 1;
}
