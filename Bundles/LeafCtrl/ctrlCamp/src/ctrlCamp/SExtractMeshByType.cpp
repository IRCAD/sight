/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlCamp/SExtractMeshByType.hpp"

#include <fwComEd/helper/Composite.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>


namespace ctrlCamp
{

fwServicesRegisterMacro(::ctrlCamp::ICamp, ::ctrlCamp::SExtractMeshByType, ::fwData::Composite);


//-----------------------------------------------------------------------------

SExtractMeshByType::SExtractMeshByType() : m_regex("")
{

}

//-----------------------------------------------------------------------------

SExtractMeshByType::~SExtractMeshByType()
{
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::configuring() throw( ::fwTools::Failed )
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;
    std::vector< ConfigType > extractCfg = m_configuration->find("extract");
    SLM_ASSERT("At least one 'extract' tag is required.", !extractCfg.empty());

    for(ConfigType cfg : extractCfg)
    {
        SLM_ASSERT("Missing attribute 'from'.", cfg->hasAttribute("from"));
        SLM_ASSERT("Missing attribute 'to'.", cfg->hasAttribute("to"));

        std::string from = cfg->getAttributeValue("from");
        std::string to   = cfg->getAttributeValue("to");
        std::string type = cfg->getAttributeValue("type");
        m_regex = cfg->getAttributeValue("matching");

        SLM_ASSERT("'from' attribute must begin with '@'", from.substr(0,1) == "@");

        std::pair< std::string, std::string > pair;
        pair.first  = to;
        pair.second = type;

        m_extract[from] = pair;
    }
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::starting() throw( ::fwTools::Failed )
{
    this->updating();
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::updating() throw( ::fwTools::Failed )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    ::fwData::mt::ObjectWriteLock lock(composite);
    ::fwComEd::helper::Composite compHelper(composite);

    for(ExtractMapType::value_type elt : m_extract)
    {
        std::string from = elt.first;
        std::string to   = elt.second.first;
        std::string type = elt.second.second;

        ::fwData::Object::sptr object = ::fwDataCamp::getObject( composite, from );
        OSLM_ASSERT("Object from '"<< from <<"' not found", object);

        ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(object);

        OSLM_ASSERT("Object from '"<< from <<"' is not a model series", modelSeries);

        ::fwMedData::ModelSeries::ReconstructionVectorType recs = modelSeries->getReconstructionDB();
        for(::fwData::Reconstruction::sptr element : recs)
        {
            if(element->getCRefStructureType() == type)
            {
                ::boost::regex regSurface(m_regex);
                ::boost::smatch match;
                std::string name = element->getOrganName();

                if(m_regex.empty() || ::boost::regex_match(name,match,regSurface))
                {
                    ::fwData::Mesh::sptr obj = element->getMesh();
                    if (composite->find(to) == composite->end())
                    {
                        compHelper.add(to, obj);
                    }
                    else
                    {
                        OSLM_INFO("A key named '"<< to << "' already exists in the composite. object doesn't added.");
                    }

                    break;
                }
            }
        }
        OSLM_INFO_IF("Mesh with organ name matching '" << m_regex << "' and structure type'" << type << "' didn't find", composite->find(
                         to) == composite->end());
    }
    compHelper.notify();
}

//-----------------------------------------------------------------------------

void SExtractMeshByType::stopping() throw( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------


} // namespace ctrlCamp
