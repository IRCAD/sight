/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "service/config.hpp"

#include <core/base.hpp>
#include <core/com/SignalBase.hpp>
#include <core/com/SlotBase.hpp>
#include <core/mt/types.hpp>

#include <vector>

namespace sight::service
{
namespace registry
{

/**
 * @brief This class defines a proxy for signal/slot connections.
 *
 * @note All the signals registered in a Proxy's channel are connected to all slots registered in the same channel.
 */
class SERVICE_CLASS_API Proxy : public core::BaseObject
{

public:

    SIGHT_DECLARE_CLASS(Proxy, core::BaseObject, std::make_shared< Proxy >)

    typedef std::string ChannelKeyType;

    /**
     * @brief Structure to regsiter signal and slot informations
     * Contains a signal container, a slot container and a mutex to protect access
     */
    struct SigSlots
    {
        typedef std::set< core::com::SignalBase::sptr > SignalContainerType;
        typedef std::set< core::com::SlotBase::sptr > SlotContainerType;
        SignalContainerType m_signals;
        SlotContainerType m_slots;
        SlotContainerType::iterator m_lastConnectedSlot;

        core::mt::ReadWriteMutex m_mutex;
    };

    typedef std::map< ChannelKeyType, SPTR(SigSlots) > ChannelMapType;

    /// Constructor, does nothing
    SERVICE_API Proxy();

    /// Destructor, does nothing
    SERVICE_API virtual ~Proxy();

    /// Returns an instance of Proxy.
    SERVICE_API static Proxy::sptr getDefault();

    /// Registers a signal in the channel. It will be connected to all slots in the channel.
    SERVICE_API void connect(ChannelKeyType channel, core::com::SignalBase::sptr signal);

    /// Registers a slot in the channel. It will be connected to all signals in the channel.
    SERVICE_API void connect(ChannelKeyType channel, core::com::SlotBase::sptr slot);

    /// Unregisters the signal. Disconnects it from the slots in channel
    SERVICE_API void disconnect(ChannelKeyType channel, core::com::SignalBase::sptr signal);

    /// Unregisters the slot. Disconnects it from the signals in channel
    SERVICE_API void disconnect(ChannelKeyType channel, core::com::SlotBase::sptr slot);

protected:

    /// Association channels, SigSlot
    ChannelMapType m_channels;

    /// Used to protect the m_channels access.
    mutable core::mt::ReadWriteMutex m_channelMutex;

    /// The global instance of the proxy.
    static Proxy::sptr s_currentProxy;

};

} // namespace registry
} // namespace sight::service
