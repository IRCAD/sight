/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlCamp/SExtractMeshByType.hpp"

#include <fwData/Mesh.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/registry/ObjectService.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

namespace ctrlCamp
{

fwServicesRegisterMacro(::ctrlCamp::ICamp, ::ctrlCamp::SExtractMeshByType);

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
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

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
                OSLM_ASSERT(
                    "You must have as many 'extract' tags as 'out' keys." << extractCfg.size() << " " <<  keyCfg.size(),
                    extractCfg.size() == keyCfg.size());
                ok = true;
            }
        }
    }
    SLM_ASSERT("Missing 'target' output keys", ok);

    for(ConfigType cfg : extractCfg)
    {
        std::string type  = cfg->getAttributeValue("type");
        std::string regex = cfg->getAttributeValue("matching");

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
    ::fwMedData::ModelSeries::sptr modelSeries = this->getInOut< ::fwMedData::ModelSeries>("source");
    OSLM_ASSERT("ModelSeries not found", modelSeries);

    size_t index = 0;
    for(const auto& elt : m_extract)
    {
        std::string type  = elt.first;
        std::string regex = elt.second;

        bool found = false;
        ::fwMedData::ModelSeries::ReconstructionVectorType recs = modelSeries->getReconstructionDB();
        for(::fwData::Reconstruction::sptr element : recs)
        {
            if(element->getCRefStructureType() == type)
            {
                ::boost::regex regSurface(regex);
                ::boost::smatch match;
                std::string name = element->getOrganName();

                if(regex.empty() || ::boost::regex_match(name, match, regSurface))
                {
                    ::fwData::Mesh::sptr obj = element->getMesh();

                    this->setOutput("target", obj, index);
                    found = true;
                    ++index;

                    break;
                }
            }
        }
        OSLM_ERROR_IF(
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

} // namespace ctrlCamp
