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

#include "core/com/signals.hpp"

#include "core/com/signal_base.hpp"

namespace sight::core::com
{

//-----------------------------------------------------------------------------

signals& signals::operator()(const signal_key_type& key, const signal_base::sptr& signal)
{
    m_signals.insert({key, signal});
    return *this;
}

//-----------------------------------------------------------------------------

signal_base::sptr signals::operator[](const signal_key_type& key) const
{
    auto it = m_signals.find(key);

    if(it != m_signals.end())
    {
        return it->second;
    }

    return {};
}

//-----------------------------------------------------------------------------

signals::signal_key_container_type signals::get_signal_keys() const
{
    signals::signal_key_container_type signal_keys;
    for(const auto& elem : m_signals)
    {
        signal_keys.push_back(elem.first);
    }

    return signal_keys;
}

//-----------------------------------------------------------------------------

signals::signals(const signals& /*unused*/)
{
}

//-----------------------------------------------------------------------------

} // namespace sight::core::com
