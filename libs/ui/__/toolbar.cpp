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

#include "ui/__/toolbar.hpp"

#include "ui/__/action.hpp"
#include "ui/__/menu_item_callback.hpp"

#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>
#include <core/tools/id.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

namespace sight::ui
{

//-----------------------------------------------------------------------------

toolbar::toolbar()
{
    new_slot(slots::SET_VISIBLE_SLOT, &toolbar::setVisible, this);
    new_slot(slots::SET_VISIBLE_BY_PARAM_SLOT, &toolbar::setVisibleByParameter, this);
    new_slot(slots::SHOW_SLOT, &toolbar::show, this);
    new_slot(slots::HIDE_SLOT, &toolbar::hide, this);
}

//-----------------------------------------------------------------------------

void toolbar::initialize()
{
    m_registry = ui::detail::registry::tool_bar::make(this->get_id());

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

void toolbar::create()
{
    ui::container::toolbar::sptr tool_bar               = m_registry->getParent();
    std::vector<ui::menu_item_callback::sptr> callbacks = m_registry->getCallbacks();

    SIGHT_ASSERT("Parent toolBar is unknown.", tool_bar);
    m_layoutManager->setCallbacks(callbacks);

    const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->createLayout(tool_bar, service_id);
        })
    ).wait();

    m_registry->manage(m_layoutManager->getMenuItems());
    m_registry->manage(m_layoutManager->getMenus());
    m_registry->manage(m_layoutManager->getContainers());
}

//-----------------------------------------------------------------------------

void toolbar::destroy()
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

void toolbar::actionServiceStopping(std::string _action_srv_sid)
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

void toolbar::actionServiceStarting(std::string _action_srv_sid)
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
            m_layoutManager->menuItemSetEnabled(menu_item, action_srv->enabled());
            const bool inverted   = action_srv->inverted();
            const bool is_checked = action_srv->checked();
            m_layoutManager->menuItemSetChecked(menu_item, inverted ? !is_checked : is_checked);
            m_layoutManager->menuItemSetVisible(menu_item, action_srv->visible());
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::actionServiceSetChecked(std::string _action_srv_sid, bool _is_checked)
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

void toolbar::actionServiceSetEnabled(std::string _action_srv_sid, bool _is_enabled)
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

void toolbar::actionServiceSetVisible(std::string _action_srv_sid, bool _is_visible)
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

void toolbar::initializeLayoutManager(const ui::config_t& _layout_config)
{
    ui::object::sptr gui_obj = ui::factory::make(
        ui::layout::toolbar_manager::REGISTRY_KEY
    );
    m_layoutManager = std::dynamic_pointer_cast<ui::layout::toolbar_manager>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::layout::toolbar_manager::REGISTRY_KEY,
        m_layoutManager
    );

    m_layoutManager->initialize(_layout_config);
}

//-----------------------------------------------------------------------------

void toolbar::setVisible(bool _is_visible)
{
    m_layoutManager->setVisible(_is_visible);
}

//-----------------------------------------------------------------------------

void toolbar::setVisibleByParameter(ui::parameter_t _is_visible)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(_is_visible))
    {
        this->setVisible(std::get<bool>(_is_visible));
    }
}

//-----------------------------------------------------------------------------

void toolbar::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void toolbar::hide()
{
    this->setVisible(false);
}

} // namespace sight::ui
