/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "core/config.hpp"
#include "core/macros.hpp"

#include <map>
#include <vector>

namespace sight::core::com
{

struct signal_base;

/**
 * @brief   This class proposes a storage for signals.
 */
class CORE_CLASS_API signals
{
public:

    using key_t                  = std::string;
    using signal_key_type        = key_t;
    using signal_key_container_t = std::vector<key_t>;

    CORE_API signals()          = default;
    CORE_API virtual ~signals() = default;

    /// Copy constructor forbidden
    signals& operator=(const signals&) = delete;

    /// Registers SignalBase in m_signals
    CORE_API signals& operator()(const key_t& _key, const SPTR(signal_base)& _signal);

    /// Returns the SignalBase associated to the key, if key does not exist, the ptr is null
    CORE_API SPTR(signal_base) operator[](const key_t& _key) const;

    /// Returns all key_t registered in m_signals
    [[nodiscard]] CORE_API signal_key_container_t get_signal_keys() const;

protected:

    /// Copy constructor forbidden
    signals(const signals& /*unused*/);

    /// Association < key , SPTR( SignalBase ) >
    using signal_map_type = std::map<key_t, SPTR(signal_base)>;
    signal_map_type m_signals;
};

} // namespace sight::core::com
