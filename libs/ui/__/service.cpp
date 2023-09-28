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
#include "ui/__/detail/registry/View.hpp"
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
    m_viewRegistry = ui::detail::registry::View::make(this->get_id());

    const auto& config = this->getConfiguration();

    if(const auto registryConfig = config.get_child_optional("registry"); registryConfig.has_value())
    {
        m_viewRegistry->initialize(registryConfig.value());
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
        if(const auto toolBar = gui->get_child_optional("toolBar"); toolBar.has_value())
        {
            this->initializeToolBarBuilder(toolBar.value());

            m_hasToolBar = true;
        }

        // find slideView configurations
        const auto slideViewCfg = gui->equal_range("slideView");
        for(const auto& slideCfg : boost::make_iterator_range(slideViewCfg))
        {
            this->initializeSlideViewBuilder(slideCfg.second);
        }
    }
}

//-----------------------------------------------------------------------------

void service::create()
{
    SIGHT_ASSERT(
        "[" + this->get_id() + "'] View must be initialized, don't forget to call 'initialize()' in "
                               "'configuring()' method.",
        m_viewRegistry
    );
    ui::container::widget::sptr parent = m_viewRegistry->getParent();
    SIGHT_ASSERT("Parent container is unknown.", parent);

    core::thread::get_default_worker()->post_task<void>(
        [this, &parent]
        {
            SIGHT_ASSERT("View must be initialized.", m_viewRegistry);

            ui::object::sptr guiObj =
                ui::factory::make(ui::builder::widget::REGISTRY_KEY);
            m_containerBuilder = std::dynamic_pointer_cast<ui::builder::widget>(guiObj);

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

                const std::string serviceID = get_id().substr(get_id().find_last_of('_') + 1);

                core::thread::get_default_worker()->post_task<void>(
                    [&]
                {
                    m_viewLayoutManager->createLayout(container, serviceID);
                }).wait();

                std::vector<ui::container::widget::sptr> views = m_viewLayoutManager->getSubViews();

                for(const auto& slideBuilder : m_slideViewBuilders)
                {
                    SIGHT_ASSERT("Slide builder is not instantiated", slideBuilder);
                    core::thread::get_default_worker()->post_task<void>(
                        [&]
                    {
                        slideBuilder->createContainer(container);
                    }).wait();
                    views.push_back(slideBuilder->getContainer());
                }

                m_viewRegistry->manage(views);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void service::destroy()
{
    SIGHT_ASSERT("View must be initialized.", m_viewRegistry);

    m_viewRegistry->unmanage();

    for(const auto& slideBuilder : m_slideViewBuilders)
    {
        SIGHT_ASSERT("Slide builder is not instantiated", slideBuilder);
        core::thread::get_default_worker()->post_task<void>(
            [&]
            {
                slideBuilder->destroyContainer();
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

void service::initializeLayoutManager(const ui::config_t& layoutConfig)
{
    const auto layoutManagerClassName = layoutConfig.get<std::string>("<xmlattr>.type");

    ui::object::sptr guiObj = ui::factory::make(layoutManagerClassName);
    m_viewLayoutManager = std::dynamic_pointer_cast<ui::layout::view>(guiObj);
    SIGHT_ASSERT("ClassFactoryRegistry failed for class " + layoutManagerClassName, m_viewLayoutManager);

    m_viewLayoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

void service::initializeToolBarBuilder(const ui::config_t& toolBarConfig)
{
    ui::object::sptr guiObj = ui::factory::make(ui::builder::toolbar::REGISTRY_KEY);
    m_toolBarBuilder = std::dynamic_pointer_cast<ui::builder::toolbar>(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " + ui::builder::toolbar::REGISTRY_KEY,
        m_toolBarBuilder
    );

    m_toolBarBuilder->initialize(toolBarConfig);
}

//-----------------------------------------------------------------------------

void service::initializeSlideViewBuilder(const ui::config_t& slideViewConfig)
{
    ui::object::sptr guiObj = ui::factory::make(
        ui::builder::slideview::REGISTRY_KEY
    );
    ui::builder::slideview::sptr slideViewBuildfer =
        std::dynamic_pointer_cast<ui::builder::slideview>(
            guiObj
        );
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " + ui::builder::slideview::REGISTRY_KEY,
        slideViewBuildfer
    );

    slideViewBuildfer->initialize(slideViewConfig);

    m_slideViewBuilders.push_back(slideViewBuildfer);
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr service::getContainer()
{
    return m_containerBuilder->getContainer();
}

//-----------------------------------------------------------------------------

void service::setParent(std::string wid)
{
    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [this, &wid]
        {
            m_viewRegistry->setParent(wid);
            ui::container::widget::sptr parent = m_viewRegistry->getParent();
            SIGHT_ASSERT("Parent container is unknown.", parent);
            m_containerBuilder->setParent(parent);
        })
    );
}

//-----------------------------------------------------------------------------

void service::setEnabled(bool isEnabled)
{
    ui::container::widget::sptr container = m_viewRegistry->getParent();
    container->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void service::setEnabledByParameter(ui::parameter_t isEnabled)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(isEnabled))
    {
        this->setEnabled(std::get<bool>(isEnabled));
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

void service::setVisible(bool isVisible)
{
    ui::container::widget::sptr container = m_viewRegistry->getParent();
    container->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void service::setVisibleByParameter(ui::parameter_t isVisible)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(isVisible))
    {
        this->setVisible(std::get<bool>(isVisible));
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

void service::modifyLayout(ui::parameter_t parameter, std::string key)
{
    if(m_viewLayoutManagerIsCreated)
    {
        SIGHT_ASSERT("ViewLayoutManager must be initialized.", m_viewLayoutManager);
        m_viewLayoutManager->modifyLayout(parameter, key);
    }
}

} // namespace sight::ui
