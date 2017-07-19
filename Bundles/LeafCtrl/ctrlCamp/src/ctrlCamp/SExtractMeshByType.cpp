/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlCamp/SExtractMeshByType.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/registry/ObjectService.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

namespace ctrlCamp
{

fwServicesRegisterMacro(::ctrlCamp::ICamp, ::ctrlCamp::SExtractMeshByType, ::fwData::Composite);


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

    if(this->isVersion2())
    {
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
    else
    {
        /// Old way
        std::vector< ConfigType > extractCfg = m_configuration->find("extract");
        SLM_ASSERT("At least one 'extract' tag is required.", !extractCfg.empty());

        for(ConfigType cfg : extractCfg)
        {
            SLM_ASSERT("Missing attribute 'from'.", cfg->hasAttribute("from"));
            SLM_ASSERT("Missing attribute 'to'.", cfg->hasAttribute("to"));

            std::string from  = cfg->getAttributeValue("from");
            std::string to    = cfg->getAttributeValue("to");
            std::string type  = cfg->getAttributeValue("type");
            std::string regex = cfg->getAttributeValue("matching");

            SLM_ASSERT("'from' attribute must begin with '@'", from.substr(0,1) == "@");

            m_extractOld[from] = std::make_tuple(to, type, regex);
        }
    }
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::starting()
{
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::updating()
{
    if(this->isVersion2())
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
    else
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
        ::fwData::mt::ObjectWriteLock lock(composite);
        ::fwDataTools::helper::Composite compHelper(composite);

        for(ExtractMapType::value_type elt : m_extractOld)
        {
            std::string from  = elt.first;
            std::string to    = std::get<0>(elt.second);
            std::string type  = std::get<1>(elt.second);
            std::string regex = std::get<2>(elt.second);

            ::fwData::Object::sptr object = ::fwDataCamp::getObject( composite, from );
            OSLM_ASSERT("Object from '"<< from <<"' not found", object);

            ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(object);
            OSLM_ASSERT("Object from '"<< from <<"' is not a model series", modelSeries);

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
                        if (composite->find(to) == composite->end())
                        {
                            compHelper.add(to, obj);
                        }
                        else
                        {
                            OSLM_INFO("A key named '"<< to <<
                                      "' already exists in the composite. object doesn't added.");
                        }

                        break;
                    }
                }
            }
            OSLM_INFO_IF(
                "Mesh with organ name matching '" << regex << "' and structure type'" << type << "' didn't find",
                composite->find(to) == composite->end());
        }
        compHelper.notify();
    }
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::stopping()
{
    // Unregister outputs
    for (size_t i = 0; i<this->getKeyGroupSize("target"); ++i)
    {
        this->setOutput("target", nullptr, i);
    }
}

//-----------------------------------------------------------------------------


} // namespace ctrlCamp
