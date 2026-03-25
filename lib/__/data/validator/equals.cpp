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

#include "equals.hpp"

#include <data/string_serializable.hpp>
#include <data/validator/registry/macros.hpp>

#include <algorithm>
#include <string>

namespace sight::data::validator
{

SIGHT_REGISTER_DATA_VALIDATOR(sight::data::validator::equals);

//------------------------------------------------------------------------------

void equals::configure(const config_t& _config)
{
    m_value = _config.get<std::string>("value", "");
    SIGHT_ERROR_IF("Configuration value is empty", m_value.empty());
}

//-----------------------------------------------------------------------------

sight::data::validator::return_t equals::validate(const data::object::csptr& _object) const
{
    if(m_value.empty())
    {
        return {false, "Configured value is empty"};
    }

    sight::data::validator::return_t validation {true, {}};
    try
    {
        if(const auto obj = std::dynamic_pointer_cast<const data::string_serializable>(_object); obj)
        {
            auto string_value = obj->to_string();
            std::ranges::transform(
                string_value,
                string_value.begin(),
                [](unsigned char _c){return std::tolower(_c);});

            if(string_value == m_value)
            {
                validation = {true, "Object contains 'true'"};
            }
            else if(std::stoi(string_value) != 0)
            {
                validation = {true, "Object contains integer != '0'"};
            }
            else
            {
                validation = {false, "Object doesn't contains 'true'"};
            }
        }
        else
        {
            validation = {false, "Current object is not string_serializable"};
        }
    }
    catch(...)
    {
        validation = {false, "Error in validation"};
    }

    return validation;
}

//------------------------------------------------------------------------------

equals::auto_connect_signals_t equals::auto_connect_signals() const
{
    return {sight::data::signals::MODIFIED};
}

//-----------------------------------------------------------------------------

} // namespace sight::data::validator
