/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/Mesh.hpp>
#include <data/ModelSeries.hpp>
#include <data/mt/ObjectReadLock.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/Reconstruction.hpp>

#include <services/registry/ObjectService.hpp>

#include <regex>

namespace sight::modules::data
{

fwServicesRegisterMacro(sight::modules::data::ICamp, ::sight::modules::data::SExtractMeshByType)

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
    typedef core::runtime::ConfigurationElement::sptr ConfigType;

    const ConfigType inoutCfg = m_configuration->findConfigurationElement("inout");
    SLM_ASSERT("At one 'inout' tag is required.", inoutCfg);

    const std::vector< ConfigType > extractCfg = inoutCfg->find("extract");
    SLM_ASSERT("At least one 'extract' tag is required.", !extractCfg.empty());

    bool ok = false;

    const std::vector< ConfigType > outCfg = m_configuration->find("out");
    for (const auto& cfg : outCfg)
    {
        if(cfg->hasAttribute("group"))
        {
            if(cfg->getAttributeValue("group") == "target")
            {
                const std::vector< ConfigType > keyCfg = cfg->find("key");
                SLM_ASSERT(
                    "You must have as many 'extract' tags as 'out' keys." << extractCfg.size() << " " <<  keyCfg.size(),
                        extractCfg.size() == keyCfg.size());
                ok = true;
            }
        }
    }
    SLM_ASSERT("Missing 'target' output keys", ok);

    for(ConfigType cfg : extractCfg)
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
    sight::data::ModelSeries::sptr modelSeries = this->getInOut< sight::data::ModelSeries>("source");
    SLM_ASSERT("ModelSeries not found", modelSeries);

    sight::data::mt::ObjectReadLock lock(modelSeries);

    size_t index = 0;
    for(const auto& elt : m_extract)
    {
        const std::string type  = elt.first;
        const std::string regex = elt.second;

        bool found                                                    = false;
        const sight::data::ModelSeries::ReconstructionVectorType recs = modelSeries->getReconstructionDB();
        for(const sight::data::Reconstruction::csptr element : recs)
        {
            if(element->getStructureType() == type)
            {
                const std::regex regSurface(regex);
                std::smatch match;
                const std::string name = element->getOrganName();

                if(regex.empty() || std::regex_match(name, match, regSurface))
                {
                    sight::data::Mesh::sptr obj = element->getMesh();

                    this->setOutput("target", obj, index);
                    found = true;
                    ++index;

                    break;
                }
            }
        }
        SLM_ERROR_IF(
            "Mesh with organ name matching '" << regex << "' and structure type'" << type << "' didn't find",
                !found);
    }
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::stopping()
{
    // Unregister outputs
    for (size_t i = 0; i < this->getKeyGroupSize("target"); ++i)
    {
        this->setOutput("target", nullptr, i);
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::modules::data
