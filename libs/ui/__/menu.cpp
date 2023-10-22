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

#include "ui/__/menu.hpp"

#include "ui/__/action.hpp"
#include "ui/__/menu_item_callback.hpp"

#include <core/thread/worker.hxx>

#include <service/op.hpp>

namespace sight::ui
{

//-----------------------------------------------------------------------------

void menu::initialize()
{
    m_registry = ui::detail::registry::menu::make(this->get_id());

    const auto& config = this->get_config();

    // find ViewRegistryManager configuration
    if(const auto registry_config = config.get_child_optional("registry"); registry_config.has_value())
    {
        m_registry->initialize(registry_config.value());
    }

    // find layout configuration
    if(const auto layout_config = config.get_child_optional("gui.layout"); layout_config.has_value())
    {
        this->initializeLayoutManager(layout_config.value());

        m_hideActions = layout_config->get<bool>("hideActions", m_hideActions);
    }
}

//-----------------------------------------------------------------------------

void menu::create()
{
    ui::container::menu::sptr menu                      = m_registry->getParent();
    std::vector<ui::menu_item_callback::sptr> callbacks = m_registry->getCallbacks();

    SIGHT_ASSERT("Parent menu is unknown.", menu);
    m_layoutManager->setCallbacks(callbacks);

    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->createLayout(menu, service_id);
        })
    ).wait();

    m_registry->manage(m_layoutManager->getMenuItems());
    m_registry->manage(m_layoutManager->getMenus());
}

//-----------------------------------------------------------------------------

void menu::destroy()
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

void menu::actionServiceStopping(std::string _action_srv_sid)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layoutManager->getMenuItems()
    );

    if(m_hideActions)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetVisible(menu_item, false);
            })
        ).wait();
    }
    else
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetEnabled(menu_item, false);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void menu::actionServiceStarting(std::string _action_srv_sid)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layoutManager->getMenuItems()
    );

    if(m_hideActions)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetVisible(menu_item, true);
            })
        ).wait();
    }
    else
    {
        const service::base::csptr service = service::get(_action_srv_sid);
        const ui::action::csptr action_srv = std::dynamic_pointer_cast<const ui::action>(service);

        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetEnabled(menu_item, action_srv->enabled());
                const bool inverted   = action_srv->inverted();
                const bool is_checked = action_srv->checked();
                m_layoutManager->menuItemSetChecked(menu_item, inverted ? !is_checked : is_checked);
                m_layoutManager->menuItemSetVisible(menu_item, action_srv->visible());
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void menu::actionServiceSetChecked(std::string _action_srv_sid, bool _is_checked)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layoutManager->getMenuItems()
    );

    const service::base::csptr service = service::get(_action_srv_sid);
    const ui::action::csptr action_srv = std::dynamic_pointer_cast<const ui::action>(service);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            const bool inverted = action_srv->inverted();
            m_layoutManager->menuItemSetChecked(menu_item, inverted ? !_is_checked : _is_checked);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void menu::actionServiceSetEnabled(std::string _action_srv_sid, bool _is_enabled)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layoutManager->getMenuItems()
    );

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->menuItemSetEnabled(menu_item, _is_enabled);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void menu::actionServiceSetVisible(std::string _action_srv_sid, bool _is_visible)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layoutManager->getMenuItems()
    );

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->menuItemSetVisible(menu_item, _is_visible);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void menu::initializeLayoutManager(const ui::config_t& _layout_config)
{
    ui::object::sptr gui_obj = ui::factory::make(
        ui::layout::menu_manager::REGISTRY_KEY
    );
    m_layoutManager = std::dynamic_pointer_cast<ui::layout::menu_manager>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::layout::menu_manager::REGISTRY_KEY,
        m_layoutManager
    );

    m_layoutManager->initialize(_layout_config);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
