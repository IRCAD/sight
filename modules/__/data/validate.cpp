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

#include "validate.hpp"

#include <core/com/signal.hxx>

#include <data/validator/factory/new.hpp>

namespace sight::module::data
{

//------------------------------------------------------------------------------

validate::validate()
{
    new_signal<signals::void_t>(signals::VALID);
    new_signal<signals::void_t>(signals::INVALID);
    new_signal<signals::bool_t>(signals::IS_VALID);
    new_signal<signals::bool_t>(signals::IS_INVALID);
}

//------------------------------------------------------------------------------

service::connections_t validate::auto_connections() const
{
    sight::service::connections_t connections_map;
    for(auto& signal_name : m_validator->auto_connect_signals())
    {
        connections_map.push(m_data, signal_name, slots::UPDATE);
    }

    return connections_map;
}

//------------------------------------------------------------------------------

void validate::configuring(const service::config_t& _config)
{
    const auto id = _config.get<std::string>("config.<xmlattr>.id");

    m_validator = sight::data::validator::factory::make(id);
    SIGHT_THROW_IF("Validator " << std::quoted(id) << " can not be instantiated.", m_validator == nullptr);
}

//------------------------------------------------------------------------------

void validate::starting()
{
}

//-----------------------------------------------------------------------------

void validate::updating()
{
    const auto data = m_data.lock();

    const auto [result, _] = m_validator->validate(data.get_shared());

    if(result)
    {
        this->async_emit(signals::VALID);
    }
    else
    {
        this->async_emit(signals::INVALID);
    }

    this->async_emit(signals::IS_VALID, result);
    this->async_emit(signals::IS_INVALID, not result);
}

//-----------------------------------------------------------------------------

void validate::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
