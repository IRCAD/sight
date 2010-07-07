/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwGui/layoutManager/IMenuBarLayoutManager.hpp"

namespace fwGui
{
namespace layoutManager
{

const IMenuBarLayoutManager::RegistryKeyType IMenuBarLayoutManager::REGISTRAR_KEY = "::fwGui::MenuBarLayoutManager";

//-----------------------------------------------------------------------------

IMenuBarLayoutManager::IMenuBarLayoutManager()
{}

//-----------------------------------------------------------------------------

IMenuBarLayoutManager::~IMenuBarLayoutManager()
{}

//-----------------------------------------------------------------------------

void IMenuBarLayoutManager::initialize( ConfigurationType configuration)
{
    OSLM_ASSERT("Bad configuration name "<<configuration->getName()<< ", must be layout",
                configuration->getName() == "layout");

    std::vector < ConfigurationType > vectMenus = configuration->find("menu");
    SLM_TRACE_IF("No menu define.", vectMenus.empty() );
    m_menus.clear();
    BOOST_FOREACH (ConfigurationType menu, vectMenus)
    {
        SLM_ASSERT("missing <name> attribute", menu->hasAttribute("name"));
        if( menu->hasAttribute("name") )
        {
            std::string name = menu->getExistingAttributeValue("name") ;
            m_menuNames.push_back(name);
        }
    }
}

//-----------------------------------------------------------------------------

void IMenuBarLayoutManager::destroyMenus()
{
    BOOST_FOREACH( ::fwGui::fwMenu::sptr menu, m_menus)
    {
        menu->destroyContainer();
    }
    m_menus.clear();
}

//-----------------------------------------------------------------------------

std::vector< ::fwGui::fwMenu::sptr > IMenuBarLayoutManager::getMenus()
{
    return this->m_menus;
}

//-----------------------------------------------------------------------------

} // namespace layoutManager
} // namespace fwGui



