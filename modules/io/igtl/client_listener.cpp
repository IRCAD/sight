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

#include "client_listener.hpp"

#include <core/com/signal.hxx>
#include <core/tools/failed.hpp>

#include <data/frame_tl.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>
#include <data/matrix_tl.hpp>
#include <data/object.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>

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

    const config_t config_in_out = config.get_child("inout");

    SIGHT_ASSERT(
        "configured group must be 'objects'",
        config_in_out.get<std::string>("<xmlattr>.group", "") == "objects"
    );

    const auto key_cfg = config_in_out.equal_range("key");
    for(auto it_cfg = key_cfg.first ; it_cfg != key_cfg.second ; ++it_cfg)
    {
        const service::config_t& attr = it_cfg->second.get_child("<xmlattr>");
        const std::string device_name = attr.get("deviceName", "Sight");
        m_device_names.push_back(device_name);
        m_client.add_authorized_device(device_name);
    }

    m_client.set_filtering_by_device_name(true);

    const std::string server_info = config.get("server", "");
    if(!server_info.empty())
    {
        const std::string::size_type split_position = server_info.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", split_position != std::string::npos);

        m_hostname_config = server_info.substr(0, split_position);
        m_port_config     = server_info.substr(split_position + 1, server_info.size());
    }
    else
    {
        throw core::tools::failed("Server element not found");
    }
}

//-----------------------------------------------------------------------------

void client_listener::run_client()
{
    // 1. Connection
    try
    {
        ui::preferences preferences;
        const auto port     = preferences.delimited_get<std::uint16_t>(m_port_config);
        const auto hostname = preferences.delimited_get<std::string>(m_hostname_config);

        m_client.connect(hostname, port);
        m_sig_connected->async_emit();
    }
    catch(core::exception& ex)
    {
        // Only open a dialog if the service is started.
        // connect may throw if we request the service to stop,
        // in this case opening a dialog will result in a deadlock
        if(this->started())
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
        while(m_client.is_connected())
        {
            std::string device_name;
            data::object::sptr receive_object = m_client.receive_object(device_name);
            if(receive_object)
            {
                const auto& iter = std::find(m_device_names.begin(), m_device_names.end(), device_name);

                if(iter != m_device_names.end())
                {
                    const auto index_receive_object = std::distance(m_device_names.begin(), iter);
                    const auto obj                  = m_objects[static_cast<std::size_t>(index_receive_object)].lock();

                    const bool is_a_timeline = obj->is_a("data::matrix_tl") || obj->is_a("data::frame_tl");
                    if(is_a_timeline)
                    {
                        this->manage_timeline(receive_object, static_cast<std::size_t>(index_receive_object));
                    }
                    else
                    {
                        obj->shallow_copy(receive_object);

                        data::object::modified_signal_t::sptr sig;
                        sig = obj->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
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
        if(this->started())
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
    m_client_future = std::async(std::launch::async, [this](auto&& ...){run_client();});
}

//-----------------------------------------------------------------------------

void client_listener::stopping()
{
    try
    {
        if(m_client.is_connected())
        {
            m_client.disconnect();
        }

        m_client_future.wait();
        m_tl_initialized = false;
        m_sig_disconnected->async_emit();
    }
    catch(core::exception& ex)
    {
        sight::ui::dialog::message::show("Connection error", ex.what());
        SIGHT_ERROR(ex.what());
    }
}

//-----------------------------------------------------------------------------

void client_listener::manage_timeline(data::object::sptr _obj, std::size_t _index)
{
    core::clock::type timestamp = core::clock::get_time_in_milli_sec();

    const auto data     = m_objects[_index].lock();
    const auto mat_tl   = std::dynamic_pointer_cast<data::matrix_tl>(data.get_shared());
    const auto frame_tl = std::dynamic_pointer_cast<data::frame_tl>(data.get_shared());

    //MatrixTL
    if(mat_tl)
    {
        if(!m_tl_initialized)
        {
            mat_tl->set_maximum_size(10);
            mat_tl->init_pool_size(1);
            m_tl_initialized = true;
        }

        SPTR(data::matrix_tl::buffer_t) matrix_buf;
        matrix_buf = mat_tl->create_buffer(timestamp);

        data::matrix4::sptr t = std::dynamic_pointer_cast<data::matrix4>(_obj);
        std::array<float, 16> float_values {};
        std::transform(t->begin(), t->end(), float_values.begin(), boost::numeric_cast<float, double>);

        matrix_buf->set_element(float_values, 0);
        mat_tl->push_object(matrix_buf);
        auto sig = mat_tl->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
        sig->async_emit(timestamp);
    }
    //FrameTL
    else if(frame_tl)
    {
        data::image::sptr im = std::dynamic_pointer_cast<data::image>(_obj);

        if(!m_tl_initialized)
        {
            const auto frame_pixel_format =
                [](data::image::pixel_format_t _image_pixel_format)
                {
                    switch(_image_pixel_format)
                    {
                        case data::image::pixel_format_t::bgr:
                            return data::frame_tl::pixel_format::bgr;

                        case data::image::pixel_format_t::rgb:
                            return data::frame_tl::pixel_format::rgb;

                        case data::image::pixel_format_t::rgba:
                            return data::frame_tl::pixel_format::rgba;

                        case data::image::pixel_format_t::bgra:
                            return data::frame_tl::pixel_format::bgra;

                        case data::image::pixel_format_t::gray_scale:
                            return data::frame_tl::pixel_format::gray_scale;

                        default:
                            return data::frame_tl::pixel_format::undefined;
                    }
                }(im->pixel_format());

            frame_tl->set_maximum_size(10);
            frame_tl->init_pool_size(im->size()[0], im->size()[1], im->type(), frame_pixel_format);
            m_tl_initialized = true;
        }

        SPTR(data::frame_tl::buffer_t) buffer = frame_tl->create_buffer(timestamp);

        auto* dest_buffer = reinterpret_cast<std::uint8_t*>(buffer->add_element(0));

        const auto dump_lock = im->dump_lock();
        auto itr             = im->begin<std::uint8_t>();
        const auto end       = im->end<std::uint8_t>();

        std::copy(itr, end, dest_buffer);

        frame_tl->push_object(buffer);

        data::timeline::signals::pushed_t::sptr sig;
        sig = frame_tl->signal<data::timeline::signals::pushed_t>
                  (data::timeline::signals::PUSHED);
        sig->async_emit(timestamp);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
