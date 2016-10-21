/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlCamp/SExtractObj.hpp"

#include <fwData/Composite.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataCamp/exception/NullPointer.hpp>
#include <fwDataCamp/exception/ObjectNotFound.hpp>
#include <fwDataCamp/getObject.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/registry/ObjectService.hpp>


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

    if(this->isVersion2())
    {
        const ConfigurationType inoutCfg = m_configuration->findConfigurationElement("inout");
        SLM_ASSERT("At one 'inout' tag is required.", inoutCfg);

        const std::vector< ConfigurationType > extractCfg = inoutCfg->find("extract");
        SLM_ASSERT("At least one 'extract' tag is required.", !extractCfg.empty());

        const ConfigurationType outCfg = m_configuration->findConfigurationElement("out");

        const std::vector< ConfigurationType > outKeyCfg = outCfg->find("key");
        SLM_ASSERT("You must have as many 'from' tags as 'out' keys.", extractCfg.size() == outKeyCfg.size());

        for(ConfigurationType cfg : extractCfg)
        {
            SLM_ASSERT("Missing attribute 'from'.", cfg->hasAttribute("from"));
            const std::string from = cfg->getAttributeValue("from");

            m_sourcePaths.push_back(from);
        }
    }
    else
    {
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
    // Unregister outputs
    for (size_t i = 0; i<this->getKeyGroupSize("target"); ++i)
    {
        this->setOutput("target", nullptr, i);
    }
}

//-----------------------------------------------------------------------------

void SExtractObj::extract()
{
    if(this->isVersion2())
    {
        auto sourceObject = this->getInOut< ::fwData::Object >("source");

        size_t index = 0;
        for(auto path : m_sourcePaths)
        {
            const std::string from = path;

            ::fwData::Object::sptr object;
            try
            {
                object = ::fwDataCamp::getObject( sourceObject, from, true );
            }
            catch(::fwDataCamp::exception::NullPointer& np)
            {
                this->setOutput("target", nullptr, index);
            }
            catch(::fwDataCamp::exception::ObjectNotFound& nf)
            {
                SLM_WARN("Object from '"+ from +"' not found");
            }
            catch(std::exception& e)
            {
                OSLM_FATAL("Unhandled exception: " << e.what());
            }

            SLM_WARN_IF("Object from '"+ from +"' not found", !object);
            if(object)
            {
                this->setOutput("target", object, index);
            }
            ++index;
        }
    }
    else
    {
        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

        ::fwData::mt::ObjectWriteLock lock(composite);
        ::fwDataTools::helper::Composite compHelper(composite);

        for(ExtractMapType::value_type elt : m_extract)
        {
            std::string from = elt.first;
            std::string to   = elt.second;

            try
            {
                ::fwData::Object::sptr object = ::fwDataCamp::getObject( composite, from, true );

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
            catch(::fwDataCamp::exception::NullPointer& np)
            {
                if (composite->find(to) != composite->end())
                {
                    compHelper.remove(to);
                }

            }
            catch(::fwDataCamp::exception::ObjectNotFound& nf)
            {
                SLM_WARN("Object from '"+ from +"' not found");
            }
            catch(std::exception& e)
            {
                OSLM_FATAL("Unhandled exception: " << e.what());
            }
        }
        compHelper.notify();
    }
}
//-----------------------------------------------------------------------------

} // namespace ctrlCamp
