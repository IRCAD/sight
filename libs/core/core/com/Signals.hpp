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

struct SignalBase;

/**
 * @brief   This class proposes a storage for signals.
 */
class CORE_CLASS_API Signals
{
public:

    using key_t                  = std::string;
    using SignalKeyType          = key_t;
    using SignalKeyContainerType = std::vector<key_t>;

    CORE_API Signals()          = default;
    CORE_API virtual ~Signals() = default;

    /// Copy constructor forbidden
    Signals& operator=(const Signals&) = delete;

    /// Registers SignalBase in m_signals
    CORE_API Signals& operator()(const key_t& key, const SPTR(SignalBase)& Signal);

    /// Returns the SignalBase associated to the key, if key does not exist, the ptr is null
    CORE_API SPTR(SignalBase) operator[](const key_t& key) const;

    /// Returns all key_t registered in m_signals
    [[nodiscard]] CORE_API SignalKeyContainerType getSignalKeys() const;

protected:

    /// Copy constructor forbidden
    Signals(const Signals& /*unused*/);

    /// Association < key , SPTR( SignalBase ) >
    using SignalMapType = std::map<key_t, SPTR(SignalBase)>;
    SignalMapType m_signals;
};

} // namespace sight::core::com
