/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/**
 * @file fwGui/IToolBarLayoutManager.cpp
 * @brief This file defines the implementation of the base class for managing a toolbar.
 *
 * @author IRCAD (Research and Development Team).
 * @date 2009-2010
 */

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>


#include "fwGui/layoutManager/IToolBarLayoutManager.hpp"

namespace fwGui
{
namespace layoutManager
{

const IToolBarLayoutManager::RegistryKeyType IToolBarLayoutManager::REGISTRY_KEY = "::fwGui::ToolBarLayoutManager";

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
        if( (*iter)->getName() == "menuItem" )
        {
            ConfigurationType toolBarItem = *iter;
            ActionInfo info;
            SLM_ASSERT("Depreciated tag <state>", ! toolBarItem->hasAttribute("state"));
            SLM_ASSERT("Depreciated tag <enable>", ! toolBarItem->hasAttribute("enable"));

            SLM_ASSERT("missing <name> attribute", toolBarItem->hasAttribute("name"));
            if( toolBarItem->hasAttribute("name") )
            {
                info.m_name = toolBarItem->getExistingAttributeValue("name") ;
            }

            SLM_ASSERT("missing <icon> attribute", toolBarItem->hasAttribute("icon"));
            if( toolBarItem->hasAttribute("icon") )
            {
                info.m_icon = toolBarItem->getExistingAttributeValue("icon") ;
            }

            if( toolBarItem->hasAttribute("style") )
            {
                std::string style = toolBarItem->getExistingAttributeValue("style") ;
                info.m_isCheckable = (style == "check");
                info.m_isRadio = (style == "radio");
            }

            m_actionInfo.push_back(info);
        }
        if( (*iter)->getName() == "separator" )
        {
            ActionInfo info;
            info.m_isSeparator = true;

            if( (*iter)->hasAttribute("size") )
            {
                info.m_size = ::boost::lexical_cast< int > ((*iter)->getExistingAttributeValue("size")) ;
            }

            m_actionInfo.push_back( info ) ;
        }
        if( (*iter)->getName() == "spacer" )
        {
            ActionInfo info;
            info.m_isSpacer = true;
            m_actionInfo.push_back( info ) ;
        }
        if( (*iter)->getName() == "menu" )
        {
            ActionInfo info;
            info.m_isMenu = true;
            if( (*iter)->hasAttribute("name") )
            {
                info.m_name = (*iter)->getExistingAttributeValue("name") ;
            }

//            SLM_ASSERT("missing <icon> attribute", (*iter)->hasAttribute("icon"));
            if( (*iter)->hasAttribute("icon") )
            {
                info.m_icon = (*iter)->getExistingAttributeValue("icon") ;
            }
            m_actionInfo.push_back( info ) ;
        }

    }
}

//-----------------------------------------------------------------------------

void IToolBarLayoutManager::destroyActions()
{
    BOOST_FOREACH( ::fwGui::container::fwMenuItem::sptr menuItem, m_menuItems)
    {
        menuItem->destroyContainer();
    }
    m_menuItems.clear();
    BOOST_FOREACH( ::fwGui::container::fwMenu::sptr menu, m_menus)
    {
        menu->destroyContainer();
    }
    m_menus.clear();
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::container::fwMenuItem::sptr > IToolBarLayoutManager::getMenuItems()
{
    return this->m_menuItems;
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::container::fwMenu::sptr > IToolBarLayoutManager::getMenus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



