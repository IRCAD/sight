/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include "translate.hpp"

#include <boost/range/iterator_range_core.hpp>

namespace sight::module::data
{

//-----------------------------------------------------------------------------

translate::translate() :
    service::filter(m_signals)
{
}

//------------------------------------------------------------------------------

sight::service::base::connections_t translate::auto_connections() const
{
    return {
        {m_key, sight::data::object::MODIFIED_SIG, slots::UPDATE}
    };
}

//-----------------------------------------------------------------------------

void translate::configuring(const config_t& _config)
{
    const auto map = _config.get_child("map");
    for(const auto& entry : boost::make_iterator_range(map.equal_range("entry")))
    {
        const auto key   = entry.second.get<std::string>("<xmlattr>.key", {});
        const auto value = entry.second.get<std::string>("<xmlattr>.value", {});
        m_map[key] = value;
    }
}

//-----------------------------------------------------------------------------

void translate::starting()
{
    const auto key   = m_key.lock();
    const auto value = m_value.lock();

    const std::string key_str = key->to_string();
    if(m_map.contains(key_str))
    {
        value->from_string(m_map[key_str]);
    }
}

//-----------------------------------------------------------------------------

void translate::updating()
{
    const auto key   = m_key.lock();
    const auto value = m_value.lock();

    const std::string key_str = key->to_string();
    SIGHT_THROW_IF("Can not map find any entry for key " << std::quoted(key_str), not m_map.contains(key_str));

    value->from_string(m_map[key_str]);
    value->async_emit(sight::data::object::MODIFIED_SIG);
}

//-----------------------------------------------------------------------------

void translate::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
