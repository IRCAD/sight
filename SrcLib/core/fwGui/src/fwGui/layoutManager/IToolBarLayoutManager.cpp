/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwGui/layoutManager/IToolBarLayoutManager.hpp"

namespace fwGui
{
namespace layoutManager
{

const IToolBarLayoutManager::RegistryKeyType IToolBarLayoutManager::REGISTRY_KEY = "::fwGui::ToolBarBarLayoutManager";

//-----------------------------------------------------------------------------

IToolBarLayoutManager::IToolBarLayoutManager()
{}

//-----------------------------------------------------------------------------

IToolBarLayoutManager::~IToolBarLayoutManager()
{}

//-----------------------------------------------------------------------------

void IToolBarLayoutManager::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
                configuration->getName() == "layout");


    ::fwRuntime::ConfigurationElementContainer::Iterator iter ;
    for( iter = configuration->begin() ; iter != configuration->end() ; ++iter )
    {
        if( (*iter)->getName() == "toolBarItem" )
        {
            ConfigurationType toolBarItem = *iter;
            ActionInfo info;
            SLM_ASSERT("missing <name> attribute", toolBarItem->hasAttribute("name"));
            if( toolBarItem->hasAttribute("name") )
            {
                info.m_name = toolBarItem->getExistingAttributeValue("name") ;
            }

            if( toolBarItem->hasAttribute("shortcut") )
            {
                info.m_shortcut = toolBarItem->getExistingAttributeValue("shortcut") ;
            }

            if( toolBarItem->hasAttribute("style") )
            {
                std::string style = toolBarItem->getExistingAttributeValue("style") ;
                info.m_isCheckable = (style == "check");
                info.m_isRadio = (style == "radio");

                if ((info.m_isCheckable || info.m_isRadio) && toolBarItem->hasAttribute("state") )
                {
                    std::string state = toolBarItem->getExistingAttributeValue("state");
                    info.m_isChecked = (state == "checked");
                }
            }

            if( toolBarItem->hasAttribute("enable") )
            {
                std::string enable = toolBarItem->getExistingAttributeValue("enable") ;
                OSLM_TRACE("enable : " << enable ) ;
                info.m_isEnabled = (enable =="true");
            }

            if( toolBarItem->hasAttribute("specialAction") )
            {
                std::string specialActionName = toolBarItem->getExistingAttributeValue("specialAction") ;
                if (specialActionName == "DEFAULT")
                {
                    info.m_type = DEFAULT;
                }
                else if (specialActionName == "QUIT")
                {
                    info.m_type = QUIT;
                }
                else if (specialActionName == "ABOUT")
                {
                    info.m_type = ABOUT;
                }
                else if (specialActionName == "HELP")
                {
                    info.m_type = HELP;
                }
                else if (specialActionName == "NEW")
                {
                    info.m_type = NEW;
                }
                else
                {
                    OSLM_FATAL("specialAction " << specialActionName << " is unknown." );
                }
            }

            m_actionInfo.push_back(info);
        }
        if( (*iter)->getName() == "separator" )
        {
            ActionInfo info;
            info.m_isSeparator = true;
            info.m_type = SEPARATOR;
            m_actionInfo.push_back( info ) ;
        }
    }
}

//-----------------------------------------------------------------------------

void IToolBarLayoutManager::destroyActions()
{
    BOOST_FOREACH( ::fwGui::fwMenuItem::sptr menuItem, m_menuItems)
    {
        menuItem->destroyContainer();
    }
    m_menuItems.clear();
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::fwMenuItem::sptr > IToolBarLayoutManager::getMenuItems()
{
    return this->m_menuItems;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



