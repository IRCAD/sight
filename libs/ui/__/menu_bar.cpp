/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/detail/registry/menu_bar.hpp"
#include "ui/__/menu_bar.hpp"

#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

namespace sight::ui
{

//-----------------------------------------------------------------------------

void menu_bar::initialize()
{
    m_registry = ui::detail::registry::menu_bar::make(this->get_id());

    const auto config = this->get_config();

    // find ViewRegistryManager configuration
    if(const auto registry = config.get_child_optional("registry"); registry.has_value())
    {
        m_registry->initialize(registry.value());
    }

    // find layout configuration
    if(const auto layout_config = config.get_child_optional("gui.layout"); layout_config.has_value())
    {
        this->initializeLayoutManager(layout_config.value());
    }
}

//-----------------------------------------------------------------------------

void menu_bar::create()
{
    ui::container::menubar::sptr menu_bar = m_registry->getParent();
    SIGHT_ASSERT("Parent menuBar is unknown.", menu_bar);

    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->createLayout(menu_bar, service_id);
        })
    ).wait();

    m_registry->manage(m_layoutManager->getMenus());
}

//-----------------------------------------------------------------------------

void menu_bar::destroy()
{
    m_registry->unmanage();

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->destroyLayout();
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void menu_bar::menuServiceStopping(std::string _menu_srv_sid)
{
    ui::container::menu::sptr menu = m_registry->getFwMenu(_menu_srv_sid, m_layoutManager->getMenus());

    if(m_hideMenus)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuIsVisible(menu, false);
            })
        ).wait();
    }
    else
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuIsEnabled(menu, false);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void menu_bar::menuServiceStarting(std::string _menu_srv_sid)
{
    ui::container::menu::sptr menu = m_registry->getFwMenu(_menu_srv_sid, m_layoutManager->getMenus());

    if(m_hideMenus)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuIsVisible(menu, true);
            })
        ).wait();
    }
    else
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuIsEnabled(menu, true);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void menu_bar::initializeLayoutManager(const ui::config_t& _layout_config)
{
    ui::object::sptr gui_obj = ui::factory::make(
        ui::layout::menubar_manager::REGISTRY_KEY
    );
    m_layoutManager = std::dynamic_pointer_cast<ui::layout::menubar_manager>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::layout::menubar_manager::REGISTRY_KEY,
        m_layoutManager
    );

    m_layoutManager->initialize(_layout_config);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
