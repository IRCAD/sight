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
    new_slot(slots::SET_ENABLED, &service::setEnabled, this);
    new_slot(slots::SET_ENABLED_BY_PARAM, &service::setEnabledByParameter, this);
    new_slot(slots::ENABLE, &service::enable, this);
    new_slot(slots::DISABLE, &service::disable, this);
    new_slot(slots::SET_VISIBLE, &service::setVisible, this);
    new_slot(slots::SET_VISIBLE_BY_PARAM, &service::setVisibleByParameter, this);
    new_slot(slots::SHOW, &service::show, this);
    new_slot(slots::HIDE, &service::hide, this);
    new_slot(slots::TOGGLE_VISIBILITY, &service::toggleVisibility, this);
    new_slot(slots::MODIFY_LAYOUT, &service::modifyLayout, this);
}

//-----------------------------------------------------------------------------

void service::initialize()
{
    // Create view registry
    m_viewRegistry = ui::detail::registry::view::make(this->get_id());

    const auto& config = this->get_config();

    if(const auto registry_config = config.get_child_optional("registry"); registry_config.has_value())
    {
        m_viewRegistry->initialize(registry_config.value());
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
            this->initializeLayoutManager(layout.value());
            m_viewLayoutManagerIsCreated = true;
        }

        // find toolBarBuilder configuration
        if(const auto tool_bar = gui->get_child_optional("toolBar"); tool_bar.has_value())
        {
            this->initializeToolBarBuilder(tool_bar.value());

            m_hasToolBar = true;
        }

        // find slideView configurations
        const auto slide_view_cfg = gui->equal_range("slideView");
        for(const auto& slide_cfg : boost::make_iterator_range(slide_view_cfg))
        {
            this->initializeSlideViewBuilder(slide_cfg.second);
        }
    }
}

//-----------------------------------------------------------------------------

void service::create()
{
    SIGHT_ASSERT(
        "[" + this->get_id() + "'] view must be initialized, don't forget to call 'initialize()' in "
                               "'configuring()' method.",
        m_viewRegistry
    );
    ui::container::widget::sptr parent = m_viewRegistry->getParent();
    SIGHT_ASSERT("Parent container is unknown.", parent);

    core::thread::get_default_worker()->post_task<void>(
        [this, &parent]
        {
            SIGHT_ASSERT("view must be initialized.", m_viewRegistry);

            ui::object::sptr gui_obj =
                ui::factory::make(ui::builder::widget::REGISTRY_KEY);
            m_containerBuilder = std::dynamic_pointer_cast<ui::builder::widget>(gui_obj);

            SIGHT_ASSERT(
                "Cannot create main container: factory failed for '"
                + ui::builder::widget::REGISTRY_KEY + "'",
                m_containerBuilder
            );
            m_containerBuilder->createContainer(parent);

            ui::container::widget::sptr container = m_containerBuilder->getContainer();

            if(m_viewLayoutManagerIsCreated)
            {
                if(m_hasToolBar)
                {
                    core::thread::get_default_worker()->post_task<void>(
                        [&]
                    {
                        m_toolBarBuilder->createToolBar(parent);
                    }).wait();

                    m_viewRegistry->manageToolBar(m_toolBarBuilder->getToolBar());
                }

                const std::string service_id = get_id().substr(get_id().find_last_of('_') + 1);

                core::thread::get_default_worker()->post_task<void>(
                    [&]
                {
                    m_viewLayoutManager->createLayout(container, service_id);
                }).wait();

                std::vector<ui::container::widget::sptr> views = m_viewLayoutManager->getSubViews();

                for(const auto& slide_builder : m_slideViewBuilders)
                {
                    SIGHT_ASSERT("Slide builder is not instantiated", slide_builder);
                    core::thread::get_default_worker()->post_task<void>(
                        [&]
                    {
                        slide_builder->createContainer(container);
                    }).wait();
                    views.push_back(slide_builder->getContainer());
                }

                m_viewRegistry->manage(views);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void service::destroy()
{
    SIGHT_ASSERT("view must be initialized.", m_viewRegistry);

    m_viewRegistry->unmanage();

    for(const auto& slide_builder : m_slideViewBuilders)
    {
        SIGHT_ASSERT("Slide builder is not instantiated", slide_builder);
        core::thread::get_default_worker()->post_task<void>(
            [&]
            {
                slide_builder->destroyContainer();
            }).wait();
    }

    if(m_viewLayoutManagerIsCreated)
    {
        SIGHT_ASSERT("ViewLayoutManager must be initialized.", m_viewLayoutManager);

        core::thread::get_default_worker()->post_task<void>(
            [&]
            {
                m_viewLayoutManager->destroyLayout();
            }).wait();

        if(m_hasToolBar)
        {
            m_viewRegistry->unmanageToolBar();
            SIGHT_ASSERT("toolbar must be initialized.", m_toolBarBuilder);

            core::thread::get_default_worker()->post_task<void>(
                [&]
                {
                    m_toolBarBuilder->destroyToolBar();
                }).wait();
        }
    }

    m_containerBuilder->destroyContainer();
}

//-----------------------------------------------------------------------------

void service::initializeLayoutManager(const ui::config_t& _layout_config)
{
    const auto layout_manager_class_name = _layout_config.get<std::string>("<xmlattr>.type");

    ui::object::sptr gui_obj = ui::factory::make(layout_manager_class_name);
    m_viewLayoutManager = std::dynamic_pointer_cast<ui::layout::view>(gui_obj);
    SIGHT_ASSERT("ClassFactoryRegistry failed for class " + layout_manager_class_name, m_viewLayoutManager);

    m_viewLayoutManager->initialize(_layout_config);
}

//-----------------------------------------------------------------------------

void service::initializeToolBarBuilder(const ui::config_t& _tool_bar_config)
{
    ui::object::sptr gui_obj = ui::factory::make(ui::builder::toolbar::REGISTRY_KEY);
    m_toolBarBuilder = std::dynamic_pointer_cast<ui::builder::toolbar>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " + ui::builder::toolbar::REGISTRY_KEY,
        m_toolBarBuilder
    );

    m_toolBarBuilder->initialize(_tool_bar_config);
}

//-----------------------------------------------------------------------------

void service::initializeSlideViewBuilder(const ui::config_t& _slide_view_config)
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

    m_slideViewBuilders.push_back(slide_view_buildfer);
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr service::getContainer()
{
    return m_containerBuilder->getContainer();
}

//-----------------------------------------------------------------------------

void service::setParent(std::string _wid)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [this, &_wid]
        {
            m_viewRegistry->setParent(_wid);
            ui::container::widget::sptr parent = m_viewRegistry->getParent();
            SIGHT_ASSERT("Parent container is unknown.", parent);
            m_containerBuilder->setParent(parent);
        })
    );
}

//-----------------------------------------------------------------------------

void service::setEnabled(bool _is_enabled)
{
    ui::container::widget::sptr container = m_viewRegistry->getParent();
    container->setEnabled(_is_enabled);
}

//-----------------------------------------------------------------------------

void service::setEnabledByParameter(ui::parameter_t _is_enabled)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(_is_enabled))
    {
        this->setEnabled(std::get<bool>(_is_enabled));
    }
}

//-----------------------------------------------------------------------------

void service::enable()
{
    this->setEnabled(true);
}

//-----------------------------------------------------------------------------

void service::disable()
{
    this->setEnabled(false);
}

//-----------------------------------------------------------------------------

void service::setVisible(bool _is_visible)
{
    ui::container::widget::sptr container = m_viewRegistry->getParent();
    container->setVisible(_is_visible);
}

//-----------------------------------------------------------------------------

void service::setVisibleByParameter(ui::parameter_t _is_visible)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(_is_visible))
    {
        this->setVisible(std::get<bool>(_is_visible));
    }
}

//-----------------------------------------------------------------------------

void service::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void service::hide()
{
    this->setVisible(false);
}

//------------------------------------------------------------------------------

void service::toggleVisibility()
{
    this->setVisible(!m_viewRegistry->getParent()->isShownOnScreen());
}

//-----------------------------------------------------------------------------

void service::modifyLayout(ui::parameter_t _parameter, std::string _key)
{
    if(m_viewLayoutManagerIsCreated)
    {
        SIGHT_ASSERT("ViewLayoutManager must be initialized.", m_viewLayoutManager);
        m_viewLayoutManager->modifyLayout(_parameter, _key);
    }
}

} // namespace sight::ui
