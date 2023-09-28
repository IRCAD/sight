/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

void proxy::connect(channel_key_type channel, core::com::signal_base::sptr signal)
{
    SPTR(sig_slots) sigslots;

    {
        core::mt::read_to_write_lock lock(m_channel_mutex);
        auto iter = m_channels.find(channel);

        if(iter == m_channels.end())
        {
            sigslots = SPTR(sig_slots)(new sig_slots);

            core::mt::upgrade_to_write_lock write_lock(lock);
            m_channels[channel] = sigslots;
        }
        else
        {
            sigslots = iter->second;
        }
    }

    core::mt::write_lock lock(sigslots->m_mutex);
    auto ret = sigslots->m_signals.insert(signal);

    if(ret.second)
    {
        // Only connect if the signal was not already in the proxy
        for(const core::com::slot_base::sptr& slot : sigslots->m_slots)
        {
            signal->connect(slot);
        }
    }
}

//-----------------------------------------------------------------------------

void proxy::connect(channel_key_type channel, core::com::slot_base::sptr slot)
{
    SPTR(sig_slots) sigslots;

    {
        core::mt::read_to_write_lock lock(m_channel_mutex);
        auto iter = m_channels.find(channel);

        if(iter == m_channels.end())
        {
            sigslots = SPTR(sig_slots)(new sig_slots);

            core::mt::upgrade_to_write_lock write_lock(lock);
            m_channels[channel] = sigslots;
        }
        else
        {
            sigslots = iter->second;
        }
    }

    core::mt::write_lock lock(sigslots->m_mutex);
    auto ret = sigslots->m_slots.insert(slot);

    if(ret.second)
    {
        // Only connect if the slot was not already in the proxy
        for(const core::com::signal_base::sptr& signal : sigslots->m_signals)
        {
            signal->connect(slot);
        }
    }
}

//-----------------------------------------------------------------------------

void proxy::disconnect(channel_key_type channel, core::com::signal_base::sptr signal)
{
    core::mt::read_to_write_lock lock(m_channel_mutex);
    auto iter = m_channels.find(channel);

    SIGHT_ASSERT("channel '" << channel << "' doesn't exist in Proxy.", iter != m_channels.end());
    SPTR(sig_slots) sigslots = iter->second;

    core::mt::write_lock sig_slot_lock(sigslots->m_mutex);

    for(const core::com::slot_base::sptr& slot : sigslots->m_slots)
    {
        signal->disconnect(slot);
    }

    sig_slots::signal_container_type::iterator sig_iter;
    sig_iter = std::find(sigslots->m_signals.begin(), sigslots->m_signals.end(), signal);
    SIGHT_ASSERT("Signal is not found", sig_iter != sigslots->m_signals.end());
    sigslots->m_signals.erase(sig_iter);

    if(sigslots->m_signals.empty() && sigslots->m_slots.empty())
    {
        core::mt::upgrade_to_write_lock write_lock(lock);
        m_channels.erase(channel);
    }
}

//-----------------------------------------------------------------------------

void proxy::disconnect(channel_key_type channel, core::com::slot_base::sptr slot)
{
    core::mt::read_to_write_lock lock(m_channel_mutex);
    auto iter = m_channels.find(channel);

    SIGHT_ASSERT("channel '" << channel << "' doesn't exist in Proxy.", iter != m_channels.end());
    SPTR(sig_slots) sigslots = iter->second;

    core::mt::write_lock sig_slot_lock(sigslots->m_mutex);

    for(const core::com::signal_base::sptr& signal : sigslots->m_signals)
    {
        signal->disconnect(slot);
    }

    sig_slots::slot_container_type::iterator slot_iter;
    slot_iter = std::find(sigslots->m_slots.begin(), sigslots->m_slots.end(), slot);
    SIGHT_ASSERT("Slot is not found", slot_iter != sigslots->m_slots.end());
    sigslots->m_slots.erase(slot_iter);

    if(sigslots->m_signals.empty() && sigslots->m_slots.empty())
    {
        core::mt::upgrade_to_write_lock write_lock(lock);
        m_channels.erase(channel);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
