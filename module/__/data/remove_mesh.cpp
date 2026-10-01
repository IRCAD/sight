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

#include "remove_mesh.hpp"

#include <data/exception.hpp>

#include <map>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace sight::module::data
{

//-----------------------------------------------------------------------------

void remove_mesh::configuring()
{
}

//-----------------------------------------------------------------------------

void remove_mesh::starting()
{
}

//-----------------------------------------------------------------------------

void remove_mesh::stopping()
{
}

//-----------------------------------------------------------------------------

void remove_mesh::updating()
{
    const auto model = m_model.lock();
    if(!model)
    {
        SIGHT_THROW_EXCEPTION(sight::data::exception("Missing input model series"));
    }

    struct rule
    {
        std::optional<std::regex> type;
        std::optional<std::regex> name;
    };

    std::map<std::size_t, std::string> organ_types;
    std::map<std::size_t, std::string> organ_names;
    for(const auto index : m_organ_types.indices())
    {
        const auto organ_type = m_organ_types[index].lock();
        if(organ_type)
        {
            organ_types.emplace(index, organ_type->value());
        }
    }

    for(const auto index : m_organ_names.indices())
    {
        const auto organ_name = m_organ_names[index].lock();
        if(organ_name)
        {
            organ_names.emplace(index, organ_name->value());
        }
    }

    std::set<std::size_t> indices;
    for(const auto& index : std::views::keys(organ_types))
    {
        indices.insert(index);
    }

    for(const auto& index : std::views::keys(organ_names))
    {
        indices.insert(index);
    }

    std::vector<rule> rules;
    rules.reserve(indices.size());
    for(const auto index : indices)
    {
        rule current_rule;

        if(const auto it = organ_types.find(index); it != organ_types.end() && !it->second.empty())
        {
            current_rule.type.emplace(it->second);
        }

        if(const auto it = organ_names.find(index); it != organ_names.end() && !it->second.empty())
        {
            current_rule.name.emplace(it->second);
        }

        if(current_rule.type || current_rule.name)
        {
            rules.push_back(std::move(current_rule));
        }
    }

    const auto& reconstructions = model->get_reconstruction_db();
    sight::data::model_series::reconstruction_vector_t kept;
    sight::data::model_series::reconstruction_vector_t removed;
    kept.reserve(reconstructions.size());
    removed.reserve(reconstructions.size());

    for(const auto& reconstruction : reconstructions)
    {
        bool matches = false;
        if(reconstruction)
        {
            for(const auto& current_rule : rules)
            {
                const bool type_matches = !current_rule.type
                                          || std::regex_match(reconstruction->get_structure_type(), *current_rule.type);
                const bool name_matches = !current_rule.name
                                          || std::regex_match(reconstruction->get_organ_name(), *current_rule.name);

                if(type_matches && name_matches)
                {
                    matches = true;
                    break;
                }
            }
        }

        (matches ? removed : kept).push_back(reconstruction);
    }

    if(removed.empty())
    {
        return;
    }

    model->set_reconstruction_db(kept);
    model->async_emit(sight::data::model_series::signals::RECONSTRUCTIONS_REMOVED, removed);
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
