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

#include "SClientListener.hpp"

#include <core/com/Signal.hxx>

#include <data/FrameTL.hpp>
#include <data/Image.hpp>
#include <data/Matrix4.hpp>
#include <data/MatrixTL.hpp>
#include <data/Object.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/Preferences.hpp>

#include <functional>
#include <string>

namespace sight::module::io::igtl
{

const service::IService::KeyType s_OBJECTS_GROUP = "objects";

//-----------------------------------------------------------------------------

SClientListener::SClientListener() :
    m_tlInitialized(false)
{
}

//-----------------------------------------------------------------------------

SClientListener::~SClientListener()
{
}

//-----------------------------------------------------------------------------

void SClientListener::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();

    const ConfigType configInOut = config.get_child("inout");

    SIGHT_ASSERT(
        "configured group must be '" + s_OBJECTS_GROUP + "'",
        configInOut.get<std::string>("<xmlattr>.group", "") == s_OBJECTS_GROUP
    );

    const auto keyCfg = configInOut.equal_range("key");
    for(auto itCfg = keyCfg.first ; itCfg != keyCfg.second ; ++itCfg)
    {
        const service::IService::ConfigType& attr = itCfg->second.get_child("<xmlattr>");
        const std::string deviceName              = attr.get("deviceName", "Sight");
        m_deviceNames.push_back(deviceName);
        m_client.addAuthorizedDevice(deviceName);
    }

    m_client.setFilteringByDeviceName(true);

    const std::string serverInfo = config.get("server", "");
    if(!serverInfo.empty())
    {
        const std::string::size_type splitPosition = serverInfo.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_hostnameConfig = serverInfo.substr(0, splitPosition);
        m_portConfig     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw core::tools::Failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void SClientListener::runClient()
{
    // 1. Connection
    try
    {
        ui::base::Preferences preferences;
        const auto port     = preferences.delimited_get<std::uint16_t>(m_portConfig);
        const auto hostname = preferences.delimited_get<std::string>(m_hostnameConfig);

        m_client.connect(hostname, port);
        m_sigConnected->asyncEmit();
    }
    catch(core::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            sight::ui::base::dialog::MessageDialog::show("Connection error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            SIGHT_ERROR(ex.what());
        }

        return;
    }

    // 2. Receive messages
    try
    {
        while(m_client.isConnected())
        {
            std::string deviceName;
            data::Object::sptr receiveObject = m_client.receiveObject(deviceName);
            if(receiveObject)
            {
                const auto& iter = std::find(m_deviceNames.begin(), m_deviceNames.end(), deviceName);

                if(iter != m_deviceNames.end())
                {
                    const auto indexReceiveObject = std::distance(m_deviceNames.begin(), iter);
                    const auto obj                = m_objects[static_cast<std::size_t>(indexReceiveObject)].lock();

                    const bool isATimeline = obj->isA("data::MatrixTL") || obj->isA("data::FrameTL");
                    if(isATimeline)
                    {
                        this->manageTimeline(receiveObject, static_cast<std::size_t>(indexReceiveObject));
                    }
                    else
                    {
                        obj->shallowCopy(receiveObject);

                        data::Object::ModifiedSignalType::sptr sig;
                        sig = obj->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
                        sig->asyncEmit();
                    }
                }
            }
        }
    }
    catch(core::Exception& ex)
    {
        // Only open a dialog if the service is started.
        // ReceiveObject may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->getStatus() == STARTED)
        {
            sight::ui::base::dialog::MessageDialog::show("Error", ex.what());
            this->slot(s_STOP_SLOT)->asyncRun();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            SIGHT_ERROR(ex.what());
        }
    }
}

//-----------------------------------------------------------------------------

void SClientListener::starting()
{
    m_clientFuture = std::async(std::launch::async, std::bind(&SClientListener::runClient, this));
}

//-----------------------------------------------------------------------------

void SClientListener::stopping()
{
    try
    {
        if(m_client.isConnected())
        {
            m_client.disconnect();
        }

        m_clientFuture.wait();
        m_tlInitialized = false;
        m_sigDisconnected->asyncEmit();
    }
    catch(core::Exception& ex)
    {
        sight::ui::base::dialog::MessageDialog::show("Connection error", ex.what());
        SIGHT_ERROR(ex.what());
    }
}

//-----------------------------------------------------------------------------

void SClientListener::manageTimeline(data::Object::sptr obj, std::size_t index)
{
    core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();

    const auto data    = m_objects[index].lock();
    const auto matTL   = std::dynamic_pointer_cast<data::MatrixTL>(data.get_shared());
    const auto frameTL = std::dynamic_pointer_cast<data::FrameTL>(data.get_shared());

    //MatrixTL
    if(matTL)
    {
        if(!m_tlInitialized)
        {
            matTL->setMaximumSize(10);
            matTL->initPoolSize(1);
            m_tlInitialized = true;
        }

        SPTR(data::MatrixTL::BufferType) matrixBuf;
        matrixBuf = matTL->createBuffer(timestamp);
        data::Matrix4::TMCoefArray values;
        data::Matrix4::sptr t = data::Matrix4::dynamicCast(obj);
        values = t->getCoefficients();
        float matrixValues[16];

        for(unsigned int i = 0 ; i < 16 ; ++i)
        {
            matrixValues[i] = static_cast<float>(values[i]);
        }

        matrixBuf->setElement(matrixValues, 0);
        matTL->pushObject(matrixBuf);
        data::TimeLine::ObjectPushedSignalType::sptr sig;
        sig = matTL->signal<data::TimeLine::ObjectPushedSignalType>(
            data::TimeLine::s_OBJECT_PUSHED_SIG
        );
        sig->asyncEmit(timestamp);
    }
    //FrameTL
    else if(frameTL)
    {
        data::Image::sptr im = data::Image::dynamicCast(obj);
        if(!m_tlInitialized)
        {
            frameTL->setMaximumSize(10);
            frameTL->initPoolSize(im->getSize()[0], im->getSize()[1], im->getType(), im->numComponents());
            m_tlInitialized = true;
        }

        SPTR(data::FrameTL::BufferType) buffer = frameTL->createBuffer(timestamp);

        std::uint8_t* destBuffer = reinterpret_cast<std::uint8_t*>(buffer->addElement(0));

        const auto dumpLock = im->lock();
        auto itr            = im->begin<std::uint8_t>();
        const auto end      = im->end<std::uint8_t>();

        std::copy(itr, end, destBuffer);

        frameTL->pushObject(buffer);

        data::TimeLine::ObjectPushedSignalType::sptr sig;
        sig = frameTL->signal<data::TimeLine::ObjectPushedSignalType>
                  (data::TimeLine::s_OBJECT_PUSHED_SIG);
        sig->asyncEmit(timestamp);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
