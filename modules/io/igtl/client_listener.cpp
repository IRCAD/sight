/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "client_listener.hpp"

#include <core/com/signal.hxx>
#include <core/tools/failed.hpp>

#include <data/frame_tl.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>
#include <data/matrix_tl.hpp>
#include <data/object.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/Preferences.hpp>

#include <functional>
#include <string>

namespace sight::module::io::igtl
{

//-----------------------------------------------------------------------------

client_listener::client_listener()
= default;

//-----------------------------------------------------------------------------

client_listener::~client_listener()
= default;

//-----------------------------------------------------------------------------

void client_listener::configuring()
{
    service::config_t config = this->get_config();

    const config_t configInOut = config.get_child("inout");

    SIGHT_ASSERT(
        "configured group must be 'objects'",
        configInOut.get<std::string>("<xmlattr>.group", "") == "objects"
    );

    const auto keyCfg = configInOut.equal_range("key");
    for(auto itCfg = keyCfg.first ; itCfg != keyCfg.second ; ++itCfg)
    {
        const service::config_t& attr = itCfg->second.get_child("<xmlattr>");
        const std::string deviceName  = attr.get("deviceName", "Sight");
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
        throw core::tools::failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void client_listener::runClient()
{
    // 1. Connection
    try
    {
        ui::Preferences preferences;
        const auto port     = preferences.delimited_get<std::uint16_t>(m_portConfig);
        const auto hostname = preferences.delimited_get<std::string>(m_hostnameConfig);

        m_client.connect(hostname, port);
        m_sigConnected->async_emit();
    }
    catch(core::exception& ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->status() == STARTED)
        {
            sight::ui::dialog::message::show("Connection error", ex.what());
            this->slot(service::slots::STOP)->async_run();
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
            data::object::sptr receiveObject = m_client.receiveObject(deviceName);
            if(receiveObject)
            {
                const auto& iter = std::find(m_deviceNames.begin(), m_deviceNames.end(), deviceName);

                if(iter != m_deviceNames.end())
                {
                    const auto indexReceiveObject = std::distance(m_deviceNames.begin(), iter);
                    const auto obj                = m_objects[static_cast<std::size_t>(indexReceiveObject)].lock();

                    const bool isATimeline = obj->is_a("data::matrix_tl") || obj->is_a("data::frame_tl");
                    if(isATimeline)
                    {
                        this->manageTimeline(receiveObject, static_cast<std::size_t>(indexReceiveObject));
                    }
                    else
                    {
                        obj->shallow_copy(receiveObject);

                        data::object::ModifiedSignalType::sptr sig;
                        sig = obj->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
                        sig->async_emit();
                    }
                }
            }
        }
    }
    catch(core::exception& ex)
    {
        // Only open a dialog if the service is started.
        // ReceiveObject may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->status() == STARTED)
        {
            sight::ui::dialog::message::show("Error", ex.what());
            this->slot(service::slots::STOP)->async_run();
        }
        else
        {
            // Only report the error on console (this normally happens only if we have requested the disconnection)
            SIGHT_ERROR(ex.what());
        }
    }
}

//-----------------------------------------------------------------------------

void client_listener::starting()
{
    m_clientFuture = std::async(std::launch::async, [this](auto&& ...){runClient();});
}

//-----------------------------------------------------------------------------

void client_listener::stopping()
{
    try
    {
        if(m_client.isConnected())
        {
            m_client.disconnect();
        }

        m_clientFuture.wait();
        m_tlInitialized = false;
        m_sigDisconnected->async_emit();
    }
    catch(core::exception& ex)
    {
        sight::ui::dialog::message::show("Connection error", ex.what());
        SIGHT_ERROR(ex.what());
    }
}

//-----------------------------------------------------------------------------

void client_listener::manageTimeline(data::object::sptr obj, std::size_t index)
{
    core::hires_clock::type timestamp = core::hires_clock::get_time_in_milli_sec();

    const auto data    = m_objects[index].lock();
    const auto matTL   = std::dynamic_pointer_cast<data::matrix_tl>(data.get_shared());
    const auto frameTL = std::dynamic_pointer_cast<data::frame_tl>(data.get_shared());

    //MatrixTL
    if(matTL)
    {
        if(!m_tlInitialized)
        {
            matTL->setMaximumSize(10);
            matTL->initPoolSize(1);
            m_tlInitialized = true;
        }

        SPTR(data::matrix_tl::BufferType) matrixBuf;
        matrixBuf = matTL->createBuffer(timestamp);

        data::matrix4::sptr t = std::dynamic_pointer_cast<data::matrix4>(obj);
        std::array<float, 16> floatValues {};
        std::transform(t->begin(), t->end(), floatValues.begin(), boost::numeric_cast<float, double>);

        matrixBuf->setElement(floatValues, 0);
        matTL->pushObject(matrixBuf);
        auto sig = matTL->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
        sig->async_emit(timestamp);
    }
    //FrameTL
    else if(frameTL)
    {
        data::image::sptr im = std::dynamic_pointer_cast<data::image>(obj);

        if(!m_tlInitialized)
        {
            const auto frame_pixel_format =
                [](data::image::PixelFormat image_pixel_format) -> data::frame_tl::PixelFormat
                {
                    switch(image_pixel_format)
                    {
                        case data::image::PixelFormat::BGR:
                            return data::frame_tl::PixelFormat::BGR;

                        case data::image::PixelFormat::RGB:
                            return data::frame_tl::PixelFormat::RGB;

                        case data::image::PixelFormat::RGBA:
                            return data::frame_tl::PixelFormat::RGBA;

                        case data::image::PixelFormat::BGRA:
                            return data::frame_tl::PixelFormat::BGRA;

                        case data::image::PixelFormat::GRAY_SCALE:
                            return data::frame_tl::PixelFormat::GRAY_SCALE;

                        default:
                            return data::frame_tl::PixelFormat::UNDEFINED;
                    }
                }(im->getPixelFormat());

            frameTL->setMaximumSize(10);
            frameTL->initPoolSize(im->size()[0], im->size()[1], im->getType(), frame_pixel_format);
            m_tlInitialized = true;
        }

        SPTR(data::frame_tl::BufferType) buffer = frameTL->createBuffer(timestamp);

        auto* destBuffer = reinterpret_cast<std::uint8_t*>(buffer->addElement(0));

        const auto dumpLock = im->dump_lock();
        auto itr            = im->begin<std::uint8_t>();
        const auto end      = im->end<std::uint8_t>();

        std::copy(itr, end, destBuffer);

        frameTL->pushObject(buffer);

        data::timeline::signals::pushed_t::sptr sig;
        sig = frameTL->signal<data::timeline::signals::pushed_t>
                  (data::timeline::signals::PUSHED);
        sig->async_emit(timestamp);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
