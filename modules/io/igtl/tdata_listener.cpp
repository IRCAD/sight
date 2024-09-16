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

#include "tdata_listener.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

#include <data/map.hpp>
#include <data/matrix4.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <functional>
#include <string>

namespace sight::module::io::igtl
{

//-----------------------------------------------------------------------------

void tdata_listener::configuring()
{
    const auto configuration = this->get_config();

    const auto server_info = configuration.get<std::string>("server");
    SIGHT_INFO("OpenIGTLinkListener::configure server: " + server_info);
    const std::string::size_type split_position = server_info.find(':');
    SIGHT_ASSERT("Server info not formatted correctly", split_position != std::string::npos);

    m_hostname_config = server_info.substr(0, split_position);
    m_port_config     = server_info.substr(split_position + 1, server_info.size());

    const auto devices = configuration.equal_range("deviceName");
    std::transform(
        devices.first,
        devices.second,
        std::back_inserter(m_device_names_config),
        [](const auto& _device)
        {
            return _device.second.template get_value<std::string>();
        });

    const auto tdata = configuration.get_child("TData");

    for(const auto& m : boost::make_iterator_range(tdata.equal_range("matrix")))
    {
        const auto index = m.second.get<std::uint64_t>("<xmlattr>.index");
        m_matrix_name_index[m.second.get<std::string>("<xmlattr>.name")] = index;
    }
}

//-----------------------------------------------------------------------------

void tdata_listener::run_client()
{
    data::map::sptr map = std::make_shared<data::map>();

    // 1. Connection
    try
    {
        ui::preferences preferences;
        const auto port     = preferences.delimited_get<std::uint16_t>(m_port_config);
        const auto hostname = preferences.delimited_get<std::string>(m_hostname_config);

        if(!m_device_names_config.empty())
        {
            for(const auto& dn : m_device_names_config)
            {
                m_client.add_authorized_device(preferences.delimited_get<std::string>(dn));
            }

            m_client.set_filtering_by_device_name(true);
        }

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
            double timestamp                  = 0;
            data::object::sptr receive_object = m_client.receive_object(device_name, timestamp);
            if(receive_object)
            {
                map->shallow_copy(receive_object);
                this->manage_timeline(map, timestamp);
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

void tdata_listener::starting()
{
    const auto mat_tl = m_timeline.lock();
    mat_tl->set_maximum_size(10);
    mat_tl->init_pool_size(static_cast<unsigned int>(m_matrix_name_index.size()));

    m_client_future = std::async(std::launch::async, [this](auto&& ...){run_client();});
}

//-----------------------------------------------------------------------------

void tdata_listener::stopping()
{
    m_client.disconnect();
    m_client_future.wait();
    m_sig_disconnected->async_emit();
}

//-----------------------------------------------------------------------------

void tdata_listener::manage_timeline(const data::map::sptr& _obj, double _timestamp)
{
    const auto mat_tl = m_timeline.lock();
    SPTR(data::matrix_tl::buffer_t) matrix_buf;
    matrix_buf = mat_tl->create_buffer(_timestamp);

    for(const auto& elt : *_obj)
    {
        data::matrix4::csptr transfo_matrix = std::dynamic_pointer_cast<const data::matrix4>(elt.second);

        auto it = m_matrix_name_index.find(elt.first);

        if(transfo_matrix && it != m_matrix_name_index.end())
        {
            std::uint64_t index = it->second;

            std::array<float, 16> matrix_values {};
            bool is_zero = true;
            for(unsigned int i = 0 ; i < 16 ; ++i)
            {
                matrix_values[i] = static_cast<float>((*transfo_matrix)[i]);
                //Test if matrix contains only '0' except last value (always '1)
                is_zero &= i < 15 ? (matrix_values[i] == 0.F) : true;
            }

            //don't push the matrix if it contains only '0'
            if(!is_zero)
            {
                matrix_buf->set_element(matrix_values, static_cast<unsigned int>(index));
            }
        }
    }

    mat_tl->push_object(matrix_buf);

    data::timeline::signals::pushed_t::sptr sig;
    sig = mat_tl->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
    sig->async_emit(_timestamp);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
