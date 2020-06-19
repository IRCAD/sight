/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ctrlCamp/SExtractObj.hpp"

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataCamp/exception/NullPointer.hpp>
#include <fwDataCamp/exception/ObjectNotFound.hpp>
#include <fwDataCamp/getObject.hpp>

#include <fwServices/registry/ObjectService.hpp>

namespace ctrlCamp
{

fwServicesRegisterMacro(::ctrlCamp::ICamp, ::ctrlCamp::SExtractObj);

static const std::string s_SOURCE_INOUT  = "source";
static const std::string s_TARGET_OUTPUT = "target";

//-----------------------------------------------------------------------------

SExtractObj::SExtractObj()
{
    this->registerObject(s_SOURCE_INOUT, AccessType::INOUT);
    this->registerObjectGroup(s_TARGET_OUTPUT, AccessType::OUTPUT, 0);
}

//-----------------------------------------------------------------------------

SExtractObj::~SExtractObj()
{
}

//-----------------------------------------------------------------------------

void SExtractObj::configuring()
{
    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    const ConfigurationType inoutCfg = m_configuration->findConfigurationElement("inout");
    SLM_ASSERT("At one 'inout' tag is required.", inoutCfg);

    const std::vector< ConfigurationType > extractCfg = inoutCfg->find("extract");
    SLM_ASSERT("At least one 'extract' tag is required.", !extractCfg.empty());

    for(ConfigurationType cfg : extractCfg)
    {
        SLM_ASSERT("Missing attribute 'from'.", cfg->hasAttribute("from"));
        const std::string from = cfg->getAttributeValue("from");

        m_sourcePaths.push_back(from);
    }
}

//-----------------------------------------------------------------------------

void SExtractObj::starting()
{
}

//-----------------------------------------------------------------------------

void SExtractObj::updating()
{
    auto sourceObject = this->getInOut< ::fwData::Object >(s_SOURCE_INOUT);

    size_t index = 0;
    for(auto path : m_sourcePaths)
    {
        const std::string from = path;

        ::fwData::Object::sptr object;
        try
        {
            object = ::fwDataCamp::getObject( sourceObject, from, true );
        }
        catch(::fwDataCamp::exception::NullPointer&)
        {
            this->setOutput("target", nullptr, index);
        }
        catch(::fwDataCamp::exception::ObjectNotFound&)
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
            this->setOutput(s_TARGET_OUTPUT, object, index);
        }
        ++index;
    }
}

//-----------------------------------------------------------------------------

void SExtractObj::stopping()
{
    // Unregister outputs
    for (size_t i = 0; i < this->getKeyGroupSize(s_TARGET_OUTPUT); ++i)
    {
        this->setOutput("target", nullptr, i);
    }
}

//-----------------------------------------------------------------------------

} // namespace ctrlCamp
