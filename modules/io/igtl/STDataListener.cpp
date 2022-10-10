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

#include "STDataListener.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>

#include <data/Composite.hpp>
#include <data/Matrix4.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/Preferences.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/range/iterator_range_core.hpp>

#include <functional>
#include <string>

namespace sight::module::io::igtl
{

//-----------------------------------------------------------------------------

void STDataListener::configuring()
{
    const auto configuration = this->getConfigTree();

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

void STDataListener::runClient()
{
    data::Composite::sptr composite = data::Composite::New();

    // 1. Connection
    try
    {
        ui::base::Preferences preferences;
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
            double timestamp                 = 0;
            data::Object::sptr receiveObject = m_client.receiveObject(deviceName, timestamp);
            if(receiveObject)
            {
                composite->shallowCopy(receiveObject);
                this->manageTimeline(composite, timestamp);
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

void STDataListener::starting()
{
    const auto matTL = m_timeline.lock();
    matTL->setMaximumSize(10);
    matTL->initPoolSize(static_cast<unsigned int>(m_matrixNameIndex.size()));

    m_clientFuture = std::async(std::launch::async, [this](auto&& ...){runClient();});
}

//-----------------------------------------------------------------------------

void STDataListener::stopping()
{
    m_client.disconnect();
    m_clientFuture.wait();
    m_sigDisconnected->asyncEmit();
}

//-----------------------------------------------------------------------------

void STDataListener::manageTimeline(const data::Composite::sptr& obj, double timestamp)
{
    const auto matTL = m_timeline.lock();
    SPTR(data::MatrixTL::BufferType) matrixBuf;
    matrixBuf = matTL->createBuffer(timestamp);

    for(const auto& elt : *obj)
    {
        data::Matrix4::csptr transfoMatrix =
            data::Matrix4::dynamicConstCast(elt.second);

        auto it = m_matrixNameIndex.find(elt.first);

        if(transfoMatrix && it != m_matrixNameIndex.end())
        {
            std::uint64_t index = it->second;

            data::Matrix4::TMCoefArray values;
            values = transfoMatrix->getCoefficients();
            std::array<float, 16> matrixValues {};
            bool isZero = true;
            for(unsigned int i = 0 ; i < 16 ; ++i)
            {
                matrixValues[i] = static_cast<float>(values[i]);
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

    data::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = matTL->signal<data::TimeLine::ObjectPushedSignalType>(data::TimeLine::s_OBJECT_PUSHED_SIG);
    sig->asyncEmit(timestamp);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::igtl
