/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwGui/layoutManager/IMenuLayoutManager.hpp"

namespace fwGui
{
namespace layoutManager
{

const IMenuLayoutManager::RegistryKeyType IMenuLayoutManager::REGISTRAR_KEY = "::fwGui::MenuBarLayoutManager";

//-----------------------------------------------------------------------------

IMenuLayoutManager::IMenuLayoutManager()
{}

//-----------------------------------------------------------------------------

IMenuLayoutManager::~IMenuLayoutManager()
{}

//-----------------------------------------------------------------------------

void IMenuLayoutManager::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
                configuration->getName() == "layout");

    std::vector < ConfigurationType > vectMenuItems = configuration->find("menuItem");
    SLM_TRACE_IF("No menu define.", vectMenuItems.empty() );
    m_actionInfo.clear();
    BOOST_FOREACH (ConfigurationType menuItem, vectMenuItems)
    {
        ActionInfo info;
        SLM_ASSERT("missing <name> attribute", menuItem->hasAttribute("name"));
        if( menuItem->hasAttribute("name") )
        {
            info.m_name = menuItem->getExistingAttributeValue("name") ;
        }

        if( menuItem->hasAttribute("shortcut") )
        {
            info.m_shortcut = menuItem->getExistingAttributeValue("shortcut") ;
        }

        if( menuItem->hasAttribute("style") )
        {
            std::string style = menuItem->getExistingAttributeValue("style") ;
            info.m_isCheckable = (style == "check");
            info.m_isRadio = (style == "radio");

            if ((info.m_isCheckable || info.m_isRadio) && menuItem->hasAttribute("state") )
            {
                std::string state = menuItem->getExistingAttributeValue("state");
                info.m_isChecked = (state == "checked");
            }
        }

        if( menuItem->hasAttribute("enable") )
        {
            std::string enable = menuItem->getExistingAttributeValue("enable") ;
            OSLM_TRACE("enable : " << enable ) ;
            info.m_isEnabled = (enable =="true");
        }

        if( menuItem->hasAttribute("specialAction") )
        {
            std::string specialActionName = menuItem->getExistingAttributeValue("specialAction") ;
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
}

//-----------------------------------------------------------------------------

void IMenuLayoutManager::destroyActions()
{
    BOOST_FOREACH( ::fwGui::fwMenuItem::sptr menuItem, m_menuItems)
    {
        menuItem->destroyContainer();
    }
    m_menuItems.clear();
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::fwMenuItem::sptr > IMenuLayoutManager::getMenuItems()
{
    return this->m_menuItems;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



