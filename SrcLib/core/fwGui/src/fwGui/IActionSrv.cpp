/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/fwID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IActionSrv.hpp"

namespace fwGui
{

IActionSrv::IActionSrv() :
        m_isActive(false),
        m_isExecutable(true)
{}

//-----------------------------------------------------------------------------

IActionSrv::~IActionSrv()
{}

//-----------------------------------------------------------------------------

void IActionSrv::initialize()
{
    m_registrar = ::fwGui::registrar::ActionRegistrar::NewSptr(this->getID());

    OSLM_ASSERT("Depreciated tag <name> in "<< this->getID() << " configuration.", ! m_configuration->hasAttribute("name"));
    OSLM_ASSERT("Depreciated tag <shortcut> in "<< this->getID() << " configuration.", ! m_configuration->hasAttribute("shortcut"));
    OSLM_ASSERT("Depreciated tag <enable> in "<< this->getID() << " configuration.", ! m_configuration->hasAttribute("enable"));
    OSLM_ASSERT("Depreciated tag <specialAction> in "<< this->getID() << " configuration.", ! m_configuration->hasAttribute("specialAction"));
    OSLM_ASSERT("Depreciated tag <style> in "<< this->getID() << " configuration.", ! m_configuration->hasAttribute("style"));
    OSLM_ASSERT("Depreciated tag <state> in "<< this->getID() << " configuration.", ! m_configuration->hasAttribute("state"));

    ::fwRuntime::ConfigurationElementContainer::Iterator iter ;
    for( iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter )
    {
        if( (*iter)->getName() == "state" )
        {
            ConfigurationType stateCfg = *iter;

            if( stateCfg->hasAttribute("active") )
            {
                std::string isActive = stateCfg->getExistingAttributeValue("active");
                SLM_ASSERT("Wrong attribute value : must be 'true' or 'false'", (isActive == "true") || (isActive == "false"));
                m_isActive = (isActive == "true") ;
            }

            if( stateCfg->hasAttribute("executable") )
            {
                std::string isExecutable = stateCfg->getExistingAttributeValue("executable");
                SLM_ASSERT("Wrong attribute value : must be 'true' or 'false'", (isExecutable == "true") || (isExecutable == "false"));
                m_isExecutable = (isExecutable == "true") ;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IActionSrv::actionServiceStopping()
{
    this->m_registrar->actionServiceStopping();
}

//-----------------------------------------------------------------------------

void IActionSrv::actionServiceStarting()
{
    this->m_registrar->actionServiceStarting();
    this->m_registrar->actionServiceSetActive(m_isActive);
}

//-----------------------------------------------------------------------------

void IActionSrv::setIsActive(bool isActive)
{
    m_isActive = isActive;
    this->m_registrar->actionServiceSetActive(isActive);
}

//-----------------------------------------------------------------------------

bool IActionSrv::getIsActive()
{
    return m_isActive;
}

//-----------------------------------------------------------------------------

void IActionSrv::setIsExecutable(bool isExecutable)
{
    m_isExecutable = isExecutable;
    this->m_registrar->actionServiceSetExecutable(isExecutable);
}

//-----------------------------------------------------------------------------

bool IActionSrv::getIsExecutable()
{
    return m_isExecutable;
}

//-----------------------------------------------------------------------------

}
