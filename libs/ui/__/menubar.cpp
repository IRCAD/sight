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

#include "ui/__/detail/registry/menubar.hpp"
#include "ui/__/menubar.hpp"

#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

namespace sight::ui
{

//-----------------------------------------------------------------------------

void menubar::initialize()
{
    m_registry = ui::detail::registry::menubar::make(this->get_id());

    const auto config = this->get_config();

    // find ViewRegistryManager configuration
    if(const auto registry = config.get_child_optional("registry"); registry.has_value())
    {
        m_registry->initialize(registry.value());
    }

    // find layout configuration
    if(const auto layout_config = config.get_child_optional("gui.layout"); layout_config.has_value())
    {
        this->initialize_layout_manager(layout_config.value());
    }
}

//-----------------------------------------------------------------------------

void menubar::create()
{
    ui::container::menubar::sptr menubar = m_registry->get_parent();
    SIGHT_ASSERT("Parent menubar is unknown.", menubar);

    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layout_manager->create_layout(menubar, service_id);
        })
    ).wait();

    m_registry->manage(m_layout_manager->get_menus());
}

//-----------------------------------------------------------------------------

void menubar::destroy()
{
    m_registry->unmanage();

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layout_manager->destroy_layout();
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void menubar::menu_service_stopping(std::string _menu_srv_sid)
{
    ui::container::menu::sptr menu = m_registry->get_fw_menu(_menu_srv_sid, m_layout_manager->get_menus());

    if(m_hide_menus)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layout_manager->menu_is_visible(menu, false);
            })
        ).wait();
    }
    else
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layout_manager->menu_is_enabled(menu, false);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void menubar::menu_service_starting(std::string _menu_srv_sid)
{
    ui::container::menu::sptr menu = m_registry->get_fw_menu(_menu_srv_sid, m_layout_manager->get_menus());

    if(m_hide_menus)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layout_manager->menu_is_visible(menu, true);
            })
        ).wait();
    }
    else
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layout_manager->menu_is_enabled(menu, true);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void menubar::initialize_layout_manager(const ui::config_t& _layout_config)
{
    ui::object::sptr gui_obj = ui::factory::make(
        ui::layout::menubar_manager::REGISTRY_KEY
    );
    m_layout_manager = std::dynamic_pointer_cast<ui::layout::menubar_manager>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::layout::menubar_manager::REGISTRY_KEY,
        m_layout_manager
    );

    m_layout_manager->initialize(_layout_config);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
