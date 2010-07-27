/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IMenuItemCallback.hpp"
#include "fwGui/IToolBarSrv.hpp"

namespace fwGui
{

IToolBarSrv::IToolBarSrv() : m_hideActions (false)
{}

//-----------------------------------------------------------------------------

IToolBarSrv::~IToolBarSrv()
{}

//-----------------------------------------------------------------------------

void IToolBarSrv::initialize()
{
    m_registrar = ::fwGui::registrar::ToolBarRegistrar::NewSptr(this->getUUID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistrar = m_configuration->find("registry");
    if(!vectRegistrar.empty())
    {
        m_registrarConfig = vectRegistrar.at(0);
        m_registrar->initialize(m_registrarConfig);
    }

    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    if(!vectGui.empty())
    {
        // find LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        if(!vectLayoutMng.empty())
        {
            m_layoutConfig = vectLayoutMng.at(0);
            this->initializeLayoutManager(m_layoutConfig);
        }
    }
}

//-----------------------------------------------------------------------------

void IToolBarSrv::create()
{
    ::fwGui::fwToolBar::sptr toolBar = m_registrar->getParent();
    std::vector< ::fwGui::IMenuItemCallback::sptr > callbacks = m_registrar->getCallbacks();

    SLM_ASSERT("Parent toolBar is unknown.", toolBar);
    m_layoutManager->setCallbacks(callbacks);
    m_layoutManager->createLayout(toolBar);

    m_registrar->manage(m_layoutManager->getMenuItems());
}

//-----------------------------------------------------------------------------

void IToolBarSrv::destroy()
{
    m_registrar->unmanage();
    m_layoutManager->destroyLayout();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceStopping(std::string actionSrvSID)
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

void IToolBarSrv::actionServiceStarting(std::string actionSrvSID)
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

void IToolBarSrv::actionServiceChecked(std::string actionSrvSID, bool checked)
{
    ::fwGui::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID, m_layoutManager->getMenuItems());

    m_layoutManager->actionIsChecked(menuItem, checked);

}

//-----------------------------------------------------------------------------

void IToolBarSrv::initializeLayoutManager(ConfigurationType layoutConfig)
{
    OSLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
            layoutConfig->getName() == "layout");

    m_layoutManager = ::fwTools::ClassFactoryRegistry::create< ::fwGui::layoutManager::IToolBarLayoutManager >( ::fwGui::layoutManager::IToolBarLayoutManager::REGISTRY_KEY );
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::layoutManager::IToolBarLayoutManager::REGISTRY_KEY, m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
