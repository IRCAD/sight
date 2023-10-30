/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016 IHU Strasbourg
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

#include "core/config.hpp"

#include <core/com/signals.hpp>
#include <core/com/slots.hpp>

#include <utility>

namespace sight::core::com::helper
{

/// Helper class to register proxy connections.
struct proxy_connections
{
    using signal_info_t         = std::pair<std::string, core::com::signals::key_t>;
    using slot_info_t           = std::pair<std::string, core::com::slots::key_t>;
    using slot_info_container_t = std::vector<slot_info_t>;
    using uid_t                 = std::string;
    using key_t                 = std::string;
    using proxy_elt_t           = std::pair<uid_t, key_t>;
    using proxy_elt_vect_t      = std::vector<proxy_elt_t>;

    std::string m_channel;
    proxy_elt_vect_t m_slots;
    proxy_elt_vect_t m_signals;

    proxy_connections() :
        m_channel("undefined")
    {
    }

    proxy_connections(std::string _channel) :
        m_channel(std::move(_channel))
    {
    }

    ~proxy_connections()
    = default;

    //------------------------------------------------------------------------------

    void add_signal_connection(const uid_t& _uid, const key_t& _key)
    {
        m_signals.push_back(std::make_pair(_uid, _key));
    }

    //------------------------------------------------------------------------------

    void add_signal_connection(const signal_info_t& _pair)
    {
        m_signals.push_back(_pair);
    }

    //------------------------------------------------------------------------------

    void add_slot_connection(const uid_t& _uid, const key_t& _key)
    {
        m_slots.push_back(std::make_pair(_uid, _key));
    }

    //------------------------------------------------------------------------------

    void add_slot_connection(const slot_info_t& _pair)
    {
        m_slots.push_back(_pair);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool empty() const
    {
        return m_slots.empty() && m_signals.empty();
    }
};

} // namespace sight::core::com::helper
