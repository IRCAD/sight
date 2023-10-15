/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "service/detail/service_connection.hpp"

#include "core/com/proxy.hpp"

#include "service/base.hpp"

#include <core/com/signal.hxx>

namespace sight::service::detail
{

//-----------------------------------------------------------------------------

void service_connection::connect(const sight::service::base& _service)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& signalCfg : proxyCfg.second.m_signals)
        {
            SIGHT_ASSERT("Invalid signal source", signalCfg.first == _service.get_id());

            core::com::signal_base::sptr sig = _service.signal(signalCfg.second);
            SIGHT_ASSERT("Signal '" + signalCfg.second + "' not found in source '" + signalCfg.first + "'.", sig);
            try
            {
                proxy->connect(proxyCfg.second.m_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signalCfg.second + "' from '" + signalCfg.first
                    + "' can not be connected to the channel '" + proxyCfg.second.m_channel
                    + "': " + std::string(e.what())
                );
            }
        }

        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            // The start slot is connected before the start stage in connectStartSlot(), skip it
            if(slotCfg.second == sight::service::slots::START)
            {
                continue;
            }

            SIGHT_ASSERT("Invalid slot destination", slotCfg.first == _service.get_id());

            core::com::slot_base::sptr slot = _service.slot(slotCfg.second);
            SIGHT_ASSERT("Slot '" + slotCfg.second + "' not found in source '" + slotCfg.first + "'.", slot);

            try
            {
                proxy->connect(proxyCfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slotCfg.second + "' from '" + slotCfg.first
                    + "' can not be connected to the channel '" + proxyCfg.second.m_channel
                    + "': " + std::string(e.what())
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void service_connection::connectStartSlot(const sight::service::base& _service)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            if(slotCfg.second != sight::service::slots::START)
            {
                continue;
            }

            SIGHT_ASSERT("Invalid slot destination", slotCfg.first == _service.get_id());

            core::com::slot_base::sptr slot = _service.slot(slotCfg.second);
            SIGHT_ASSERT("Slot '" + slotCfg.second + "' not found in source '" + slotCfg.first + "'.", slot);

            try
            {
                proxy->connect(proxyCfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be connected to the "
                                                                             "channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                             )
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void service_connection::disconnect(const sight::service::base& _service)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& signalCfg : proxyCfg.second.m_signals)
        {
            SIGHT_ASSERT("Invalid signal source", signalCfg.first == _service.get_id());

            core::com::signal_base::sptr sig = _service.signal(signalCfg.second);

            try
            {
                proxy->disconnect(proxyCfg.second.m_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signalCfg.second + "' from '" + signalCfg.first + "' can not be disconnected "
                                                                                   "from the channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                                   )
                );
            }
        }

        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            SIGHT_ASSERT("Invalid slot destination", slotCfg.first == _service.get_id());

            // The start slot is disconnected after the stop stage in disconnectStartSlot(), skip it
            if(slotCfg.second == sight::service::slots::START)
            {
                continue;
            }

            core::com::slot_base::sptr slot = _service.slot(slotCfg.second);
            try
            {
                proxy->disconnect(proxyCfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be disconnected from the "
                                                                             "channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                             )
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void service_connection::disconnectStartSlot(const sight::service::base& _service)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const auto& proxyCfg : m_proxies)
    {
        for(const auto& slotCfg : proxyCfg.second.m_slots)
        {
            SIGHT_ASSERT("Invalid slot destination", slotCfg.first == _service.get_id());

            if(slotCfg.second != sight::service::slots::START)
            {
                continue;
            }

            core::com::slot_base::sptr slot = _service.slot(slotCfg.second);
            try
            {
                proxy->disconnect(proxyCfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slotCfg.second + "' from '" + slotCfg.first + "' can not be disconnected from the "
                                                                             "channel '" + proxyCfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                             )
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void service_connection::add(const core::com::helper::proxy_connections& _proxy)
{
    m_proxies[_proxy.m_channel] = _proxy;
}

} // namespace sight::service::detail
