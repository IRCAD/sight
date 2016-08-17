/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlMessageBase.h"

#include <fwCore/spyLog.hpp>
#include <fwCore/util/FactoryRegistry.hpp>
#include <fwThread/Worker.hpp>

#include <igtlNetwork/Server.hpp>

#include <boost/date_time.hpp>
#include <boost/type.hpp>

#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <functional>


/**
 * @brief Utility app to redirect IGTL message with specific device name to a specific port
 *
 * In F4S each type of message has it's own port, but it's not the case in other system
 * so if we wanted to communicate with this kind of system we need to redirect messages.
 * Example : A system send a TRANSFORM and a POLYDATA respectively named Matrix1 and Polydata1 on port 4242
 * In Order to receive them in F4S we configure the proxy to redirect Matrix1 on 4243 and PolyData1 on 4244
 * The configuration is handle in a file described below.
 *
 * Configuration file should be written as "Type" "DeviceNameOut" "DeviceNameOut" "port" on each line
 * TRANSFORM Matrix Matrix1 4243
 * POLYDATA PolyData PolyData1 4244
 *
 * An example of file is given in ./config/config-proxy.cfg
 *
 * Usage : NetworkProxy "port" "configuration file"
 *
 */

/**
 * @brief The configuration struct to handle message parameters
 */
struct configuration
{
    igtlNetwork::Server::sptr server;
    std::string deviceIn;
    std::string deviceOut;
    std::string deviceType;
    ::boost::uint16_t port;
    ::fwThread::Worker::sptr worker;

};

/**
 * @brief Read the configuration file and initialize re-sending servers
 * @param path to configFile
 * @return  a map
 */
std::map< std::string, configuration > initialize(std::string configFile)
{
    SLM_INFO("Reading parameters...");

    std::vector< std::string > messageType;
    std::vector< std::string > deviceInTab;
    std::vector< std::string > deviceOutTab;
    std::vector< ::boost::uint16_t > portTab;
    std::map< std::string, configuration > association;


    std::ifstream fileStream;
    fileStream.open(configFile.c_str(), std::ios::in);
    if (fileStream.is_open())
    {
        std::istringstream lineStream;
        std::string line;
        std::vector<std::string>    words;

        unsigned int i = 0;

        while(std::getline (fileStream, line))
        {
            lineStream.str (line);
            // iterate all word and push it in words vector
            std::copy (std::istream_iterator<std::string> (lineStream),
                       std::istream_iterator<std::string>(),
                       std::back_inserter<std::vector<std::string> > (words));

            OSLM_FATAL_IF("Configuration file empty ",words.size() == 0);

            if(words[0].find("#") == std::string::npos)
            {
                OSLM_FATAL_IF("Error in configuration file line: '"<<i<<"'.",words.size() != 4);

                OSLM_INFO(
                    "Type : "<<words[0]<<", device In : "<<words[1]<< ", device Out : "<<words[2]<<", port : "<<
                    words[3]);

                messageType.push_back(words[0]);
                deviceInTab.push_back(words[1]);
                deviceOutTab.push_back(words[2]);
                portTab.push_back(::boost::lexical_cast< ::boost::uint16_t > (words[3]));
            }

            lineStream.clear();
            words.clear();
            ++i;
        }
    }


    for(unsigned int i = 0; i< messageType.size(); i++)
    {
        //check if port num for this config isn't used
        std::map< std::string, configuration >::iterator it;
        igtlNetwork::Server::sptr server = ::igtlNetwork::Server::sptr (new ::igtlNetwork::Server());
        ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
        bool serverAlreadyStarted = false;

        for(it = association.begin(); it != association.end(); ++it)
        {
            //if port is already used we use the same server and worker
            if(it->second.port == portTab[i])
            {
                OSLM_TRACE("Found that "<<it->second.deviceIn<<" and "<<deviceInTab[i]<<
                           " have the same port ("<<it->second.port<<").");
                server               = it->second.server;
                worker               = it->second.worker;
                serverAlreadyStarted = true;
            }
        }

        configuration config;
        config.deviceIn   = deviceInTab[i];
        config.deviceOut  = deviceOutTab[i];
        config.deviceType = messageType[i];
        config.port       = portTab[i];
        config.worker     = worker;


        config.server = server;
        if(!serverAlreadyStarted)
        {
            config.server->start(config.port);

            std::function<void() > task = std::bind (&::igtlNetwork::Server::runServer, config.server);
            config.worker->post(task);
        }

        association[deviceInTab[i]] = config;

    }


    return association;
}
/**
 * @brief main
 * @param argc number of arguments
 * @param argv NetworkProxy port configuration file
 */
int main (int argc, char** argv)
{
    //------------------------------------------------------------
    // Parse Arguments

    if (argc != 3) // check number of arguments
    {
        // If not correct, print usage
        std::cerr << "Usage: " << argv[0] << " <port> <configuration file>" << std::endl;
        exit(0);
    }

    ::boost::uint16_t port = ::boost::lexical_cast< ::boost::uint16_t >(argv[1]);
    std::string configFile(argv[2]);

    //Initialization of parameters
    //std::map< std::string , igtlNetwork::Server::sptr > associationDeviceServer = initialize(configFile);
    std::map< std::string, configuration > associationDeviceServer = initialize(configFile);

    ::igtlNetwork::Server::sptr receiveServer = ::igtlNetwork::Server::sptr (new ::igtlNetwork::Server());
    ::fwThread::Worker::sptr worker           = ::fwThread::Worker::New();
    try
    {
        receiveServer->start(port);
        std::function<void() > task = std::bind (&::igtlNetwork::Server::runServer, receiveServer);
        worker->post(task);

    }
    catch (std::exception const& err)
    {
        SLM_INFO(err.what());
    }

    OSLM_INFO("server started on port: "<<port);


    //main loop
    while (1)
    {
        // Create a message buffer to receive header
        igtl::MessageHeader::Pointer headerMsg;
        igtl::MessageBase::Pointer msg;

        std::vector< ::igtl::MessageBase::Pointer > headerMsgs;
        // Initialize receive buffer
        headerMsgs = receiveServer->receiveHeader();

        for(unsigned int i = 0; i< headerMsgs.size(); ++i)
        {
            headerMsg = headerMsgs[i];


            if(headerMsg.IsNotNull())
            {
                // Deserialize the header
                headerMsg->Unpack();
                std::string deviceName = headerMsg->GetDeviceName();
                std::string deviceType = headerMsg->GetDeviceType();

                ::igtlNetwork::Server::sptr sendingServer;

                if(associationDeviceServer.find(deviceName) != associationDeviceServer.end())
                {
                    configuration config = associationDeviceServer.find(deviceName)->second;
                    OSLM_TRACE("Received a '"<<deviceType<<"' named '"<<deviceName);

                    sendingServer = config.server;

                    if(config.deviceType == deviceType)
                    {
                        if(sendingServer)
                        {
                            msg = receiveServer->receiveBody(headerMsg,i);

                            if(msg.IsNotNull())
                            {
                                OSLM_TRACE("Resending the message '"<<deviceName<<"' with name '"<<config.deviceOut
                                                                    <<" to port : '"<<config.port<<"'.");

                                //re-send the message with the correct server
                                sendingServer->setMessageDeviceName(config.deviceOut);
                                sendingServer->broadcast(msg);
                            }
                        }
                        else
                        {
                            SLM_WARN("This message is not handled ... message skipped");
                        }
                    }
                    else
                    {
                        OSLM_WARN(
                            "No corresponding between type "<<deviceType<<" and name "<<deviceName<<" message skipped");
                    }
                }
                else
                {
                    SLM_WARN("This message is not handled... message skipped.");
                }
            }
        }
        headerMsgs.clear();

    }

    receiveServer->stop();
    worker->stop();

    for(auto s : associationDeviceServer)
    {
        s.second.server->stop();
        s.second.worker->stop();
    }

    return 1;

}
