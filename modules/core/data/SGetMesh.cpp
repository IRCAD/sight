/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include "SGetMesh.hpp"

#include <regex>

namespace sight::module::data
{

//------------------------------------------------------------------------------

void SGetMesh::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();
    const ConfigType configOut           = config.get_child("out");
    const auto keyCfg                    = configOut.equal_range("key");
    size_t outputVarIndex                = 0;

    // All the configuration is parsed
    for(auto itCfg = keyCfg.first ; itCfg != keyCfg.second ; ++itCfg, ++outputVarIndex)
    {
        // A configuration can be a constraint either on the index or on the type
        auto typeCfg  = itCfg->second.get_child_optional("<xmlattr>.type");
        auto indexCfg = itCfg->second.get_child_optional("<xmlattr>.index");

        if(indexCfg)
        {
            // There is a constraint on the index.
            // The index of the mesh required from the reconstructionDB and the index of the output var are stored.
            m_indexConfigurations.push_back(
                {
                    indexCfg->get_value<std::size_t>(),
                    outputVarIndex
                });
        }
        else if(typeCfg)
        {
            // There is a constraint on the index.
            // The type, the name matching (if there is one) and the index of the output var are stored
            const auto matchingCfg = itCfg->second.get_child_optional("<xmlattr>.matching");

            m_typeConfiguration.push_back(
                {
                    typeCfg->get_value<std::string>(),
                    matchingCfg ? matchingCfg->get<std::string>("") : "",
                    outputVarIndex
                });
        }
    }
}

//------------------------------------------------------------------------------

void SGetMesh::starting()
{
}

//-----------------------------------------------------------------------------

void SGetMesh::updating()
{
    //Get the reconstruction database from which the mesh are extracted
    auto modelSeries = m_modelSeries.lock();
    if(modelSeries == nullptr)
    {
        SIGHT_THROW_EXCEPTION(sight::data::Exception("Missing input models series"));
    }

    const sight::data::ModelSeries::ReconstructionVectorType recs = modelSeries->getReconstructionDB();

    // do all index requests
    for(indexConfig indexCfg : m_indexConfigurations)
    {
        // Get the mesh from the reconstructionDB which is located at the position stored in
        // indexCfg.inputIndex
        if(indexCfg.inputIndex > recs.size())
        {
            SIGHT_THROW_EXCEPTION(sight::data::Exception("Mesh index is out of bound"));
        }

        const auto mesh = modelSeries->getReconstructionDB()[indexCfg.inputIndex]->getMesh();

        // put the mesh in the appropriate output variable
        m_mesh[indexCfg.outputIndex] = mesh;
    }

    // do all type requests
    for(typeConfig typeCfg : m_typeConfiguration)
    {
        //go through the whole reconstructionDB, and check if a mesh which respects the type and regex is found.
        bool found = false;
        for(const auto& reconstruction : modelSeries->getReconstructionDB())
        {
            // check if the type of the reconstruction corresponds to the requested one.
            if(reconstruction->getStructureType() == typeCfg.type)
            {
                // the reconstruction has the right type. Check if the name match the regex, if there is one.
                if(typeCfg.matching.empty()
                   || std::regex_match(reconstruction->getOrganName(), std::regex(typeCfg.matching)))
                {
                    //the type and name are appropriate. It is the mesh required.
                    auto mesh = reconstruction->getMesh();

                    // put the mesh in the appropriate output variable.
                    m_mesh[typeCfg.outputIndex] = mesh;

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
            SIGHT_THROW_EXCEPTION(sight::data::Exception("Type of mesh does not match"));
        }
    }
}

//-----------------------------------------------------------------------------

void SGetMesh::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
