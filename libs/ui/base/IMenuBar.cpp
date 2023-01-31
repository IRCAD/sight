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

#include "ui/base/IMenuBar.hpp"

#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>

namespace sight::ui::base
{

//-----------------------------------------------------------------------------

void IMenuBar::initialize()
{
    m_registry = ui::base::registry::MenuBar::New(this->getID());

    const auto config = this->getConfiguration();

    // find ViewRegistryManager configuration
    if(const auto registry = config.get_child_optional("registry"); registry.has_value())
    {
        m_registry->initialize(registry.value());
    }

    // find LayoutManager configuration
    if(const auto layoutConfig = config.get_child_optional("gui.layout"); layoutConfig.has_value())
    {
        this->initializeLayoutManager(layoutConfig.value());
    }
}

//-----------------------------------------------------------------------------

void IMenuBar::create()
{
    ui::base::container::fwMenuBar::sptr menuBar = m_registry->getParent();
    SIGHT_ASSERT("Parent menuBar is unknown.", menuBar);

    const std::string serviceID = getID().substr(getID().find_last_of('_') + 1);

    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->createLayout(menuBar, serviceID);
        })
    ).wait();

    m_registry->manage(m_layoutManager->getMenus());
}

//-----------------------------------------------------------------------------

void IMenuBar::destroy()
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

void IMenuBar::menuServiceStopping(std::string menuSrvSID)
{
    ui::base::container::fwMenu::sptr menu = m_registry->getFwMenu(menuSrvSID, m_layoutManager->getMenus());

    if(m_hideMenus)
    {
        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuIsVisible(menu, false);
            })
        ).wait();
    }
    else
    {
        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuIsEnabled(menu, false);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenuBar::menuServiceStarting(std::string menuSrvSID)
{
    ui::base::container::fwMenu::sptr menu = m_registry->getFwMenu(menuSrvSID, m_layoutManager->getMenus());

    if(m_hideMenus)
    {
        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuIsVisible(menu, true);
            })
        ).wait();
    }
    else
    {
        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuIsEnabled(menu, true);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenuBar::initializeLayoutManager(const ui::base::config_t& layoutConfig)
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
        ui::base::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY
    );
    m_layoutManager = ui::base::layoutManager::IMenuBarLayoutManager::dynamicCast(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::base::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY,
        m_layoutManager
    );

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base
