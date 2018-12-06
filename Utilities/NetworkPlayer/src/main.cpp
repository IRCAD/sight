/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "NetworkPlayer/OpenIGTLinkPlayer.hpp"
#include "NetworkPlayer/ZeroMQPlayer.hpp"

#include <fwCore/spyLog.hpp>
#include <fwCore/util/FactoryRegistry.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <igtlNetwork/Server.hpp>

#include <boost/lexical_cast.hpp>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace networkPlayer;

//------------------------------------------------------------------------------

static ::fwData::TransformationMatrix3D::sptr     toMatrix (std::vector<double> const& data)
{
    unsigned int row    = 0;
    unsigned int column = 0;
    ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::New();

    for (std::size_t i = 1; i < data.size(); ++i)
    {
        if (column % 4 == 0 && column != 0)
        {
            ++row;
            column = 0;
        }
        matrix->setCoefficient(row, column, data[i]);
        ++column;
    }
    return matrix;
}

//------------------------------------------------------------------------------

static double   toDouble (std::string const& str)
{
    return ::boost::lexical_cast<double> (str);
}

//------------------------------------------------------------------------------

template<typename Socket>
static void     readStream (std::istream& stream, Socket& socket)
{
    std::istringstream lineStream;
    std::uint32_t diffTime;
    double prevTime = -1;
    std::string line;
    std::vector<std::string>    words;
    std::vector<double>         datas;

    while (!stream.eof())
    {
        std::getline(stream, line);
        lineStream.str(line);

        // iterate all word and push it in words vector
        std::copy(std::istream_iterator<std::string> (lineStream),
                  std::istream_iterator<std::string>(),
                  std::back_inserter<std::vector<std::string> > (words));

        // test if words size is time + 16 value for 4x4 matrix
        if (words.size() == 17)
        {
            // resize datas by words size to prepare transform
            datas.resize(words.size());

            // transform a all word in words in double and push it in datas
            std::transform(words.begin(), words.end(), datas.begin(), &toDouble);
            if (prevTime == -1)
            {
                prevTime = datas[0];
            }
            else
            {
                diffTime = static_cast< std::uint32_t> (datas[0] - prevTime);
                prevTime = datas[0];
                std::this_thread::sleep_for(std::chrono::milliseconds(diffTime));

            }
            socket.sendObject(toMatrix(datas));
        }
        lineStream.clear();
        datas.clear();
        words.clear();
    }
}

//------------------------------------------------------------------------------

int     main (int ac, char** av)
{
    typedef ::fwCore::util::FactoryRegistry<INetworkPlayer::sptr()>       FactoryRegistryType;

    FactoryRegistryType factoryRegistry;
    std::string file;
    std::string socketType;
    std::ifstream fileStream;
    INetworkPlayer::sptr player;

    factoryRegistry.addFactory("igtl", std::bind(&OpenIGTLinkPlayer::create,
                                                 &readStream<OpenIGTLinkPlayer::SocketType>));
    factoryRegistry.addFactory("zmq", std::bind(&ZeroMQPlayer::create,
                                                &readStream<ZeroMQPlayer::SocketType>));
    if (ac > 3)
    {
        socketType = std::string(av[1]);
        file       = std::string(av[2]);
        fileStream.open(file.c_str(), std::ios::in);

        if (fileStream.is_open())
        {
            try
            {
                player = factoryRegistry.create(socketType);
                if (!player)
                {
                    fileStream.close();
                    throw std::runtime_error("Cannot create player : unknown socket type");
                }
                player->configure(ac - 3, &av[3]);
                player->play(fileStream);
                fileStream.close();
            }
            catch (std::exception const& err)
            {
                SLM_ERROR(err.what());
            }
        }
        else
        {
            SLM_ERROR("Cannot open file : " + file);
            return 1;
        }
    }
    else
    {
        SLM_ERROR("./NetworkPlayer <type> <logfile> <zmq_bind_str|port>");
    }
    return 0;
}

