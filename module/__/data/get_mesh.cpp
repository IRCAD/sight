/************************************************************************
 *
 * Copyright (C) 2022-2025 IRCAD France
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

#include "get_mesh.hpp"

#include <regex>

namespace sight::module::data
{

//------------------------------------------------------------------------------

void get_mesh::configuring()
{
    service::config_t config  = this->get_config();
    const config_t config_out = config.get_child("out");
    const auto key_cfg        = config_out.equal_range("key");
    size_t output_var_index   = 0;

    // All the configuration is parsed
    for(auto it_cfg = key_cfg.first ; it_cfg != key_cfg.second ; ++it_cfg, ++output_var_index)
    {
        // A configuration can be a constraint either on the index or on the type
        auto type_cfg  = it_cfg->second.get_child_optional("<xmlattr>.type");
        auto index_cfg = it_cfg->second.get_child_optional("<xmlattr>.index");

        if(index_cfg)
        {
            // There is a constraint on the index.
            // The index of the mesh required from the reconstructionDB and the index of the output var are stored.
            m_index_configurations.push_back(
                {
                    index_cfg->get_value<std::size_t>(),
                    output_var_index
                });
        }
        else if(type_cfg)
        {
            // There is a constraint on the index.
            // The type, the name matching (if there is one) and the index of the output var are stored
            const auto matching_cfg = it_cfg->second.get_child_optional("<xmlattr>.matching");

            m_type_configuration.push_back(
                {
                    type_cfg->get_value<std::string>(),
                    matching_cfg ? matching_cfg->get<std::string>("") : "",
                    output_var_index
                });
        }
    }
}

//------------------------------------------------------------------------------

void get_mesh::starting()
{
}

//-----------------------------------------------------------------------------

void get_mesh::updating()
{
    //Get the reconstruction database from which the mesh are extracted
    auto model_series = m_model_series.lock();
    if(model_series == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::exception("Missing input models series"));
    }

    const sight::data::model_series::reconstruction_vector_t recs = model_series->get_reconstruction_db();

    // do all index requests
    for(index_config index_cfg : m_index_configurations)
    {
        // Get the mesh from the reconstructionDB which is located at the position stored in
        // indexCfg.inputIndex
        if(index_cfg.input_index >= recs.size())
        {
            SIGHT_THROW_EXCEPTION(sight::data::exception("Mesh index is out of bound"));
        }

        const auto mesh = model_series->get_reconstruction_db()[index_cfg.input_index]->get_mesh();

        // put the mesh in the appropriate output variable
        m_mesh[index_cfg.output_index] = mesh;
    }

    // do all type requests
    for(const type_config& type_cfg : m_type_configuration)
    {
        //go through the whole reconstructionDB, and check if a mesh which respects the type and regex is found.
        bool found = false;
        for(const auto& reconstruction : model_series->get_reconstruction_db())
        {
            // check if the type of the reconstruction corresponds to the requested one.
            if(reconstruction->get_structure_type() == type_cfg.type)
            {
                // the reconstruction has the right type. Check if the name match the regex, if there is one.
                if(type_cfg.matching.empty()
                   || std::regex_match(reconstruction->get_organ_name(), std::regex(type_cfg.matching)))
                {
                    //the type and name are appropriate. It is the mesh required.
                    auto mesh = reconstruction->get_mesh();

                    // put the mesh in the appropriate output variable.
                    m_mesh[type_cfg.output_index] = mesh;

                    //When the appropriate mesh is found for a given type-regex, there is no need to go further in the
                    // reconstructionDB.
                    found = true;
                    break;
                }
            }
        }

        // if the whole reconstructionDB has been parsed, and no mesh corresponds to the given type/name, throw
        // exception.
        if(!found)
        {
            SIGHT_THROW_EXCEPTION(sight::data::exception("Type of mesh does not match"));
        }
    }
}

//-----------------------------------------------------------------------------

void get_mesh::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
