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

#include "core/config.hpp"
#include "core/macros.hpp"

#include <map>
#include <vector>

namespace sight::core::com
{

struct SignalBase;

/**
 * @brief   This class proposes a mapping between a SignalKeyType and a SignalBase.
 */
class CORE_CLASS_API Signals
{
public:

    typedef std::string SignalKeyType;
    typedef std::map<SignalKeyType, SPTR(SignalBase)> SignalMapType;
    typedef std::vector<SignalKeyType> SignalKeyContainerType;

    /// Constructor, does nothing
    CORE_API Signals();

    /// Constructor, check if all signals are disconnected
    CORE_API virtual ~Signals();

    /// Registers SignalBase in m_signals
    CORE_API Signals& operator()(const SignalKeyType& key, const SPTR(SignalBase)& Signal);

    /// Returns the SignalBase associated to the key, if key does not exist, the ptr is null
    CORE_API SPTR(SignalBase) operator[](const SignalKeyType& key) const;

    /// Returns all SignalKeyType registered in m_signals
    CORE_API SignalKeyContainerType getSignalKeys() const;

protected:

    /// Copy constructor forbidden
    Signals(const Signals&);

    /// Copy constructor forbidden
    Signals& operator=(const Signals&);

    /// Association < key , SPTR( SignalBase ) >
    SignalMapType m_signals;
};

} // namespace sight::core::com
