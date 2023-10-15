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

#include "tdata_listener.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>

#include <data/composite.hpp>
#include <data/matrix4.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/Preferences.hpp>

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

    const auto serverInfo = configuration.get<std::string>("server");
    SIGHT_INFO("OpenIGTLinkListener::configure server: " + serverInfo);
    const std::string::size_type splitPosition = serverInfo.find(':');
    SIGHT_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

    m_hostnameConfig = serverInfo.substr(0, splitPosition);
    m_portConfig     = serverInfo.substr(splitPosition + 1, serverInfo.size());

    const auto devices = configuration.equal_range("deviceName");
    std::transform(
        devices.first,
        devices.second,
        std::back_inserter(m_deviceNamesConfig),
        [](const auto& device)
        {
            return device.second.template get_value<std::string>();
        });

    const auto tdata = configuration.get_child("TData");

    for(const auto& m : boost::make_iterator_range(tdata.equal_range("matrix")))
    {
        const auto index = m.second.get<std::uint64_t>("<xmlattr>.index");
        m_matrixNameIndex[m.second.get<std::string>("<xmlattr>.name")] = index;
    }
}

//-----------------------------------------------------------------------------

void tdata_listener::runClient()
{
    data::composite::sptr composite = std::make_shared<data::composite>();

    // 1. Connection
    try
    {
        ui::Preferences preferences;
        const auto port     = preferences.delimited_get<std::uint16_t>(m_portConfig);
        const auto hostname = preferences.delimited_get<std::string>(m_hostnameConfig);

        if(!m_deviceNamesConfig.empty())
        {
            for(const auto& dn : m_deviceNamesConfig)
            {
                m_client.addAuthorizedDevice(preferences.delimited_get<std::string>(dn));
            }

            m_client.setFilteringByDeviceName(true);
        }

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
            double timestamp                 = 0;
            data::object::sptr receiveObject = m_client.receiveObject(deviceName, timestamp);
            if(receiveObject)
            {
                composite->shallow_copy(receiveObject);
                this->manageTimeline(composite, timestamp);
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

void tdata_listener::starting()
{
    const auto matTL = m_timeline.lock();
    matTL->setMaximumSize(10);
    matTL->initPoolSize(static_cast<unsigned int>(m_matrixNameIndex.size()));

    m_clientFuture = std::async(std::launch::async, [this](auto&& ...){runClient();});
}

//-----------------------------------------------------------------------------

void tdata_listener::stopping()
{
    m_client.disconnect();
    m_clientFuture.wait();
    m_sigDisconnected->async_emit();
}

//-----------------------------------------------------------------------------

void tdata_listener::manageTimeline(const data::composite::sptr& obj, double timestamp)
{
    const auto matTL = m_timeline.lock();
    SPTR(data::matrix_tl::BufferType) matrixBuf;
    matrixBuf = matTL->createBuffer(timestamp);

    for(const auto& elt : *obj)
    {
        data::matrix4::csptr transfoMatrix = std::dynamic_pointer_cast<const data::matrix4>(elt.second);

        auto it = m_matrixNameIndex.find(elt.first);

        if(transfoMatrix && it != m_matrixNameIndex.end())
        {
            std::uint64_t index = it->second;

            std::array<float, 16> matrixValues {};
            bool isZero = true;
            for(unsigned int i = 0 ; i < 16 ; ++i)
            {
                matrixValues[i] = static_cast<float>((*transfoMatrix)[i]);
                //Test if matrix contains only '0' except last value (always '1)
                isZero &= i < 15 ? (matrixValues[i] == 0.F) : true;
            }

            //don't push the matrix if it contains only '0'
            if(!isZero)
            {
                matrixBuf->setElement(matrixValues, static_cast<unsigned int>(index));
            }
        }
    }

    matTL->pushObject(matrixBuf);

    data::timeline::signals::pushed_t::sptr sig;
    sig = matTL->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
    sig->async_emit(timestamp);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
