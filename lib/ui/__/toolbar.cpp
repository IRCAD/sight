/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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
#include <core/id.hpp>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>

#include <service/macros.hpp>
#include <service/op.hpp>

namespace sight::ui
{

//-----------------------------------------------------------------------------

toolbar::toolbar()
{
    new_slot(slots::SET_VISIBLE, &toolbar::set_visible, this);
    new_slot(slots::SET_VISIBLE_BY_PARAM, &toolbar::set_visible_by_parameter, this);
    new_slot(slots::SHOW, &toolbar::show, this);
    new_slot(slots::HIDE, &toolbar::hide, this);
    new_slot(slots::SET_ENABLED, &toolbar::set_enabled, this);
    new_slot(slots::ENABLE, &toolbar::enable, this);
    new_slot(slots::DISABLE, &toolbar::disable, this);
}

//-----------------------------------------------------------------------------

void toolbar::initialize()
{
    m_registry = ui::detail::registry::toolbar::make(this->get_id());

    const auto& config = this->get_config();

    // find ViewRegistryManager configuration
    if(const auto registry_config = config.get_child_optional("registry"); registry_config.has_value())
    {
        m_registry->initialize(registry_config.value());
    }

    // find layout configuration
    if(const auto layout_config = config.get_child_optional("gui.layout"); layout_config.has_value())
    {
        this->initialize_layout_manager(layout_config.value());

        m_hide_actions = layout_config->get<bool>("<xmlattr>.hideActions", m_hide_actions);
    }
}

//-----------------------------------------------------------------------------

void toolbar::create()
{
    ui::container::toolbar::sptr tool_bar               = m_registry->get_parent();
    std::vector<ui::menu_item_callback::sptr> callbacks = m_registry->get_callbacks();

    SIGHT_ASSERT("Parent toolbar is unknown.", tool_bar);
    m_layout_manager->set_callbacks(callbacks);

    const std::string service_id = base_id();

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layout_manager->create_layout(tool_bar, service_id);
        })
    ).wait();

    m_registry->manage(m_layout_manager->get_menu_items());
    m_registry->manage(m_layout_manager->get_menus());
    m_registry->manage(m_layout_manager->get_containers());
}

//-----------------------------------------------------------------------------

void toolbar::destroy()
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

void toolbar::action_service_stopping(std::string _action_srv_sid)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layout_manager->get_menu_items()
    );

    if(m_hide_actions)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layout_manager->menu_item_set_visible(menu_item, false);
            })
        ).wait();
    }
    else
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layout_manager->menu_item_set_enabled(menu_item, false);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void toolbar::action_service_starting(std::string _action_srv_sid)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layout_manager->get_menu_items()
    );

    const service::base::csptr service = service::get(_action_srv_sid);
    const ui::action::csptr action_srv = std::dynamic_pointer_cast<const ui::action>(service);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layout_manager->menu_item_set_enabled(menu_item, action_srv->enabled());
            const bool inverted   = action_srv->inverted();
            const bool is_checked = action_srv->checked();
            m_layout_manager->menu_item_set_checked(menu_item, inverted ? !is_checked : is_checked);
            m_layout_manager->menu_item_set_visible(menu_item, action_srv->visible());
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::action_service_set_checked(std::string _action_srv_sid, bool _is_checked)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layout_manager->get_menu_items()
    );

    const service::base::csptr service = service::get(_action_srv_sid);
    const ui::action::csptr action_srv = std::dynamic_pointer_cast<const ui::action>(service);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            const bool inverted = action_srv->inverted();
            m_layout_manager->menu_item_set_checked(menu_item, inverted ? !_is_checked : _is_checked);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::action_service_set_enabled(std::string _action_srv_sid, bool _is_enabled)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layout_manager->get_menu_items()
    );

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layout_manager->menu_item_set_enabled(menu_item, _is_enabled);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::action_service_set_visible(std::string _action_srv_sid, bool _is_visible)
{
    ui::container::menu_item::sptr menu_item = m_registry->get_menu_item(
        _action_srv_sid,
        m_layout_manager->get_menu_items()
    );

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layout_manager->menu_item_set_visible(menu_item, _is_visible);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::initialize_layout_manager(const ui::config_t& _layout_config)
{
    ui::object::sptr gui_obj = ui::factory::make(
        ui::layout::toolbar_manager::REGISTRY_KEY
    );
    m_layout_manager = std::dynamic_pointer_cast<ui::layout::toolbar_manager>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::layout::toolbar_manager::REGISTRY_KEY,
        m_layout_manager
    );

    m_layout_manager->initialize(_layout_config);
}

//-----------------------------------------------------------------------------

void toolbar::set_visible(bool _is_visible)
{
    m_layout_manager->set_visible(_is_visible);
}

//-----------------------------------------------------------------------------

bool toolbar::visible() const
{
    return m_layout_manager->visible();
}

//-----------------------------------------------------------------------------

void toolbar::set_visible_by_parameter(ui::parameter_t _is_visible)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(_is_visible))
    {
        this->set_visible(std::get<bool>(_is_visible));
    }
}

//-----------------------------------------------------------------------------

void toolbar::show()
{
    this->set_visible(true);
}

//-----------------------------------------------------------------------------

void toolbar::hide()
{
    this->set_visible(false);
}

//-----------------------------------------------------------------------------

void toolbar::set_enabled(bool _enabled)
{
    m_layout_manager->set_enabled(_enabled);
}

//-----------------------------------------------------------------------------

void toolbar::enable()
{
    m_layout_manager->set_enabled(true);
}

//-----------------------------------------------------------------------------

void toolbar::disable()
{
    m_layout_manager->set_enabled(false);
}

} // namespace sight::ui
