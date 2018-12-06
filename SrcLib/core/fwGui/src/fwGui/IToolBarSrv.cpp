/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwGui/IToolBarSrv.hpp"

#include "fwGui/IActionSrv.hpp"
#include "fwGui/IMenuItemCallback.hpp"

#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwThread/Worker.hpp>
#include <fwThread/Worker.hxx>

#include <fwTools/fwID.hpp>

namespace fwGui
{

IToolBarSrv::IToolBarSrv() :
    m_hideActions(false)
{
}

//-----------------------------------------------------------------------------

IToolBarSrv::~IToolBarSrv()
{
}

//-----------------------------------------------------------------------------

void IToolBarSrv::initialize()
{
    m_registrar = ::fwGui::registrar::ToolBarRegistrar::New(this->getID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistrar = m_configuration->find("registry");
    SLM_ASSERT("Registry section is mandatory.", !vectRegistrar.empty() );

    if(!vectRegistrar.empty())
    {
        m_registrarConfig = vectRegistrar.at(0);
        m_registrar->initialize(m_registrarConfig);
    }

    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    SLM_ASSERT("Gui section is mandatory.", !vectGui.empty() );

    if(!vectGui.empty())
    {
        // find LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        SLM_ASSERT("layout section is mandatory.", !vectLayoutMng.empty() );

        if(!vectLayoutMng.empty())
        {
            m_layoutConfig = vectLayoutMng.at(0);
            this->initializeLayoutManager(m_layoutConfig);

            if (m_layoutConfig->hasAttribute("hideAction"))
            {
                std::string hideActions = m_layoutConfig->getAttributeValue("hideActions");
                SLM_ASSERT("'hideActions' attribute value must be 'true' or 'false'",
                           hideActions == "true" || hideActions == "false");
                m_hideActions = (hideActions == "true");
            }
        }
    }
}

//-----------------------------------------------------------------------------

void IToolBarSrv::create()
{
    ::fwGui::container::fwToolBar::sptr toolBar = m_registrar->getParent();
    std::vector< ::fwGui::IMenuItemCallback::sptr > callbacks = m_registrar->getCallbacks();

    SLM_ASSERT("Parent toolBar is unknown.", toolBar);
    m_layoutManager->setCallbacks(callbacks);

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->createLayout(toolBar);
        })).wait();

    m_registrar->manage(m_layoutManager->getMenuItems());
    m_registrar->manage(m_layoutManager->getMenus());
    m_registrar->manage(m_layoutManager->getContainers());
}

//-----------------------------------------------------------------------------

void IToolBarSrv::destroy()
{
    m_registrar->unmanage();

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->destroyLayout();
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceStopping(std::string actionSrvSID)
{
    ::fwGui::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    if (m_hideActions)
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetVisible(menuItem, false);
            })).wait();
    }
    else
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetEnabled(menuItem, false);
            })).wait();
    }
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceStarting(std::string actionSrvSID)
{
    ::fwGui::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    if (m_hideActions)
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetVisible(menuItem, true);
            })).wait();
    }
    else
    {
        ::fwServices::IService::sptr service = ::fwServices::get( actionSrvSID );
        ::fwGui::IActionSrv::sptr actionSrv  = ::fwGui::IActionSrv::dynamicCast(service);

        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetEnabled(menuItem, actionSrv->getIsExecutable());
                m_layoutManager->menuItemSetChecked(menuItem, actionSrv->getIsActive());
                m_layoutManager->menuItemSetVisible(menuItem, actionSrv->isVisible());
            })).wait();
    }
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceSetActive(std::string actionSrvSID, bool isActive)
{
    ::fwGui::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetChecked(menuItem, isActive);
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceSetExecutable(std::string actionSrvSID, bool isExecutable)
{
    ::fwGui::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetEnabled(menuItem, isExecutable);
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceSetVisible(std::string actionSrvSID, bool isVisible)
{
    ::fwGui::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetVisible(menuItem, isVisible);
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::initializeLayoutManager(ConfigurationType layoutConfig)
{
    OSLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
                layoutConfig->getName() == "layout");

    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(
        ::fwGui::layoutManager::IToolBarLayoutManager::REGISTRY_KEY);
    m_layoutManager = ::fwGui::layoutManager::IToolBarLayoutManager::dynamicCast(guiObj);
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::layoutManager::IToolBarLayoutManager::REGISTRY_KEY,
                m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
