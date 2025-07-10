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

#include "is_true.hpp"

#include <data/string_serializable.hpp>
#include <data/validator/registry/macros.hpp>

#include <string>

namespace sight::data::validator
{

SIGHT_REGISTER_DATA_VALIDATOR(sight::data::validator::is_true);

//-----------------------------------------------------------------------------

sight::data::validator::return_t is_true::validate(const data::object::csptr& _object) const
{
    sight::data::validator::return_t validation {true, {}};

    try
    {
        if(const auto obj = std::dynamic_pointer_cast<const data::string_serializable>(_object); obj)
        {
            auto string_value = obj->to_string();
            std::transform(
                string_value.begin(),
                string_value.end(),
                string_value.begin(),
                [](unsigned char _c){return std::tolower(_c);});

            if(string_value == "true")
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

is_true::auto_connect_signals_t is_true::auto_connect_signals() const
{
    return {
        sight::data::object::MODIFIED_SIG
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::data::validator
