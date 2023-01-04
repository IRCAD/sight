/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/com/Signals.hpp"

#include "core/com/SignalBase.hpp"

namespace sight::core::com
{

//-----------------------------------------------------------------------------

Signals& Signals::operator()(const SignalKeyType& key, const SignalBase::sptr& Signal)
{
    m_signals.insert({key, Signal});
    return *this;
}

//-----------------------------------------------------------------------------

SignalBase::sptr Signals::operator[](const SignalKeyType& key) const
{
    auto it = m_signals.find(key);

    if(it != m_signals.end())
    {
        return it->second;
    }

    return {};
}

//-----------------------------------------------------------------------------

Signals::SignalKeyContainerType Signals::getSignalKeys() const
{
    Signals::SignalKeyContainerType SignalKeys;
    for(const auto& elem : m_signals)
    {
        SignalKeys.push_back(elem.first);
    }

    return SignalKeys;
}

//-----------------------------------------------------------------------------

Signals::Signals(const Signals& /*unused*/)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
