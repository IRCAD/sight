/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "less.hpp"

#include <data/integer.hpp>
#include <data/real.hpp>
#include <data/validator/registry/macros.hpp>

#include <string>

namespace sight::data::validator
{

SIGHT_REGISTER_DATA_VALIDATOR(sight::data::validator::less);

//------------------------------------------------------------------------------

void less::configure(const config_t& _config)
{
    const auto value_str = _config.get<std::string>("value", "");
    SIGHT_ERROR_IF("Configuration value is empty", value_str.empty());

    try
    {
        m_threshold = std::stod(value_str);
    }
    catch(const std::exception& e)
    {
        m_threshold = 0.0;
        SIGHT_THROW("Failed to parse threshold value: " << e.what());
    }

    if(!std::isfinite(m_threshold))
    {
        m_threshold = 0.0;
        SIGHT_THROW("Threshold value is not finite (NaN or Infinity): " << m_threshold);
    }
}

//-----------------------------------------------------------------------------

sight::data::validator::return_t less::validate(const data::object::csptr& _object) const
{
    sight::data::validator::return_t validation {false, ""};
    try
    {
        if(const auto real_obj = std::dynamic_pointer_cast<const data::real>(_object); real_obj)
        {
            const double value = real_obj->value();
            if(value < m_threshold)
            {
                validation = {true,
                              "Object value " + real_obj->to_string() + " is less than " + std::to_string(m_threshold)
                };
            }
            else
            {
                validation = {false,
                              "Object value " + real_obj->to_string() + " is not less than "
                              + std::to_string(m_threshold)
                };
            }
        }
        else if(const auto int_obj = std::dynamic_pointer_cast<const data::integer>(_object); int_obj)
        {
            const auto value = static_cast<double>(int_obj->value());
            if(!std::isfinite(value))
            {
                validation = {false,
                              "Object value " + int_obj->to_string() + " is not a valid number (NaN or Infinity)"
                };
            }
            else if(value < m_threshold)
            {
                validation = {true,
                              "Object value " + int_obj->to_string() + " is less than " + std::to_string(m_threshold)
                };
            }
            else
            {
                validation = {false,
                              "Object value " + int_obj->to_string() + " is not less than "
                              + std::to_string(m_threshold)
                };
            }
        }
        else
        {
            validation = {false, "Current object is not sight::data::real or sight::data::integer"};
        }
    }
    catch(const std::exception& e)
    {
        validation = {false, std::string("Error in validation: ") + e.what()};
    }

    return validation;
}

//------------------------------------------------------------------------------

less::auto_connect_signals_t less::auto_connect_signals() const
{
    return {sight::data::signals::MODIFIED};
}

//-----------------------------------------------------------------------------

} // namespace sight::data::validator
