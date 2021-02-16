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

#include "ui/base/IMenu.hpp"

#include "ui/base/IAction.hpp"
#include "ui/base/IMenuItemCallback.hpp"

#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>
#include <core/tools/fwID.hpp>

#include <service/macros.hpp>
#include <service/op/Get.hpp>
#include <service/registry/ActiveWorkers.hpp>

namespace sight::ui::base
{

IMenu::IMenu() :
    m_hideActions(false)
{
}

//-----------------------------------------------------------------------------

IMenu::~IMenu()
{
}

//-----------------------------------------------------------------------------

void IMenu::initialize()
{
    m_registrar = ui::base::registrar::MenuRegistrar::New(this->getID());
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

            if (m_layoutConfig->hasAttribute("hideAction"))
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

void IMenu::create()
{
    ui::base::container::fwMenu::sptr menu                     = m_registrar->getParent();
    std::vector< ui::base::IMenuItemCallback::sptr > callbacks = m_registrar->getCallbacks();

    SLM_ASSERT("Parent menu is unknown.", menu);
    m_layoutManager->setCallbacks(callbacks);

    service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->createLayout(menu);
        })).wait();

    m_registrar->manage(m_layoutManager->getMenuItems());
    m_registrar->manage(m_layoutManager->getMenus());
}

//-----------------------------------------------------------------------------

void IMenu::destroy()
{
    m_registrar->unmanage();
    service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->destroyLayout();
        })).wait();
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceStopping(std::string actionSrvSID)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    if (m_hideActions)
    {
        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetVisible(menuItem, false);
            })).wait();
    }
    else
    {
        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetEnabled(menuItem, false);
            })).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceStarting(std::string actionSrvSID)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    if (m_hideActions)
    {
        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetVisible(menuItem, true);
            })).wait();
    }
    else
    {
        const service::IService::csptr service      = service::get( actionSrvSID );
        const ui::base::IAction::csptr actionSrv = ui::base::IAction::dynamicCast(service);

        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuItemSetEnabled(menuItem, actionSrv->getIsExecutable());
                const bool isInverted = actionSrv->isInverted();
                const bool isActive   = actionSrv->getIsActive();
                m_layoutManager->menuItemSetChecked(menuItem, isInverted ? !isActive : isActive);
                m_layoutManager->menuItemSetVisible(menuItem, actionSrv->isVisible());
            })).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceSetActive(std::string actionSrvSID, bool isActive)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    const service::IService::csptr service      = service::get( actionSrvSID );
    const ui::base::IAction::csptr actionSrv = ui::base::IAction::dynamicCast(service);

    service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >( [&]
        {
            const bool isInverted = actionSrv->isInverted();
            m_layoutManager->menuItemSetChecked(menuItem, isInverted ? !isActive : isActive);
        })).wait();
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceSetExecutable(std::string actionSrvSID, bool isExecutable)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetEnabled(menuItem, isExecutable);
        })).wait();
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceSetVisible(std::string actionSrvSID, bool isVisible)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registrar->getFwMenuItem(actionSrvSID,
                                                                                m_layoutManager->getMenuItems());

    service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->menuItemSetVisible(menuItem, isVisible);
        })).wait();
}

//-----------------------------------------------------------------------------

void IMenu::initializeLayoutManager(ConfigurationType layoutConfig)
{
    SLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
               layoutConfig->getName() == "layout");

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
        ui::base::layoutManager::IMenuLayoutManager::REGISTRY_KEY);
    m_layoutManager = ui::base::layoutManager::IMenuLayoutManager::dynamicCast(guiObj);
    SLM_ASSERT("ClassFactoryRegistry failed for class "<< ui::base::layoutManager::IMenuLayoutManager::REGISTRY_KEY,
               m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
