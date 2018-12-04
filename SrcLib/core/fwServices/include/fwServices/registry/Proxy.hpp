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

#ifndef __FWSERVICES_REGISTRY_PROXY_HPP__
#define __FWSERVICES_REGISTRY_PROXY_HPP__

#include <vector>


#include <fwCore/base.hpp>
#include <fwCore/mt/types.hpp>

#include <fwCom/SignalBase.hpp>
#include <fwCom/SlotBase.hpp>

#include "fwServices/config.hpp"

namespace fwServices
{
namespace registry
{

/**
 * @brief This class defines a proxy for signal/slot connections.
 *
 * @note All the signals registered in a Proxy's channel are connected to all slots registered in the same channel.
 */
class FWSERVICES_CLASS_API Proxy : public ::fwCore::BaseObject
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((Proxy)(::fwCore::BaseObject), (()), std::make_shared< Proxy >);

    typedef std::string ChannelKeyType;

    /**
     * @brief Structure to regsiter signal and slot informations
     * Contains a signal container, a slot container and a mutex to protect access
     */
    struct SigSlots
    {
        typedef std::set< ::fwCom::SignalBase::sptr > SignalContainerType;
        typedef std::set< ::fwCom::SlotBase::sptr > SlotContainerType;
        SignalContainerType m_signals;
        SlotContainerType m_slots;
        SlotContainerType::iterator m_lastConnectedSlot;

        ::fwCore::mt::ReadWriteMutex m_mutex;
    };

    typedef std::map< ChannelKeyType, SPTR(SigSlots) > ChannelMapType;

    /// Constructor, does nothing
    FWSERVICES_API Proxy();

    /// Destructor, does nothing
    FWSERVICES_API virtual ~Proxy();

    /// Returns an instance of Proxy.
    FWSERVICES_API static Proxy::sptr getDefault();

    /// Registers a signal in the channel. It will be connected to all slots in the channel.
    FWSERVICES_API void connect(ChannelKeyType channel, ::fwCom::SignalBase::sptr signal);

    /// Registers a slot in the channel. It will be connected to all signals in the channel.
    FWSERVICES_API void connect(ChannelKeyType channel, ::fwCom::SlotBase::sptr slot);

    /// Unregisters the signal. Disconnects it from the slots in channel
    FWSERVICES_API void disconnect(ChannelKeyType channel, ::fwCom::SignalBase::sptr signal);

    /// Unregisters the slot. Disconnects it from the signals in channel
    FWSERVICES_API void disconnect(ChannelKeyType channel, ::fwCom::SlotBase::sptr slot);

protected:

    /// Association channels, SigSlot
    ChannelMapType m_channels;

    /// Used to protect the m_channels access.
    mutable ::fwCore::mt::ReadWriteMutex m_channelMutex;

    /// The global instance of the proxy.
    static Proxy::sptr s_currentProxy;

};

} // namespace registry
} // namespace fwServices

#endif // __FWSERVICES_REGISTRY_PROXY_HPP__
