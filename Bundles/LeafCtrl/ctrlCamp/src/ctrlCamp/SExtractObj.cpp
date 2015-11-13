/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlCamp/SExtractObj.hpp"

#include <fwComEd/helper/Composite.hpp>

#include <fwData/Composite.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwRuntime/ConfigurationElement.hpp>


namespace ctrlCamp
{

fwServicesRegisterMacro(::ctrlCamp::ICamp, ::ctrlCamp::SExtractObj, ::fwData::Composite);


//-----------------------------------------------------------------------------

SExtractObj::SExtractObj() :
    m_mode(ModeType::UPDATE)
{

}

//-----------------------------------------------------------------------------

SExtractObj::~SExtractObj()
{
}

//-----------------------------------------------------------------------------

void SExtractObj::configuring() throw( ::fwTools::Failed )
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;
    std::vector< ConfigurationType > extractCfg = m_configuration->find("extract");
    SLM_ASSERT("At least one 'extract' tag is required.", !extractCfg.empty());

    for(ConfigurationType cfg : extractCfg)
    {
        SLM_ASSERT("Missing attribute 'from'.", cfg->hasAttribute("from"));
        SLM_ASSERT("Missing attribute 'to'.", cfg->hasAttribute("to"));

        std::string from = cfg->getAttributeValue("from");
        std::string to   = cfg->getAttributeValue("to");

        SLM_ASSERT("'from' attribute must begin with '@'", from.substr(0,1) == "@");

        m_extract[from] = to;
    }

    const ConfigurationType modeConfig = m_configuration->findConfigurationElement("mode");
    if (modeConfig)
    {
        auto mode = modeConfig->getValue();
        if(mode == "extractOnStart")
        {
            m_mode = ModeType::START;
        }
        else if(mode == "extractOnUpdate")
        {
            m_mode = ModeType::UPDATE;
        }
        else
        {
            SLM_ERROR("Mode " + mode + " unknown. It should be either 'extractOnStart' or 'extractOnUpdate'");
        }
    }
}

//-----------------------------------------------------------------------------

void SExtractObj::starting() throw( ::fwTools::Failed )
{
    if(m_mode == ModeType::START)
    {
        this->extract();
    }
}

//-----------------------------------------------------------------------------

void SExtractObj::updating() throw( ::fwTools::Failed )
{
    if(m_mode == ModeType::UPDATE)
    {
        this->extract();
    }
    else
    {
        SLM_ERROR("Object copy was request but the mode is to 'extractOnStart'");
    }
}

//-----------------------------------------------------------------------------

void SExtractObj::stopping() throw( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SExtractObj::extract()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    ::fwData::mt::ObjectWriteLock lock(composite);
    ::fwComEd::helper::Composite compHelper(composite);

    for(ExtractMapType::value_type elt : m_extract)
    {
        std::string from = elt.first;
        std::string to   = elt.second;

        ::fwData::Object::sptr object = ::fwDataCamp::getObject( composite, from );
        SLM_WARN_IF("Object from '"+ from +"' not found", !object);
        if(object)
        {
            if (composite->find(to) == composite->end())
            {
                compHelper.add(to, object);
            }
            else
            {
                compHelper.swap(to, object);
            }
        }
    }
    compHelper.notify();
}
//-----------------------------------------------------------------------------

} // namespace ctrlCamp
