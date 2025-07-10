/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "core/com/proxy.hpp"

#include <core/com/exception/bad_slot.hpp>
namespace sight::core::com
{

//-----------------------------------------------------------------------------

proxy::sptr s_current_proxy = std::make_shared<proxy>();

//-----------------------------------------------------------------------------

proxy::~proxy()
{
    SIGHT_ASSERT("There are still " << m_channels.size() << " channel(s) in the Proxy", m_channels.empty());
}

//-----------------------------------------------------------------------------

proxy::sptr proxy::get()
{
    return s_current_proxy;
}

//-----------------------------------------------------------------------------

void proxy::connect(channel_key_type _channel, core::com::signal_base::sptr _signal)
{
    auto sigslots = find_or_create_channel(_channel);

    core::mt::write_lock lock(sigslots->m_mutex);
    auto ret = sigslots->m_signals.insert(_signal);

    if(ret.second)
    {
        try
        {
            // Only connect if the signal was not already in the proxy
            for(const auto& slot : sigslots->m_slots)
            {
                _signal->connect(slot);
            }
        }
        catch(core::com::exception::bad_slot& e)
        {
            SIGHT_ERROR("Can't connect signal with slot in proxy '" << _channel << "' : " << e.what() << ".");
            if(ret.second)
            {
                sigslots->m_signals.erase(_signal);
            }

            // No need to remove the channel, if there is an incompatibility, this implies the channel was created for
            // a slot, thus it will be removed symmetrically
        }
    }
}

//-----------------------------------------------------------------------------

void proxy::connect(channel_key_type _channel, core::com::slot_base::sptr _slot)
{
    auto sigslots = find_or_create_channel(_channel);

    core::mt::write_lock lock(sigslots->m_mutex);
    auto ret = sigslots->m_slots.insert(_slot);

    if(ret.second)
    {
        try
        {
            // Only connect if the slot was not already in the proxy
            for(const core::com::signal_base::sptr& signal : sigslots->m_signals)
            {
                signal->connect(_slot);
            }
        }
        catch(core::com::exception::bad_slot& e)
        {
            SIGHT_ERROR("Can't connect signal with slot in proxy '" << _channel << "' : " << e.what() << ".");
            if(ret.second)
            {
                sigslots->m_slots.erase(_slot);
            }

            // No need to remove the channel, if there is an incompatibility, this implies the channel was created for
            // a signal, thus it will be removed symmetrically
        }
    }
}

//-----------------------------------------------------------------------------

void proxy::disconnect(channel_key_type _channel, core::com::signal_base::sptr _signal)
{
    core::mt::read_to_write_lock lock(m_channel_mutex);
    auto iter = m_channels.find(_channel);

    SIGHT_ASSERT("channel '" << _channel << "' doesn't exist in Proxy.", iter != m_channels.end());
    auto sigslots = iter->second;

    core::mt::write_lock sig_slot_lock(sigslots->m_mutex);

    for(const auto& slot : sigslots->m_slots)
    {
        _signal->disconnect(slot);
    }

    const auto sig_iter = std::find(sigslots->m_signals.begin(), sigslots->m_signals.end(), _signal);
    SIGHT_ASSERT("Signal is not found", sig_iter != sigslots->m_signals.end());
    sigslots->m_signals.erase(sig_iter);

    if(sigslots->m_signals.empty() && sigslots->m_slots.empty())
    {
        core::mt::upgrade_to_write_lock write_lock(lock);
        m_channels.erase(_channel);
    }
}

//-----------------------------------------------------------------------------

void proxy::disconnect(channel_key_type _channel, core::com::slot_base::sptr _slot)
{
    core::mt::read_to_write_lock lock(m_channel_mutex);
    auto iter = m_channels.find(_channel);

    SIGHT_ASSERT("channel '" << _channel << "' doesn't exist in Proxy.", iter != m_channels.end());
    auto sigslots = iter->second;

    core::mt::write_lock sig_slot_lock(sigslots->m_mutex);

    for(const auto& signal : sigslots->m_signals)
    {
        signal->disconnect(_slot);
    }

    const auto slot_iter = std::find(sigslots->m_slots.begin(), sigslots->m_slots.end(), _slot);
    SIGHT_ASSERT("Slot is not found", slot_iter != sigslots->m_slots.end());
    sigslots->m_slots.erase(slot_iter);

    if(sigslots->m_signals.empty() && sigslots->m_slots.empty())
    {
        core::mt::upgrade_to_write_lock write_lock(lock);
        m_channels.erase(_channel);
    }
}

//------------------------------------------------------------------------------

SPTR(proxy::sig_slots_t) proxy::find_or_create_channel(channel_key_type _channel)
{
    SPTR(sig_slots_t) sigslots;

    {
        core::mt::read_to_write_lock lock(m_channel_mutex);
        auto iter = m_channels.find(_channel);

        if(iter == m_channels.end())
        {
            sigslots = std::make_shared<sig_slots_t>();

            core::mt::upgrade_to_write_lock write_lock(lock);
            m_channels[_channel] = sigslots;
        }
        else
        {
            sigslots = iter->second;
        }
    }

    return sigslots;
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
