/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
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

#include "fwServices/registry/Proxy.hpp"

namespace fwServices
{
namespace registry
{

//-----------------------------------------------------------------------------

Proxy::sptr Proxy::s_currentProxy = Proxy::New();

//-----------------------------------------------------------------------------

Proxy::Proxy()
{
}

//-----------------------------------------------------------------------------

Proxy::~Proxy()
{
    SLM_ASSERT("There are still " << m_channels.size() << " channel(s) in the Proxy", m_channels.empty());
}

//-----------------------------------------------------------------------------

Proxy::sptr Proxy::getDefault()
{
    return Proxy::s_currentProxy;
}

//-----------------------------------------------------------------------------

void Proxy::connect(ChannelKeyType channel, ::fwCom::SignalBase::sptr signal)
{
    SPTR(SigSlots) sigslots;

    {
        ::fwCore::mt::ReadToWriteLock lock(m_channelMutex);
        ChannelMapType::iterator iter = m_channels.find(channel);

        if (iter == m_channels.end())
        {
            sigslots = SPTR(SigSlots)(new SigSlots);

            ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
            m_channels[channel] = sigslots;
        }
        else
        {
            sigslots = iter->second;
        }
    }

    ::fwCore::mt::WriteLock lock(sigslots->m_mutex);
    auto ret = sigslots->m_signals.insert(signal);

    if(ret.second)
    {
        // Only connect if the signal was not already in the proxy
        for( ::fwCom::SlotBase::sptr slot :  sigslots->m_slots )
        {
            signal->connect( slot );
        }
    }
}

//-----------------------------------------------------------------------------

void Proxy::connect(ChannelKeyType channel, ::fwCom::SlotBase::sptr slot)
{
    SPTR(SigSlots) sigslots;

    {
        ::fwCore::mt::ReadToWriteLock lock(m_channelMutex);
        ChannelMapType::iterator iter = m_channels.find(channel);

        if (iter == m_channels.end())
        {
            sigslots = SPTR(SigSlots)(new SigSlots);

            ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
            m_channels[channel] = sigslots;
        }
        else
        {
            sigslots = iter->second;
        }
    }

    ::fwCore::mt::WriteLock lock(sigslots->m_mutex);
    auto ret = sigslots->m_slots.insert(slot);

    if(ret.second)
    {
        // Only connect if the slot was not already in the proxy
        for( ::fwCom::SignalBase::sptr signal :  sigslots->m_signals )
        {
            signal->connect( slot );
        }
    }
}

//-----------------------------------------------------------------------------

void Proxy::disconnect(ChannelKeyType channel, ::fwCom::SignalBase::sptr signal)
{
    ::fwCore::mt::ReadToWriteLock lock(m_channelMutex);
    ChannelMapType::iterator iter = m_channels.find(channel);

    SLM_ASSERT("channel '" << channel << "' doesn't exist in Proxy.", iter != m_channels.end());
    SPTR(SigSlots) sigslots = iter->second;

    ::fwCore::mt::WriteLock sigSlotLock(sigslots->m_mutex);

    for( ::fwCom::SlotBase::sptr slot :  sigslots->m_slots )
    {
        signal->disconnect( slot );
    }

    SigSlots::SignalContainerType::iterator sigIter;
    sigIter = std::find(sigslots->m_signals.begin(), sigslots->m_signals.end(), signal);
    SLM_ASSERT("Signal is not found", sigIter != sigslots->m_signals.end());
    sigslots->m_signals.erase(sigIter);

    if (sigslots->m_signals.empty() && sigslots->m_slots.empty())
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
        m_channels.erase(channel);
    }
}

//-----------------------------------------------------------------------------

void Proxy::disconnect(ChannelKeyType channel, ::fwCom::SlotBase::sptr slot)
{
    ::fwCore::mt::ReadToWriteLock lock(m_channelMutex);
    ChannelMapType::iterator iter = m_channels.find(channel);

    SLM_ASSERT("channel '" << channel << "' doesn't exist in Proxy.", iter != m_channels.end());
    SPTR(SigSlots) sigslots = iter->second;

    ::fwCore::mt::WriteLock sigSlotLock(sigslots->m_mutex);

    for( ::fwCom::SignalBase::sptr signal :  sigslots->m_signals )
    {
        signal->disconnect( slot );
    }

    SigSlots::SlotContainerType::iterator slotIter;
    slotIter = std::find(sigslots->m_slots.begin(), sigslots->m_slots.end(), slot);
    SLM_ASSERT("Slot is not found", slotIter != sigslots->m_slots.end());
    sigslots->m_slots.erase(slotIter);

    if (sigslots->m_signals.empty() && sigslots->m_slots.empty())
    {
        ::fwCore::mt::UpgradeToWriteLock writeLock(lock);
        m_channels.erase(channel);
    }
}

//-----------------------------------------------------------------------------

} // namespace registry
} // namespace fwServices

