/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/base.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/thread/ActiveWorkers.hpp>
#include <core/tools/fwID.hpp>

#include <service/macros.hpp>

namespace sight::ui::base
{

const core::com::Slots::SlotKeyType IGuiContainer::s_SET_ENABLED_SLOT = "setEnabled";
const core::com::Slots::SlotKeyType IGuiContainer::s_ENABLE_SLOT      = "enable";
const core::com::Slots::SlotKeyType IGuiContainer::s_DISABLE_SLOT     = "disable";
const core::com::Slots::SlotKeyType IGuiContainer::s_SET_VISIBLE_SLOT = "setVisible";
const core::com::Slots::SlotKeyType IGuiContainer::s_SHOW_SLOT        = "show";
const core::com::Slots::SlotKeyType IGuiContainer::s_HIDE_SLOT        = "hide";

//-----------------------------------------------------------------------------

IGuiContainer::IGuiContainer() :
    m_viewLayoutManagerIsCreated(false),
    m_hasToolBar(false)
{
    newSlot(s_SET_ENABLED_SLOT, &IGuiContainer::setEnabled, this);
    newSlot(s_ENABLE_SLOT, &IGuiContainer::enable, this);
    newSlot(s_DISABLE_SLOT, &IGuiContainer::disable, this);
    newSlot(s_SET_VISIBLE_SLOT, &IGuiContainer::setVisible, this);
    newSlot(s_SHOW_SLOT, &IGuiContainer::show, this);
    newSlot(s_HIDE_SLOT, &IGuiContainer::hide, this);
}

//-----------------------------------------------------------------------------

IGuiContainer::~IGuiContainer()
{
}

//-----------------------------------------------------------------------------

void IGuiContainer::initialize()
{
    SIGHT_ASSERT("The service '" + this->getID() + "' does not contain a configuration", m_configuration);

    // Create view registry
    m_viewRegistry = ui::base::registry::View::New(this->getID());
    // find View configuration
    std::vector<ConfigurationType> vectViewMng = m_configuration->find("registry");
    if(!vectViewMng.empty())
    {
        m_viewRegistryConfig = vectViewMng.at(0);
        m_viewRegistry->initialize(m_viewRegistryConfig);
    }

    // Create initializeLayoutManager
    // find gui configuration
    std::vector<ConfigurationType> vectGui = m_configuration->find("gui");
    if(!vectGui.empty())
    {
        SIGHT_ASSERT(
            "[" + this->getID() + "' ] No <registry> tag is allowed in the <gui> section",
            vectGui.at(0)->find("registry").empty()
        );

        // find view LayoutManager configuration
        std::vector<ConfigurationType> vectLayoutMng = vectGui.at(0)->find("layout");
        if(!vectLayoutMng.empty())
        {
            m_viewLayoutConfig = vectLayoutMng.at(0);
            this->initializeLayoutManager(m_viewLayoutConfig);
            m_viewLayoutManagerIsCreated = true;
        }

        // find toolBarBuilder configuration
        std::vector<ConfigurationType> vectTBBuilder = vectGui.at(0)->find("toolBar");
        if(!vectTBBuilder.empty())
        {
            m_toolBarConfig = vectTBBuilder.at(0);
            this->initializeToolBarBuilder(m_toolBarConfig);

            m_hasToolBar = true;
        }

        // find slideView configuration
        std::vector<ConfigurationType> vectSlideCfg = vectGui.at(0)->find("slideView");
        for(const auto& slideCfg : vectSlideCfg)
        {
            this->initializeSlideViewBuilder(slideCfg);
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

    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
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
                    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
                        [&]
                    {
                        m_toolBarBuilder->createToolBar(parent);
                    }).wait();

                    m_viewRegistry->manageToolBar(m_toolBarBuilder->getToolBar());
                }

                core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
                    [&]
                {
                    m_viewLayoutManager->createLayout(container);
                }).wait();

                std::vector<ui::base::container::fwContainer::sptr> views = m_viewLayoutManager->getSubViews();

                for(const auto& slideBuilder : m_slideViewBuilders)
                {
                    SIGHT_ASSERT("Slide builder is not instantiated", slideBuilder);
                    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
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

    if(m_viewLayoutManagerIsCreated)
    {
        if(m_hasToolBar)
        {
            m_viewRegistry->unmanageToolBar();
            SIGHT_ASSERT("ToolBarBuilder must be initialized.", m_toolBarBuilder);

            core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
                [&]
                {
                    m_toolBarBuilder->destroyToolBar();
                }).wait();
        }

        m_viewRegistry->unmanage();
        SIGHT_ASSERT("ViewLayoutManager must be initialized.", m_viewLayoutManager);

        core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
            [&]
            {
                m_viewLayoutManager->destroyLayout();
            }).wait();
    }

    for(const auto& slideBuilder : m_slideViewBuilders)
    {
        SIGHT_ASSERT("Slide builder is not instantiated", slideBuilder);
        core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
            [&]
            {
                slideBuilder->destroyContainer();
            }).wait();
    }

    m_containerBuilder->destroyContainer();
}

//-----------------------------------------------------------------------------

void IGuiContainer::initializeLayoutManager(ConfigurationType layoutConfig)
{
    SIGHT_ASSERT(
        "[" + this->getID() + "' ] Wrong configuration name, expected: 'layout', actual: '"
        + layoutConfig->getName() + "'",
        layoutConfig->getName() == "layout"
    );
    SIGHT_ASSERT("<layout> tag must have type attribute", layoutConfig->hasAttribute("type"));
    const std::string layoutManagerClassName = layoutConfig->getAttributeValue("type");

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(layoutManagerClassName);
    m_viewLayoutManager = ui::base::layoutManager::IViewLayoutManager::dynamicCast(guiObj);
    SIGHT_ASSERT("ClassFactoryRegistry failed for class " + layoutManagerClassName, m_viewLayoutManager);

    m_viewLayoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

void IGuiContainer::initializeToolBarBuilder(ConfigurationType toolBarConfig)
{
    SIGHT_ASSERT(
        "[" + this->getID() + "' ] Wrong configuration name, expected: 'toolBar', actual: '"
        + toolBarConfig->getName() + "'",
        toolBarConfig->getName() == "toolBar"
    );

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ui::base::builder::IToolBarBuilder::REGISTRY_KEY);
    m_toolBarBuilder = ui::base::builder::IToolBarBuilder::dynamicCast(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " + ui::base::builder::IToolBarBuilder::REGISTRY_KEY,
        m_toolBarBuilder
    );

    m_toolBarBuilder->initialize(toolBarConfig);
}

//-----------------------------------------------------------------------------

void IGuiContainer::initializeSlideViewBuilder(ConfigurationType slideViewConfig)
{
    SIGHT_ASSERT(
        "[" + this->getID() + "' ] Wrong configuration name, expected: 'slideView', actual: '"
        + slideViewConfig->getName() + "'",
        slideViewConfig->getName() == "slideView"
    );

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
    core::thread::ActiveWorkers::getDefaultWorker()->postTask<void>(
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

} // namespace sight::ui
