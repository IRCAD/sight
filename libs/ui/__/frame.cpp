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

#include "ui/__/frame.hpp"

#include "ui/__/application.hpp"

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>
#include <core/tools/id.hpp>

#include <service/macros.hpp>

namespace sight::ui
{

const std::string frame::CLOSE_POLICY_EXIT    = "exit";
const std::string frame::CLOSE_POLICY_NOTIFY  = "notify";
const std::string frame::CLOSE_POLICY_MESSAGE = "message";

const core::com::slots::key_t frame::SET_VISIBLE_SLOT = "setVisible";
const core::com::slots::key_t frame::SHOW_SLOT        = "show";
const core::com::slots::key_t frame::HIDE_SLOT        = "hide";

const core::com::signals::key_t frame::CLOSED_SIG = "closed";

ui::container::widget::wptr frame::s_progress_widget = std::weak_ptr<ui::container::widget>();

//-----------------------------------------------------------------------------

frame::frame() :

    m_close_policy("exit")
{
    m_sig_closed = new_signal<closed_signal_t>(CLOSED_SIG);

    new_slot(SET_VISIBLE_SLOT, &frame::set_visible, this);
    new_slot(SHOW_SLOT, &frame::show, this);
    new_slot(HIDE_SLOT, &frame::hide, this);
}

//-----------------------------------------------------------------------------

void frame::initialize()
{
    m_view_registry = ui::detail::registry::view::make(this->get_id());

    const auto config = this->get_config();
    const auto gui    = config.get_child_optional("gui");

    if(gui.has_value())
    {
        this->initialize_layout_manager(gui->get_child("frame"));

        if(const auto menu_bar = gui->get_child_optional("menubar"); menu_bar.has_value())
        {
            this->initialize_menu_bar_builder(menu_bar.value());

            m_has_menu_bar = true;
        }

        if(const auto tool_bar = gui->get_child_optional("toolbar"); tool_bar.has_value())
        {
            this->initialize_tool_bar_builder(tool_bar.value());

            m_has_tool_bar = true;
        }

        // find ViewRegistryManager configuration
        if(const auto registry = config.get_child_optional("registry"); registry.has_value())
        {
            m_view_registry->initialize(registry.value());
        }
    }

    if(const auto onclose = config.get<std::string>("window.<xmlattr>.onclose", ""); !onclose.empty())
    {
        m_close_policy = onclose;
        SIGHT_ASSERT(
            "[" + this->get_id() + "' ] Invalid onclose value, actual: '" << m_close_policy << "', accepted: '"
            + CLOSE_POLICY_NOTIFY + "', '" + CLOSE_POLICY_EXIT + "' or '" + CLOSE_POLICY_MESSAGE + "'",
            m_close_policy == CLOSE_POLICY_NOTIFY || m_close_policy == CLOSE_POLICY_EXIT
            || m_close_policy == CLOSE_POLICY_MESSAGE
        );
    }
}

//-----------------------------------------------------------------------------

void frame::create()
{
    SIGHT_ASSERT(
        "[" + this->get_id() + "' ] frame must be initialized, don't forget to call 'initialize()' in "
                               "'configuring()' method.",
        m_frame_layout_manager
    );

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_frame_layout_manager->create_frame();
        })
    ).wait();

    ui::container::widget::sptr frame = m_frame_layout_manager->get_frame();
    if(s_progress_widget.expired())
    {
        s_progress_widget = frame;
    }

    ui::container::widget::sptr container = m_frame_layout_manager->get_container();
    std::vector<ui::container::widget::sptr> sub_views;
    sub_views.push_back(container);
    m_view_registry->manage(sub_views);

    ui::layout::frame_manager::CloseCallback fct;

    if(m_close_policy == CLOSE_POLICY_EXIT)
    {
        fct = on_close_exit;
    }
    else if(m_close_policy == CLOSE_POLICY_NOTIFY)
    {
        fct = [this](auto&& ...){on_close_notify();};
    }
    else if(m_close_policy == CLOSE_POLICY_MESSAGE)
    {
        fct = on_close_message;
        auto app = sight::ui::application::get();
        app->set_confirm(true);
    }

    m_frame_layout_manager->set_close_callback(fct);

    if(m_has_menu_bar)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_menu_bar_builder->create_menu_bar(frame);
            })
        ).wait();

        m_view_registry->manage_menu_bar(m_menu_bar_builder->get_menu_bar());
    }

    if(m_has_tool_bar)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_tool_bar_builder->create_tool_bar(frame);
            })
        ).wait();

        m_view_registry->manage_tool_bar(m_tool_bar_builder->get_tool_bar());
    }
}

//-----------------------------------------------------------------------------

void frame::destroy()
{
    SIGHT_ASSERT("view must be initialized.", m_view_registry);

    if(m_has_tool_bar)
    {
        m_view_registry->unmanage_tool_bar();
        SIGHT_ASSERT("toolbar must be initialized.", m_tool_bar_builder);

        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_tool_bar_builder->destroy_tool_bar();
            })
        ).wait();
    }

    if(m_has_menu_bar)
    {
        m_view_registry->unmanage_menu_bar();
        SIGHT_ASSERT("menubar must be initialized.", m_menu_bar_builder);

        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_menu_bar_builder->destroy_menu_bar();
            })
        ).wait();
    }

    m_view_registry->unmanage();
    SIGHT_ASSERT("frame must be initialized.", m_frame_layout_manager);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_frame_layout_manager->destroy_frame();
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void frame::initialize_layout_manager(const ui::config_t& _layout_config)
{
    ui::object::sptr gui_obj = ui::factory::make(
        ui::layout::frame_manager::REGISTRY_KEY
    );
    m_frame_layout_manager = std::dynamic_pointer_cast<ui::layout::frame_manager>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::layout::frame_manager::REGISTRY_KEY,
        m_frame_layout_manager
    );

    m_frame_layout_manager->initialize(_layout_config);
}

//-----------------------------------------------------------------------------

void frame::initialize_menu_bar_builder(const ui::config_t& _menu_bar_config)
{
    ui::object::sptr gui_obj = ui::factory::make(ui::builder::menubar::REGISTRY_KEY);
    m_menu_bar_builder = std::dynamic_pointer_cast<ui::builder::menubar>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::builder::menubar::REGISTRY_KEY,
        m_menu_bar_builder
    );

    m_menu_bar_builder->initialize(_menu_bar_config);
}

//-----------------------------------------------------------------------------

void frame::initialize_tool_bar_builder(const ui::config_t& _tool_bar_config)
{
    ui::object::sptr gui_obj = ui::factory::make(ui::builder::toolbar::REGISTRY_KEY);
    m_tool_bar_builder = std::dynamic_pointer_cast<ui::builder::toolbar>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::builder::toolbar::REGISTRY_KEY,
        m_tool_bar_builder
    );

    m_tool_bar_builder->initialize(_tool_bar_config);
}

//-----------------------------------------------------------------------------

void frame::on_close_exit()
{
    sight::ui::application::get()->exit(0);
}

//-----------------------------------------------------------------------------

void frame::on_close_notify()
{
    m_sig_closed->async_emit();
}

//-----------------------------------------------------------------------------

void frame::on_close_message()
{
    auto app = sight::ui::application::get();
    app->exit(0);
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr frame::get_progress_widget()
{
    return s_progress_widget.lock();
}

//-----------------------------------------------------------------------------

void frame::set_visible(bool _is_visible)
{
    ui::container::widget::sptr container = m_frame_layout_manager->get_frame();
    container->set_visible(_is_visible);
}

//-----------------------------------------------------------------------------

void frame::show()
{
    this->set_visible(true);
}

//-----------------------------------------------------------------------------

void frame::hide()
{
    this->set_visible(false);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
