/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwCom/config.hpp"

#include "fwCore/macros.hpp"

#include <map>
#include <vector>

namespace fwCom
{

struct SignalBase;

/**
 * @brief   This class proposes a mapping between a SignalKeyType and a SignalBase.
 */
class FWCOM_CLASS_API Signals
{
public:

    typedef std::string SignalKeyType;
    typedef std::map< SignalKeyType, SPTR( SignalBase ) > SignalMapType;
    typedef std::vector < SignalKeyType > SignalKeyContainerType;

    /// Constructor, does nothing
    FWCOM_API Signals();

    /// Constructor, check if all signals are disconnected
    FWCOM_API virtual ~Signals();

    /// Registers SignalBase in m_signals
    FWCOM_API Signals& operator()( const SignalKeyType& key, const SPTR( SignalBase )& Signal );

    /// Returns the SignalBase associated to the key, if key does not exist, the ptr is null
    FWCOM_API SPTR( SignalBase ) operator[]( const SignalKeyType& key ) const;

    /// Returns all SignalKeyType registered in m_signals
    FWCOM_API SignalKeyContainerType getSignalKeys() const;

protected:

    /// Copy constructor forbidden
    Signals( const Signals& );

    /// Copy constructor forbidden
    Signals& operator=( const Signals& );

    /// Association < key , SPTR( SignalBase ) >
    SignalMapType m_signals;
};

} // namespace fwCom
