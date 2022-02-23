/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SExtractMeshByType.hpp"

#include <core/runtime/ConfigurationElement.hpp>

#include <data/Reconstruction.hpp>

#include <service/registry/ObjectService.hpp>

#include <regex>

namespace sight::module::data
{

//-----------------------------------------------------------------------------

SExtractMeshByType::SExtractMeshByType()
{
}

//-----------------------------------------------------------------------------

SExtractMeshByType::~SExtractMeshByType()
{
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::configuring()
{
    const auto inoutCfg = m_configuration->findConfigurationElement("inout");
    SIGHT_ASSERT("At one 'inout' tag is required.", inoutCfg);

    const auto extractCfg = inoutCfg->find("extract");
    SIGHT_ASSERT("At least one 'extract' tag is required.", !extractCfg.empty());

#ifdef DEBUG
    bool ok = false;
#endif

    const auto outCfg = m_configuration->find("out");
    for(const auto& cfg : outCfg)
    {
        if(cfg->hasAttribute("group"))
        {
            if(cfg->getAttributeValue("group") == s_TARGET)
            {
                const auto keyCfg = cfg->find("key");
                SIGHT_ASSERT(
                    "You must have as many 'extract' tags as 'out' keys." << extractCfg.size() << " " << keyCfg.size(),
                    extractCfg.size() == keyCfg.size()
                );
                #ifdef DEBUG
                ok = true;
                #endif
            }
        }
    }

    SIGHT_ASSERT("Missing 'target' output keys", ok);

    for(auto cfg : extractCfg)
    {
        const std::string type  = cfg->getAttributeValue("type");
        const std::string regex = cfg->getAttributeValue("matching");

        m_extract.push_back(std::make_pair(type, regex));
    }
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::starting()
{
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::updating()
{
    auto modelSeries = m_source.lock();
    SIGHT_ASSERT("ModelSeries not found", modelSeries);

    std::size_t index = 0;
    for(const auto& elt : m_extract)
    {
        const std::string type  = elt.first;
        const std::string regex = elt.second;

        bool found = false;
        for(sight::data::Reconstruction::sptr element : modelSeries->getReconstructionDB())
        {
            if(element->getStructureType() == type)
            {
                const std::regex regSurface(regex);
                std::smatch match;
                const std::string name = element->getOrganName();

                if(regex.empty() || std::regex_match(name, match, regSurface))
                {
                    sight::data::Mesh::sptr obj = element->getMesh();

                    m_target[index] = obj;
                    found           = true;
                    ++index;

                    break;
                }
            }
        }

        SIGHT_ERROR_IF(
            "Mesh with organ name matching '" << regex << "' and structure type'" << type << "' didn't find",
            !found
        );
    }
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::stopping()
{
    // Unregister outputs
    for(std::size_t i = 0 ; i < m_target.size() ; ++i)
    {
        m_target[i] = nullptr;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::data
