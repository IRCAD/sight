/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/base/IToolBarSrv.hpp"

#include "ui/base/IActionSrv.hpp"
#include "ui/base/IMenuItemCallback.hpp"

#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>
#include <core/tools/fwID.hpp>

#include <services/macros.hpp>
#include <services/op/Get.hpp>
#include <services/registry/ActiveWorkers.hpp>

namespace sight::ui::base
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
    m_registrar = ui::base::registrar::ToolBarRegistrar::New(this->getID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistrar = m_configuration->find("registry");
    SLM_ASSERT("["+this->getID()+"'] <registry> section is mandatory.", !vectRegistrar.empty() );

    if(!vectRegistrar.empty())
    {
        m_registrarConfig = vectRegistrar.at(0);
        m_registrar->initialize(m_registrarConfig);
    }

    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    SLM_ASSERT("["+this->getID()+"'] <gui> section is mandatory.", !vectGui.empty() );

    if(!vectGui.empty())
    {
        // find LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        SLM_ASSERT("["+this->getID()+"'] <layout> section is mandatory.", !vectLayoutMng.empty() );

        if(!vectLayoutMng.empty())
        {
            m_layoutConfig = vectLayoutMng.at(0);
            this->initializeLayoutManager(m_layoutConfig);

            if (m_layoutConfig->hasAttribute("hideActions"))
            {
                std::string hideActions = m_layoutConfig->getAttributeValue("hideActions");
                SLM_ASSERT("["+this->getID()+"'] 'hideActions' attribute value must be 'true' or 'false'",
                           hideActions == "true" || hideActions == "false");
                m_hideActions = (hideActions == "true");
            }
        }
    }

}

//-----------------------------------------------------------------------------

void IToolBarSrv::create()
{
    ui::base::container::fwToolBar::sptr toolBar               = m_registrar->getParent();
    std::vector< ui::base::IMenuItemCallback::sptr > callbacks = m_registrar->getCallbacks();

    SLM_ASSERT("Parent toolBar is unknown.", toolBar);
    m_layoutManager->setCallbacks(callbacks);

    services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
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

    services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->destroyLayout();
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceStopping(std::string actionSrvSID)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    if (m_hideActions)
    {
        services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetVisible(menuItem, false);
            })).wait();
    }
    else
    {
        services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetEnabled(menuItem, false);
            })).wait();
    }
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceStarting(std::string actionSrvSID)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    const services::IService::csptr service     = services::get( actionSrvSID );
    const ui::base::IActionSrv::csptr actionSrv = ui::base::IActionSrv::dynamicCast(service);

    services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetEnabled(menuItem, actionSrv->getIsExecutable());
            const bool isInverted = actionSrv->isInverted();
            const bool isActive   = actionSrv->getIsActive();
            m_layoutManager->menuItemSetChecked(menuItem, isInverted ? !isActive : isActive);
            m_layoutManager->menuItemSetVisible(menuItem, actionSrv->isVisible());
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceSetActive(std::string actionSrvSID, bool isActive)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    const services::IService::csptr service     = services::get( actionSrvSID );
    const ui::base::IActionSrv::csptr actionSrv = ui::base::IActionSrv::dynamicCast(service);

    services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            const bool isInverted = actionSrv->isInverted();
            m_layoutManager->menuItemSetChecked(menuItem, isInverted ? !isActive : isActive);
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceSetExecutable(std::string actionSrvSID, bool isExecutable)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetEnabled(menuItem, isExecutable);
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::actionServiceSetVisible(std::string actionSrvSID, bool isVisible)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    services::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetVisible(menuItem, isVisible);
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBarSrv::initializeLayoutManager(ConfigurationType layoutConfig)
{
    SLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
               layoutConfig->getName() == "layout");

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
        ui::base::layoutManager::IToolBarLayoutManager::REGISTRY_KEY);
    m_layoutManager = ui::base::layoutManager::IToolBarLayoutManager::dynamicCast(guiObj);
    SLM_ASSERT("ClassFactoryRegistry failed for class "<< ui::base::layoutManager::IToolBarLayoutManager::REGISTRY_KEY,
               m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
