/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "NetworkLogger/INetworkLogger.hpp"
#include "NetworkLogger/OpenIGTLinkLogger.hpp"
#include "NetworkLogger/ZeroMQLogger.hpp"

#include <fwCore/spyLog.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <boost/date_time.hpp>

#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace networkLogger;

//------------------------------------------------------------------------------

static std::string getCurrentTime()
{
    unsigned long int currentTime;
    ::boost::posix_time::microsec_clock clock;

    currentTime = clock.local_time().time_of_day().total_milliseconds();
    return ::boost::lexical_cast<std::string> (currentTime);
}

//------------------------------------------------------------------------------

static std::string formatTransformMatrix (::fwData::TransformationMatrix3D::sptr const& matrix)
{
    std::stringstream stream;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            stream << matrix->getCoefficient(i, j) << " ";
        }
    }
    return stream.str();
}

//------------------------------------------------------------------------------

template<typename Socket>
static void writeStream (std::ostream& outputStream, Socket& socket)
{
    ::fwData::Object::sptr obj;
    std::string deviceName;
    while (obj = socket.receiveObject(deviceName))
    {
        ::fwData::TransformationMatrix3D::sptr matrixReceived = ::fwData::TransformationMatrix3D::dynamicCast(obj);
        outputStream << getCurrentTime() << " " << formatTransformMatrix(matrixReceived);
        outputStream << std::endl;
    }
}

//------------------------------------------------------------------------------

int main (int ac, char** av)
{
    typedef ::fwCore::util::FactoryRegistry<INetworkLogger::sptr()> FactoryRegistryType;

    FactoryRegistryType factoryRegistry;
    INetworkLogger::sptr logger;
    std::string filename;
    std::ofstream fileStream;
    std::string socketType;

    factoryRegistry.addFactory("igtl", std::bind(&OpenIGTLinkLogger::create,
                                                 &writeStream<OpenIGTLinkLogger::SocketType>));
    factoryRegistry.addFactory("zmq", std::bind(&ZeroMQLogger::create,
                                                &writeStream<ZeroMQLogger::SocketType>));
    if (ac > 3)
    {
        filename = std::string(av[2]);
        try
        {
            fileStream.open(filename.c_str(), std::ios::out | std::ios::app);
            if (fileStream.is_open())
            {
                socketType = std::string(av[1]);
                logger     = factoryRegistry.create(socketType);
                if (!logger)
                {
                    fileStream.close();
                    throw std::runtime_error("Cannot create player : unknown socket type");
                }
                logger->configure(ac - 2, &av[3]);
                logger->log(fileStream);
                fileStream.close();
            }
        }
        catch (std::exception const& err)
        {
            SLM_ERROR(err.what());
        }
    }
    else
    {
        SLM_ERROR("./NetworkLogger <type> <filename> < <ip> <port> | zmq_connect_str>");
    }
}
