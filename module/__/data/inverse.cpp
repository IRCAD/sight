/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include "inverse.hpp"

#include <data/boolean.hpp>

namespace sight::module::data
{

//-----------------------------------------------------------------------------

inverse::inverse() :
    service::filter(has_signals::signals())
{
}

//------------------------------------------------------------------------------

sight::service::base::connections_t inverse::auto_connections() const
{
    return {
        {m_source, sight::data::signals::MODIFIED, slots::UPDATE}
    };
}

//-----------------------------------------------------------------------------

void inverse::configuring(const config_t& /*_config*/)
{
}

//-----------------------------------------------------------------------------

void inverse::starting()
{
}

//-----------------------------------------------------------------------------

void inverse::updating()
{
    const auto source = m_source.lock();
    const auto target = m_target.lock();

    // Convert source string to boolean
    const std::string source_str = source->to_string();

    // Create a temporary boolean to parse the string
    auto temp_bool = std::make_shared<sight::data::boolean>();
    try
    {
        temp_bool->from_string(source_str);
    }
    catch(const std::exception& e)
    {
        SIGHT_THROW(
            "Cannot convert source '" << source_str << "' to boolean: " << e.what()
        );
    }

    // Get the inverted boolean value
    const bool inverted = !temp_bool->value();

    // Create a boolean object with the inverted value and convert it to string
    auto inverted_bool             = std::make_shared<sight::data::boolean>(inverted);
    const std::string inverted_str = inverted_bool->to_string();

    // Update the target
    target->from_string(inverted_str);
    target->async_emit(sight::data::signals::MODIFIED);
}

//-----------------------------------------------------------------------------

void inverse::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data.
