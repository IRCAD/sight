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

#include "ui/__/service.hpp"

#include "ui/__/builder/slideview.hpp"
#include "ui/__/builder/toolbar.hpp"
#include "ui/__/builder/widget.hpp"
#include "ui/__/container/widget.hpp"
#include "ui/__/detail/registry/view.hpp"
#include "ui/__/layout/view.hpp"

#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <boost/range/iterator_range_core.hpp>

namespace sight::ui
{

//-----------------------------------------------------------------------------

service::service()
{
    new_slot(slots::SET_ENABLED, &service::set_enabled, this);
    new_slot(slots::SET_ENABLED_BY_PARAM, &service::set_enabled_by_parameter, this);
    new_slot(slots::ENABLE, &service::enable, this);
    new_slot(slots::DISABLE, &service::disable, this);
    new_slot(slots::SET_VISIBLE, &service::set_visible, this);
    new_slot(slots::SET_VISIBLE_BY_PARAM, &service::set_visible_by_parameter, this);
    new_slot(slots::SHOW, &service::show, this);
    new_slot(slots::HIDE, &service::hide, this);
    new_slot(slots::TOGGLE_VISIBILITY, &service::toggle_visibility, this);
    new_slot(slots::MODIFY_LAYOUT, &service::modify_layout, this);
}

//-----------------------------------------------------------------------------

void service::initialize()
{
    // Create view registry
    m_view_registry = ui::detail::registry::view::make(this->get_id());

    const auto& config = this->get_config();

    if(const auto registry_config = config.get_child_optional("registry"); registry_config.has_value())
    {
        m_view_registry->initialize(registry_config.value());
    }

    // Create initializeLayoutManager
    const auto gui = config.get_child_optional("gui");

    if(gui.has_value())
    {
        SIGHT_ASSERT(
            "[" + this->get_id() + "' ] No <registry> tag is allowed in the <gui> section",
            !gui->get_child_optional("registry").has_value()
        );

        // find view layout configuration
        if(const auto layout = gui->get_child_optional("layout"); layout.has_value())
        {
            this->initialize_layout_manager(layout.value());
            m_view_layout_manager_is_created = true;
        }

        // find toolBarBuilder configuration
        if(const auto tool_bar = gui->get_child_optional("toolbar"); tool_bar.has_value())
        {
            this->initialize_tool_bar_builder(tool_bar.value());

            m_has_tool_bar = true;
        }

        // find slideView configurations
        const auto slide_view_cfg = gui->equal_range("slideView");
        for(const auto& slide_cfg : boost::make_iterator_range(slide_view_cfg))
        {
            this->initialize_slide_view_builder(slide_cfg.second);
        }
    }
}

//-----------------------------------------------------------------------------

void service::create()
{
    SIGHT_ASSERT(
        "[" + this->get_id() + "'] view must be initialized, don't forget to call 'initialize()' in "
                               "'configuring()' method.",
        m_view_registry
    );
    ui::container::widget::sptr parent = m_view_registry->get_parent();
    SIGHT_ASSERT("Parent container is unknown.", parent);

    core::thread::get_default_worker()->post_task<void>(
        [this, &parent]
        {
            SIGHT_ASSERT("view must be initialized.", m_view_registry);

            ui::object::sptr gui_obj =
                ui::factory::make(ui::builder::widget::REGISTRY_KEY);
            m_container_builder = std::dynamic_pointer_cast<ui::builder::widget>(gui_obj);

            SIGHT_ASSERT(
                "Cannot create main container: factory failed for '"
                + ui::builder::widget::REGISTRY_KEY + "'",
                m_container_builder
            );
            m_container_builder->create_container(parent);

            ui::container::widget::sptr container = m_container_builder->get_container();

            if(m_view_layout_manager_is_created)
            {
                if(m_has_tool_bar)
                {
                    core::thread::get_default_worker()->post_task<void>(
                        [&]
                    {
                        m_tool_bar_builder->create_tool_bar(parent);
                    }).wait();

                    m_view_registry->manage_tool_bar(m_tool_bar_builder->get_tool_bar());
                }

                const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);

                core::thread::get_default_worker()->post_task<void>(
                    [&]
                {
                    m_view_layout_manager->create_layout(container, service_id);
                }).wait();

                std::vector<ui::container::widget::sptr> views = m_view_layout_manager->get_sub_views();

                for(const auto& slide_builder : m_slide_view_builders)
                {
                    SIGHT_ASSERT("Slide builder is not instantiated", slide_builder);
                    core::thread::get_default_worker()->post_task<void>(
                        [&]
                    {
                        slide_builder->create_container(container);
                    }).wait();
                    views.push_back(slide_builder->get_container());
                }

                m_view_registry->manage(views);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void service::destroy()
{
    SIGHT_ASSERT("view must be initialized.", m_view_registry);

    m_view_registry->unmanage();

    for(const auto& slide_builder : m_slide_view_builders)
    {
        SIGHT_ASSERT("Slide builder is not instantiated", slide_builder);
        core::thread::get_default_worker()->post_task<void>(
            [&]
            {
                slide_builder->destroy_container();
            }).wait();
    }

    if(m_view_layout_manager_is_created)
    {
        SIGHT_ASSERT("ViewLayoutManager must be initialized.", m_view_layout_manager);

        core::thread::get_default_worker()->post_task<void>(
            [&]
            {
                m_view_layout_manager->destroy_layout();
            }).wait();

        if(m_has_tool_bar)
        {
            m_view_registry->unmanage_tool_bar();
            SIGHT_ASSERT("toolbar must be initialized.", m_tool_bar_builder);

            core::thread::get_default_worker()->post_task<void>(
                [&]
                {
                    m_tool_bar_builder->destroy_tool_bar();
                }).wait();
        }
    }

    m_container_builder->destroy_container();
}

//-----------------------------------------------------------------------------

void service::initialize_layout_manager(const ui::config_t& _layout_config)
{
    const auto layout_manager_class_name = _layout_config.get<std::string>("<xmlattr>.type");

    ui::object::sptr gui_obj = ui::factory::make(layout_manager_class_name);
    m_view_layout_manager = std::dynamic_pointer_cast<ui::layout::view>(gui_obj);
    SIGHT_ASSERT("ClassFactoryRegistry failed for class " + layout_manager_class_name, m_view_layout_manager);

    m_view_layout_manager->initialize(_layout_config);
}

//-----------------------------------------------------------------------------

void service::initialize_tool_bar_builder(const ui::config_t& _tool_bar_config)
{
    ui::object::sptr gui_obj = ui::factory::make(ui::builder::toolbar::REGISTRY_KEY);
    m_tool_bar_builder = std::dynamic_pointer_cast<ui::builder::toolbar>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " + ui::builder::toolbar::REGISTRY_KEY,
        m_tool_bar_builder
    );

    m_tool_bar_builder->initialize(_tool_bar_config);
}

//-----------------------------------------------------------------------------

void service::initialize_slide_view_builder(const ui::config_t& _slide_view_config)
{
    ui::object::sptr gui_obj = ui::factory::make(
        ui::builder::slideview::REGISTRY_KEY
    );
    ui::builder::slideview::sptr slide_view_buildfer =
        std::dynamic_pointer_cast<ui::builder::slideview>(
            gui_obj
        );
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " + ui::builder::slideview::REGISTRY_KEY,
        slide_view_buildfer
    );

    slide_view_buildfer->initialize(_slide_view_config);

    m_slide_view_builders.push_back(slide_view_buildfer);
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr service::get_container()
{
    return m_container_builder->get_container();
}

//-----------------------------------------------------------------------------

void service::set_parent(std::string _wid)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [this, &_wid]
        {
            m_view_registry->set_parent(_wid);
            ui::container::widget::sptr parent = m_view_registry->get_parent();
            SIGHT_ASSERT("Parent container is unknown.", parent);
            m_container_builder->set_parent(parent);
        })
    );
}

//-----------------------------------------------------------------------------

void service::set_enabled(bool _is_enabled)
{
    ui::container::widget::sptr container = m_view_registry->get_parent();
    container->set_enabled(_is_enabled);
}

//-----------------------------------------------------------------------------

void service::set_enabled_by_parameter(ui::parameter_t _is_enabled)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(_is_enabled))
    {
        this->set_enabled(std::get<bool>(_is_enabled));
    }
}

//-----------------------------------------------------------------------------

void service::enable()
{
    this->set_enabled(true);
}

//-----------------------------------------------------------------------------

void service::disable()
{
    this->set_enabled(false);
}

//-----------------------------------------------------------------------------

void service::set_visible(bool _is_visible)
{
    ui::container::widget::sptr container = m_view_registry->get_parent();
    container->set_visible(_is_visible);
}

//-----------------------------------------------------------------------------

void service::set_visible_by_parameter(ui::parameter_t _is_visible)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(_is_visible))
    {
        this->set_visible(std::get<bool>(_is_visible));
    }
}

//-----------------------------------------------------------------------------

void service::show()
{
    this->set_visible(true);
}

//-----------------------------------------------------------------------------

void service::hide()
{
    this->set_visible(false);
}

//------------------------------------------------------------------------------

void service::toggle_visibility()
{
    this->set_visible(!m_view_registry->get_parent()->is_shown_on_screen());
}

//-----------------------------------------------------------------------------

void service::modify_layout(ui::parameter_t _parameter, std::string _key)
{
    if(m_view_layout_manager_is_created)
    {
        SIGHT_ASSERT("ViewLayoutManager must be initialized.", m_view_layout_manager);
        m_view_layout_manager->modify_layout(_parameter, _key);
    }
}

} // namespace sight::ui
