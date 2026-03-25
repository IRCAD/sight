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

#include "has_model.hpp"

#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/validator/registry/macros.hpp>

namespace sight::data::validator
{

SIGHT_REGISTER_DATA_VALIDATOR(sight::data::validator::has_model);

//------------------------------------------------------------------------------

void has_model::configure(const config_t& _config)
{
    m_organs.clear();

    // Parse the configuration to extract organ constraints
    for(const auto& item : _config)
    {
        if(item.first == "organ")
        {
            const auto& organ_config = item.second;
            std::string name         = organ_config.get<std::string>("<xmlattr>.name", "");
            std::string type         = organ_config.get<std::string>("<xmlattr>.type", "");

            if(!name.empty() && !type.empty())
            {
                m_organs.emplace_back(name, type);
            }
        }
    }

    SIGHT_ERROR_IF("Configuration does not contain any valid organ elements", m_organs.empty());
}

//-----------------------------------------------------------------------------

sight::data::validator::return_t has_model::validate(const data::object::csptr& _object) const
{
    if(m_organs.empty())
    {
        return {false, "No organs configured for validation"};
    }

    sight::data::validator::return_t validation {true, {}};

    // Check if object is a model_series
    if(const auto model_series = std::dynamic_pointer_cast<const data::model_series>(_object); model_series)
    {
        const auto& reconstructions = model_series->get_reconstruction_db();

        // Check each required organ
        for(const auto& [required_name, required_type] : m_organs)
        {
            bool found = false;
            for(const auto& reconstruction : reconstructions)
            {
                if(reconstruction->get_organ_name() == required_name
                   && reconstruction->get_structure_type() == required_type)
                {
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                validation.first  = false;
                validation.second = std::format(
                    "Required organ not found: name='{}', type='{}'",
                    required_name,
                    required_type
                );
                return validation;
            }
        }

        validation.first  = true;
        validation.second = "The model contains all required organs";
    }
    else
    {
        validation.first  = false;
        validation.second = "Current object is not a model_series";
    }

    return validation;
}

//------------------------------------------------------------------------------

has_model::auto_connect_signals_t has_model::auto_connect_signals() const
{
    return {sight::data::signals::MODIFIED};
}

//-----------------------------------------------------------------------------

} // namespace sight::data::validator
