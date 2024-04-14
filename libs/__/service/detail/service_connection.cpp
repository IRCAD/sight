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

    for(const auto& proxy_cfg : m_proxies)
    {
        for(const auto& signal_cfg : proxy_cfg.second.m_signals)
        {
            SIGHT_ASSERT("Invalid signal source", signal_cfg.first == _service.get_id());

            core::com::signal_base::sptr sig = _service.signal(signal_cfg.second);
            SIGHT_ASSERT("Signal '" + signal_cfg.second + "' not found in source '" + signal_cfg.first + "'.", sig);
            try
            {
                proxy->connect(proxy_cfg.second.m_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signal_cfg.second + "' from '" + signal_cfg.first
                    + "' can not be connected to the channel '" + proxy_cfg.second.m_channel
                    + "': " + std::string(e.what())
                );
            }
        }

        for(const auto& slot_cfg : proxy_cfg.second.m_slots)
        {
            // The start slot is connected before the start stage in connectStartSlot(), skip it
            if(slot_cfg.second == sight::service::slots::START)
            {
                continue;
            }

            SIGHT_ASSERT("Invalid slot destination", slot_cfg.first == _service.get_id());

            core::com::slot_base::sptr slot = _service.slot(slot_cfg.second);
            SIGHT_ASSERT("Slot '" + slot_cfg.second + "' not found in source '" + slot_cfg.first + "'.", slot);

            try
            {
                proxy->connect(proxy_cfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slot_cfg.second + "' from '" + slot_cfg.first
                    + "' can not be connected to the channel '" + proxy_cfg.second.m_channel
                    + "': " + std::string(e.what())
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void service_connection::connect_start_slot(const sight::service::base& _service)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const auto& proxy_cfg : m_proxies)
    {
        for(const auto& slot_cfg : proxy_cfg.second.m_slots)
        {
            if(slot_cfg.second != sight::service::slots::START)
            {
                continue;
            }

            SIGHT_ASSERT("Invalid slot destination", slot_cfg.first == _service.get_id());

            core::com::slot_base::sptr slot = _service.slot(slot_cfg.second);
            SIGHT_ASSERT("Slot '" + slot_cfg.second + "' not found in source '" + slot_cfg.first + "'.", slot);

            try
            {
                proxy->connect(proxy_cfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slot_cfg.second + "' from '" + slot_cfg.first + "' can not be connected to the "
                                                                               "channel '" + proxy_cfg.second.m_channel + "': " + std::string(
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

    for(const auto& proxy_cfg : m_proxies)
    {
        for(const auto& signal_cfg : proxy_cfg.second.m_signals)
        {
            SIGHT_ASSERT("Invalid signal source", signal_cfg.first == _service.get_id());

            core::com::signal_base::sptr sig = _service.signal(signal_cfg.second);

            try
            {
                proxy->disconnect(proxy_cfg.second.m_channel, sig);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Signal '" + signal_cfg.second + "' from '" + signal_cfg.first + "' can not be disconnected "
                                                                                     "from the channel '" + proxy_cfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                                     )
                );
            }
        }

        for(const auto& slot_cfg : proxy_cfg.second.m_slots)
        {
            SIGHT_ASSERT("Invalid slot destination", slot_cfg.first == _service.get_id());

            // The start slot is disconnected after the stop stage in disconnectStartSlot(), skip it
            if(slot_cfg.second == sight::service::slots::START)
            {
                continue;
            }

            core::com::slot_base::sptr slot = _service.slot(slot_cfg.second);
            try
            {
                proxy->disconnect(proxy_cfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slot_cfg.second + "' from '" + slot_cfg.first + "' can not be disconnected from the "
                                                                               "channel '" + proxy_cfg.second.m_channel + "': " + std::string(
                        e.what()
                                                                               )
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void service_connection::disconnect_start_slot(const sight::service::base& _service)
{
    core::com::proxy::sptr proxy = core::com::proxy::get();

    for(const auto& proxy_cfg : m_proxies)
    {
        for(const auto& slot_cfg : proxy_cfg.second.m_slots)
        {
            SIGHT_ASSERT("Invalid slot destination", slot_cfg.first == _service.get_id());

            if(slot_cfg.second != sight::service::slots::START)
            {
                continue;
            }

            core::com::slot_base::sptr slot = _service.slot(slot_cfg.second);
            try
            {
                proxy->disconnect(proxy_cfg.second.m_channel, slot);
            }
            catch(const std::exception& e)
            {
                SIGHT_ERROR(
                    "Slot '" + slot_cfg.second + "' from '" + slot_cfg.first + "' can not be disconnected from the "
                                                                               "channel '" + proxy_cfg.second.m_channel + "': " + std::string(
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
