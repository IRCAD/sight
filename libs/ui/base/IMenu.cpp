/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/thread/Worker.hxx>

#include <service/op/Get.hpp>

namespace sight::ui::base
{

IMenu::IMenu()
= default;

//-----------------------------------------------------------------------------

IMenu::~IMenu()
= default;

//-----------------------------------------------------------------------------

void IMenu::initialize()
{
    m_registry = ui::base::registry::Menu::New(this->getID());

    const auto& config = this->getConfigTree();

    // find ViewRegistryManager configuration
    if(const auto registryConfig = config.get_child_optional("registry"); registryConfig.has_value())
    {
        m_registry->initialize(registryConfig.value());
    }

    // find LayoutManager configuration
    if(const auto layoutConfig = config.get_child_optional("gui.layout"); layoutConfig.has_value())
    {
        this->initializeLayoutManager(layoutConfig.value());

        m_hideActions = layoutConfig->get<bool>("hideActions", m_hideActions);
    }
}

//-----------------------------------------------------------------------------

void IMenu::create()
{
    ui::base::container::fwMenu::sptr menu                   = m_registry->getParent();
    std::vector<ui::base::IMenuItemCallback::sptr> callbacks = m_registry->getCallbacks();

    SIGHT_ASSERT("Parent menu is unknown.", menu);
    m_layoutManager->setCallbacks(callbacks);

    const std::string serviceID = getID().substr(getID().find_last_of('_') + 1);

    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->createLayout(menu, serviceID);
        })
    ).wait();

    m_registry->manage(m_layoutManager->getMenuItems());
    m_registry->manage(m_layoutManager->getMenus());
}

//-----------------------------------------------------------------------------

void IMenu::destroy()
{
    m_registry->unmanage();
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->destroyLayout();
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceStopping(std::string actionSrvSID)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    if(m_hideActions)
    {
        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetVisible(menuItem, false);
            })
        ).wait();
    }
    else
    {
        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetEnabled(menuItem, false);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceStarting(std::string actionSrvSID)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    if(m_hideActions)
    {
        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetVisible(menuItem, true);
            })
        ).wait();
    }
    else
    {
        const service::IService::csptr service   = service::get(actionSrvSID);
        const ui::base::IAction::csptr actionSrv = ui::base::IAction::dynamicCast(service);

        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetEnabled(menuItem, actionSrv->enabled());
                const bool inverted  = actionSrv->inverted();
                const bool isChecked = actionSrv->checked();
                m_layoutManager->menuItemSetChecked(menuItem, inverted ? !isChecked : isChecked);
                m_layoutManager->menuItemSetVisible(menuItem, actionSrv->visible());
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceSetChecked(std::string actionSrvSID, bool isChecked)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    const service::IService::csptr service   = service::get(actionSrvSID);
    const ui::base::IAction::csptr actionSrv = ui::base::IAction::dynamicCast(service);

    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            const bool inverted = actionSrv->inverted();
            m_layoutManager->menuItemSetChecked(menuItem, inverted ? !isChecked : isChecked);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceSetEnabled(std::string actionSrvSID, bool isEnabled)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->menuItemSetEnabled(menuItem, isEnabled);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void IMenu::actionServiceSetVisible(std::string actionSrvSID, bool isVisible)
{
    ui::base::container::fwMenuItem::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->menuItemSetVisible(menuItem, isVisible);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void IMenu::initializeLayoutManager(const ui::base::config_t& layoutConfig)
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
        ui::base::layoutManager::IMenuLayoutManager::REGISTRY_KEY
    );
    m_layoutManager = ui::base::layoutManager::IMenuLayoutManager::dynamicCast(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::base::layoutManager::IMenuLayoutManager::REGISTRY_KEY,
        m_layoutManager
    );

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base
