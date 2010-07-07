/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IMenuSrv.hpp"

namespace fwGui
{

IMenuSrv::IMenuSrv()
{}

//-----------------------------------------------------------------------------

IMenuSrv::~IMenuSrv()
{}

//-----------------------------------------------------------------------------

void IMenuSrv::initialize()
{
    m_registrar = ::fwGui::registrar::MenuRegistrar::NewSptr(this->getUUID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistrar = m_configuration->find("registrar");
    if(!vectRegistrar.empty())
    {
        m_registrarConfig = vectRegistrar.at(0);
    }
    m_registrar->initialize(m_registrarConfig);


    // find LayoutManager configuration
    std::vector < ConfigurationType > vectLayoutMng = m_configuration->find("layout");
    if(!vectLayoutMng.empty())
    {
        m_layoutConfig = vectLayoutMng.at(0);
    }
    this->initializeLayoutManager(m_layoutConfig);

    ::fwGui::fwMenu::sptr menu = m_registrar->getParent();
    SLM_ASSERT("Parent menu is unknown.", menu);
    m_layoutManager->createLayout(menu);


    m_registrar->manage(m_layoutManager->getMenuItems());
}

//-----------------------------------------------------------------------------

void IMenuSrv::actionServiceStopping(std::string actionSrvSID)
{
    ::fwGui::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID, m_layoutManager->getMenuItems());

    if (m_hideActions)
    {
        m_layoutManager->actionIsVisible(menuItem, false);
    }
    else
    {
        m_layoutManager->actionIsEnabled(menuItem, false);
    }
}

//-----------------------------------------------------------------------------

void IMenuSrv::actionServiceStarting(std::string actionSrvSID)
{
    ::fwGui::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID, m_layoutManager->getMenuItems());

    if (m_hideActions)
    {
        m_layoutManager->actionIsVisible(menuItem, true);
    }
    else
    {
        m_layoutManager->actionIsEnabled(menuItem, true);
    }
}

//-----------------------------------------------------------------------------

void IMenuSrv::initializeLayoutManager(ConfigurationType layoutConfig)
{
    OSLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
            layoutConfig->getName() == "layout");
    SLM_ASSERT("<layout> tag must have type attribute", layoutConfig->hasAttribute("type"));
    std::string layoutManagerClassName = layoutConfig->getAttributeValue("type");
    ::fwTools::Object::sptr layout = ::fwTools::Factory::New(layoutManagerClassName);
    OSLM_ASSERT("Unable to create "<< layoutManagerClassName, layout);
    m_layoutManager = ::fwGui::layoutManager::IMenuLayoutManager::dynamicCast(layout);
    OSLM_ASSERT("Unable to cast "<< layoutManagerClassName << " in layout manager", m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
