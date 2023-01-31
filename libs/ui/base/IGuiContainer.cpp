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

#include "ui/base/IGuiContainer.hpp"

#include "ui/base/builder/IContainerBuilder.hpp"
#include "ui/base/builder/ISlideViewBuilder.hpp"
#include "ui/base/builder/IToolBarBuilder.hpp"
#include "ui/base/container/fwContainer.hpp"
#include "ui/base/layoutManager/IViewLayoutManager.hpp"
#include "ui/base/registry/View.hpp"

#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>

#include <boost/range/iterator_range_core.hpp>

namespace sight::ui::base
{

const core::com::Slots::SlotKeyType IGuiContainer::s_SET_ENABLED_SLOT          = "setEnabled";
const core::com::Slots::SlotKeyType IGuiContainer::s_SET_ENABLED_BY_PARAM_SLOT = "setEnabledByParam";
const core::com::Slots::SlotKeyType IGuiContainer::s_ENABLE_SLOT               = "enable";
const core::com::Slots::SlotKeyType IGuiContainer::s_DISABLE_SLOT              = "disable";
const core::com::Slots::SlotKeyType IGuiContainer::s_SET_VISIBLE_SLOT          = "setVisible";
const core::com::Slots::SlotKeyType IGuiContainer::s_SET_VISIBLE_BY_PARAM_SLOT = "setVisibleByParam";
const core::com::Slots::SlotKeyType IGuiContainer::s_SHOW_SLOT                 = "show";
const core::com::Slots::SlotKeyType IGuiContainer::s_HIDE_SLOT                 = "hide";

//-----------------------------------------------------------------------------

IGuiContainer::IGuiContainer()
{
    newSlot(s_SET_ENABLED_SLOT, &IGuiContainer::setEnabled, this);
    newSlot(s_SET_ENABLED_BY_PARAM_SLOT, &IGuiContainer::setEnabledByParameter, this);
    newSlot(s_ENABLE_SLOT, &IGuiContainer::enable, this);
    newSlot(s_DISABLE_SLOT, &IGuiContainer::disable, this);
    newSlot(s_SET_VISIBLE_SLOT, &IGuiContainer::setVisible, this);
    newSlot(s_SET_VISIBLE_BY_PARAM_SLOT, &IGuiContainer::setVisibleByParameter, this);
    newSlot(s_SHOW_SLOT, &IGuiContainer::show, this);
    newSlot(s_HIDE_SLOT, &IGuiContainer::hide, this);
}

//-----------------------------------------------------------------------------

void IGuiContainer::initialize()
{
    // Create view registry
    m_viewRegistry = ui::base::registry::View::New(this->getID());

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
            "[" + this->getID() + "' ] No <registry> tag is allowed in the <gui> section",
            !gui->get_child_optional("registry").has_value()
        );

        // find view LayoutManager configuration
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

void IGuiContainer::create()
{
    SIGHT_ASSERT(
        "[" + this->getID() + "'] View must be initialized, don't forget to call 'initialize()' in "
                              "'configuring()' method.",
        m_viewRegistry
    );
    ui::base::container::fwContainer::sptr parent = m_viewRegistry->getParent();
    SIGHT_ASSERT("Parent container is unknown.", parent);

    core::thread::getDefaultWorker()->postTask<void>(
        [this, &parent]
        {
            SIGHT_ASSERT("View must be initialized.", m_viewRegistry);

            ui::base::GuiBaseObject::sptr guiObj =
                ui::base::factory::New(ui::base::builder::IContainerBuilder::REGISTRY_KEY);
            m_containerBuilder = ui::base::builder::IContainerBuilder::dynamicCast(guiObj);

            SIGHT_ASSERT(
                "Cannot create main container: factory failed for '"
                + ui::base::builder::IContainerBuilder::REGISTRY_KEY + "'",
                m_containerBuilder
            );
            m_containerBuilder->createContainer(parent);

            ui::base::container::fwContainer::sptr container = m_containerBuilder->getContainer();

            if(m_viewLayoutManagerIsCreated)
            {
                if(m_hasToolBar)
                {
                    core::thread::getDefaultWorker()->postTask<void>(
                        [&]
                    {
                        m_toolBarBuilder->createToolBar(parent);
                    }).wait();

                    m_viewRegistry->manageToolBar(m_toolBarBuilder->getToolBar());
                }

                const std::string serviceID = getID().substr(getID().find_last_of('_') + 1);

                core::thread::getDefaultWorker()->postTask<void>(
                    [&]
                {
                    m_viewLayoutManager->createLayout(container, serviceID);
                }).wait();

                std::vector<ui::base::container::fwContainer::sptr> views = m_viewLayoutManager->getSubViews();

                for(const auto& slideBuilder : m_slideViewBuilders)
                {
                    SIGHT_ASSERT("Slide builder is not instantiated", slideBuilder);
                    core::thread::getDefaultWorker()->postTask<void>(
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

void IGuiContainer::destroy()
{
    SIGHT_ASSERT("View must be initialized.", m_viewRegistry);

    m_viewRegistry->unmanage();

    for(const auto& slideBuilder : m_slideViewBuilders)
    {
        SIGHT_ASSERT("Slide builder is not instantiated", slideBuilder);
        core::thread::getDefaultWorker()->postTask<void>(
            [&]
            {
                slideBuilder->destroyContainer();
            }).wait();
    }

    if(m_viewLayoutManagerIsCreated)
    {
        SIGHT_ASSERT("ViewLayoutManager must be initialized.", m_viewLayoutManager);

        core::thread::getDefaultWorker()->postTask<void>(
            [&]
            {
                m_viewLayoutManager->destroyLayout();
            }).wait();

        if(m_hasToolBar)
        {
            m_viewRegistry->unmanageToolBar();
            SIGHT_ASSERT("ToolBarBuilder must be initialized.", m_toolBarBuilder);

            core::thread::getDefaultWorker()->postTask<void>(
                [&]
                {
                    m_toolBarBuilder->destroyToolBar();
                }).wait();
        }
    }

    m_containerBuilder->destroyContainer();
}

//-----------------------------------------------------------------------------

void IGuiContainer::initializeLayoutManager(const ui::base::config_t& layoutConfig)
{
    const auto layoutManagerClassName = layoutConfig.get<std::string>("<xmlattr>.type");

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(layoutManagerClassName);
    m_viewLayoutManager = ui::base::layoutManager::IViewLayoutManager::dynamicCast(guiObj);
    SIGHT_ASSERT("ClassFactoryRegistry failed for class " + layoutManagerClassName, m_viewLayoutManager);

    m_viewLayoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

void IGuiContainer::initializeToolBarBuilder(const ui::base::config_t& toolBarConfig)
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ui::base::builder::IToolBarBuilder::REGISTRY_KEY);
    m_toolBarBuilder = ui::base::builder::IToolBarBuilder::dynamicCast(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " + ui::base::builder::IToolBarBuilder::REGISTRY_KEY,
        m_toolBarBuilder
    );

    m_toolBarBuilder->initialize(toolBarConfig);
}

//-----------------------------------------------------------------------------

void IGuiContainer::initializeSlideViewBuilder(const ui::base::config_t& slideViewConfig)
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
        ui::base::builder::ISlideViewBuilder::REGISTRY_KEY
    );
    ui::base::builder::ISlideViewBuilder::sptr slideViewBuildfer = ui::base::builder::ISlideViewBuilder::dynamicCast(
        guiObj
    );
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " + ui::base::builder::ISlideViewBuilder::REGISTRY_KEY,
        slideViewBuildfer
    );

    slideViewBuildfer->initialize(slideViewConfig);

    m_slideViewBuilders.push_back(slideViewBuildfer);
}

//-----------------------------------------------------------------------------

ui::base::container::fwContainer::sptr IGuiContainer::getContainer()
{
    return m_containerBuilder->getContainer();
}

//-----------------------------------------------------------------------------

void IGuiContainer::setParent(std::string wid)
{
    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [this, &wid]
        {
            m_viewRegistry->setParent(wid);
            ui::base::container::fwContainer::sptr parent = m_viewRegistry->getParent();
            SIGHT_ASSERT("Parent container is unknown.", parent);
            m_containerBuilder->setParent(parent);
        })
    );
}

//-----------------------------------------------------------------------------

void IGuiContainer::setEnabled(bool isEnabled)
{
    ui::base::container::fwContainer::sptr container = m_viewRegistry->getParent();
    container->setEnabled(isEnabled);
}

//-----------------------------------------------------------------------------

void IGuiContainer::setEnabledByParameter(ui::base::parameter_t isEnabled)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(isEnabled))
    {
        this->setEnabled(std::get<bool>(isEnabled));
    }
}

//-----------------------------------------------------------------------------

void IGuiContainer::enable()
{
    this->setEnabled(true);
}

//-----------------------------------------------------------------------------

void IGuiContainer::disable()
{
    this->setEnabled(false);
}

//-----------------------------------------------------------------------------

void IGuiContainer::setVisible(bool isVisible)
{
    ui::base::container::fwContainer::sptr container = m_viewRegistry->getParent();
    container->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void IGuiContainer::setVisibleByParameter(ui::base::parameter_t isVisible)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(isVisible))
    {
        this->setVisible(std::get<bool>(isVisible));
    }
}

//-----------------------------------------------------------------------------

void IGuiContainer::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void IGuiContainer::hide()
{
    this->setVisible(false);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base
