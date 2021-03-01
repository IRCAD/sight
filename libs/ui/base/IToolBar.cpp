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

#include "ui/base/IToolBar.hpp"

#include "ui/base/IAction.hpp"
#include "ui/base/IMenuItemCallback.hpp"

#include <core/thread/ActiveWorkers.hpp>
#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>
#include <core/tools/fwID.hpp>

#include <service/macros.hpp>
#include <service/op/Get.hpp>

namespace sight::ui::base
{

IToolBar::IToolBar() :
    m_hideActions(false)
{
}

//-----------------------------------------------------------------------------

IToolBar::~IToolBar()
{
}

//-----------------------------------------------------------------------------

void IToolBar::initialize()
{
    m_registry = ui::base::registry::ToolBar::New(this->getID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistry = m_configuration->find("registry");
    SIGHT_ASSERT("["+this->getID()+"'] <registry> section is mandatory.", !vectRegistry.empty() );

    if(!vectRegistry.empty())
    {
        m_registryConfig = vectRegistry.at(0);
        m_registry->initialize(m_registryConfig);
    }

    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    SIGHT_ASSERT("["+this->getID()+"'] <gui> section is mandatory.", !vectGui.empty() );

    if(!vectGui.empty())
    {
        // find LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        SIGHT_ASSERT("["+this->getID()+"'] <layout> section is mandatory.", !vectLayoutMng.empty() );

        if(!vectLayoutMng.empty())
        {
            m_layoutConfig = vectLayoutMng.at(0);
            this->initializeLayoutManager(m_layoutConfig);

            if (m_layoutConfig->hasAttribute("hideActions"))
            {
                std::string hideActions = m_layoutConfig->getAttributeValue("hideActions");
                SIGHT_ASSERT("["+this->getID()+"'] 'hideActions' attribute value must be 'true' or 'false'",
                             hideActions == "true" || hideActions == "false");
                m_hideActions = (hideActions == "true");
            }
        }
    }

}

//-----------------------------------------------------------------------------

void IToolBar::create()
{
    ui::base::container::fwToolBar::sptr toolBar               = m_registry->getParent();
    std::vector< ui::base::IMenuItemCallback::sptr > callbacks = m_registry->getCallbacks();

    SIGHT_ASSERT("Parent toolBar is unknown.", toolBar);
    m_layoutManager->setCallbacks(callbacks);

    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->createLayout(toolBar);
        })).wait();

    m_registry->manage(m_layoutManager->getMenuItems());
    m_registry->manage(m_layoutManager->getMenus());
    m_registry->manage(m_layoutManager->getContainers());
}

//-----------------------------------------------------------------------------

void IToolBar::destroy()
{
    m_registry->unmanage();

    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->destroyLayout();
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBar::actionServiceStopping(std::string actionSrvSID)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    if (m_hideActions)
    {
        core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetVisible(menuItem, false);
            })).wait();
    }
    else
    {
        core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetEnabled(menuItem, false);
            })).wait();
    }
}

//-----------------------------------------------------------------------------

void IToolBar::actionServiceStarting(std::string actionSrvSID)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    const service::IService::csptr service   = service::get( actionSrvSID );
    const ui::base::IAction::csptr actionSrv = ui::base::IAction::dynamicCast(service);

    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetEnabled(menuItem, actionSrv->getIsExecutable());
            const bool isInverted = actionSrv->isInverted();
            const bool isActive   = actionSrv->getIsActive();
            m_layoutManager->menuItemSetChecked(menuItem, isInverted ? !isActive : isActive);
            m_layoutManager->menuItemSetVisible(menuItem, actionSrv->isVisible());
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBar::actionServiceSetActive(std::string actionSrvSID, bool isActive)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    const service::IService::csptr service   = service::get( actionSrvSID );
    const ui::base::IAction::csptr actionSrv = ui::base::IAction::dynamicCast(service);

    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            const bool isInverted = actionSrv->isInverted();
            m_layoutManager->menuItemSetChecked(menuItem, isInverted ? !isActive : isActive);
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBar::actionServiceSetExecutable(std::string actionSrvSID, bool isExecutable)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetEnabled(menuItem, isExecutable);
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBar::actionServiceSetVisible(std::string actionSrvSID, bool isVisible)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(actionSrvSID,
                                                                               m_layoutManager->getMenuItems());

    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetVisible(menuItem, isVisible);
        })).wait();
}

//-----------------------------------------------------------------------------

void IToolBar::initializeLayoutManager(ConfigurationType layoutConfig)
{
    SIGHT_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
                 layoutConfig->getName() == "layout");

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
        ui::base::layoutManager::IToolBarLayoutManager::REGISTRY_KEY);
    m_layoutManager = ui::base::layoutManager::IToolBarLayoutManager::dynamicCast(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class "<< ui::base::layoutManager::IToolBarLayoutManager::REGISTRY_KEY,
            m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
