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
#include "ui/base/registrar/ViewRegistrar.hpp"

#include <core/base.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/tools/fwID.hpp>

#include <service/macros.hpp>
#include <service/registry/ActiveWorkers.hpp>

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
    SLM_ASSERT("The service '" + this->getID() + "' does not contain a configuration", m_configuration);

    // Create view registrar
    m_viewRegistrar = ui::base::registrar::ViewRegistrar::New( this->getID() );
    // find ViewRegistrar configuration
    std::vector < ConfigurationType > vectViewMng = m_configuration->find("registry");
    if ( !vectViewMng.empty() )
    {
        m_viewRegistrarConfig = vectViewMng.at(0);
        m_viewRegistrar->initialize(m_viewRegistrarConfig);
    }

    // Create initializeLayoutManager
    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    if( !vectGui.empty() )
    {
        SLM_ASSERT("["+this->getID()+"' ] No <registry> tag is allowed in the <gui> section",
                   vectGui.at(0)->find("registry").empty());

        // find view LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        if( !vectLayoutMng.empty() )
        {
            m_viewLayoutConfig = vectLayoutMng.at(0);
            this->initializeLayoutManager( m_viewLayoutConfig );
            m_viewLayoutManagerIsCreated = true;
        }

        // find toolBarBuilder configuration
        std::vector < ConfigurationType > vectTBBuilder = vectGui.at(0)->find("toolBar");
        if(!vectTBBuilder.empty())
        {
            m_toolBarConfig = vectTBBuilder.at(0);
            this->initializeToolBarBuilder(m_toolBarConfig);

            m_hasToolBar = true;
        }

        // find slideView configuration
        std::vector < ConfigurationType > vectSlideCfg = vectGui.at(0)->find("slideView");
        for (const auto& slideCfg : vectSlideCfg)
        {
            this->initializeSlideViewBuilder(slideCfg);
        }
    }
}

//-----------------------------------------------------------------------------

void IGuiContainer::create()
{
    SLM_ASSERT("["+this->getID()+"'] ViewRegistrar must be initialized, don't forget to call 'initialize()' in "
               "'configuring()' method.", m_viewRegistrar);
    ui::base::container::fwContainer::sptr parent = m_viewRegistrar->getParent();
    SLM_ASSERT("Parent container is unknown.", parent);

    service::registry::ActiveWorkers::getDefaultWorker()->postTask< void >([this, &parent]
        {
            SLM_ASSERT("ViewRegistrar must be initialized.", m_viewRegistrar);

            ui::base::GuiBaseObject::sptr guiObj =
                ui::base::factory::New(ui::base::builder::IContainerBuilder::REGISTRY_KEY);
            m_containerBuilder = ui::base::builder::IContainerBuilder::dynamicCast(guiObj);

            SLM_ASSERT("Cannot create main container: factory failed for '"
                       + ui::base::builder::IContainerBuilder::REGISTRY_KEY + "'",
                       m_containerBuilder);
            m_containerBuilder->createContainer(parent);

            ui::base::container::fwContainer::sptr container = m_containerBuilder->getContainer();

            if ( m_viewLayoutManagerIsCreated )
            {
                if (m_hasToolBar)
                {
                    service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>([&]
                    {
                        m_toolBarBuilder->createToolBar(parent);
                    }).wait();

                    m_viewRegistrar->manageToolBar(m_toolBarBuilder->getToolBar());
                }

                service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>([&]
                {
                    m_viewLayoutManager->createLayout(container);
                }).wait();

                std::vector< ui::base::container::fwContainer::sptr > views = m_viewLayoutManager->getSubViews();

                for (const auto& slideBuilder : m_slideViewBuilders)
                {
                    SLM_ASSERT("Slide builder is not instantiated", slideBuilder);
                    service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>([&]
                    {
                        slideBuilder->createContainer(container);
                    }).wait();
                    views.push_back(slideBuilder->getContainer());
                }

                m_viewRegistrar->manage(views);
            }
        }).wait();
}

//-----------------------------------------------------------------------------

void IGuiContainer::destroy()
{
    SLM_ASSERT("ViewRegistrar must be initialized.", m_viewRegistrar);

    if ( m_viewLayoutManagerIsCreated )
    {
        if (m_hasToolBar)
        {
            m_viewRegistrar->unmanageToolBar();
            SLM_ASSERT("ToolBarBuilder must be initialized.", m_toolBarBuilder);

            service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>([&]
                {
                    m_toolBarBuilder->destroyToolBar();
                }).wait();
        }

        m_viewRegistrar->unmanage();
        SLM_ASSERT("ViewLayoutManager must be initialized.", m_viewLayoutManager);

        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>([&]
            {
                m_viewLayoutManager->destroyLayout();
            }).wait();
    }

    for (const auto& slideBuilder : m_slideViewBuilders)
    {
        SLM_ASSERT("Slide builder is not instantiated", slideBuilder);
        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>([&]
            {
                slideBuilder->destroyContainer();
            }).wait();
    }

    m_containerBuilder->destroyContainer();
}

//-----------------------------------------------------------------------------

void IGuiContainer::initializeLayoutManager(ConfigurationType layoutConfig)
{
    SLM_ASSERT("["+this->getID()+"' ] Wrong configuration name, expected: 'layout', actual: '"
               +layoutConfig->getName()+ "'",
               layoutConfig->getName() == "layout");
    SLM_ASSERT("<layout> tag must have type attribute", layoutConfig->hasAttribute("type"));
    const std::string layoutManagerClassName = layoutConfig->getAttributeValue("type");

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(layoutManagerClassName);
    m_viewLayoutManager = ui::base::layoutManager::IViewLayoutManager::dynamicCast(guiObj);
    SLM_ASSERT("ClassFactoryRegistry failed for class "+ layoutManagerClassName, m_viewLayoutManager);

    m_viewLayoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

void IGuiContainer::initializeToolBarBuilder(ConfigurationType toolBarConfig)
{
    SLM_ASSERT("["+this->getID()+"' ] Wrong configuration name, expected: 'toolBar', actual: '"
               +toolBarConfig->getName()+ "'",
               toolBarConfig->getName() == "toolBar");

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ui::base::builder::IToolBarBuilder::REGISTRY_KEY);
    m_toolBarBuilder = ui::base::builder::IToolBarBuilder::dynamicCast(guiObj);
    SLM_ASSERT("ClassFactoryRegistry failed for class "+ ui::base::builder::IToolBarBuilder::REGISTRY_KEY,
               m_toolBarBuilder);

    m_toolBarBuilder->initialize(toolBarConfig);
}

//-----------------------------------------------------------------------------

void IGuiContainer::initializeSlideViewBuilder(ConfigurationType slideViewConfig)
{
    SLM_ASSERT("["+this->getID()+"' ] Wrong configuration name, expected: 'slideView', actual: '"
               +slideViewConfig->getName()+ "'",
               slideViewConfig->getName() == "slideView");

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
        ui::base::builder::ISlideViewBuilder::REGISTRY_KEY);
    ui::base::builder::ISlideViewBuilder::sptr slideViewBuildfer = ui::base::builder::ISlideViewBuilder::dynamicCast(
        guiObj);
    SLM_ASSERT("ClassFactoryRegistry failed for class "+ ui::base::builder::ISlideViewBuilder::REGISTRY_KEY,
               slideViewBuildfer);

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
    service::registry::ActiveWorkers::getDefaultWorker()->postTask< void >(std::function< void() >([this, &wid]
        {
            m_viewRegistrar->setParent(wid);
            ui::base::container::fwContainer::sptr parent = m_viewRegistrar->getParent();
            SLM_ASSERT("Parent container is unknown.", parent);
            m_containerBuilder->setParent(parent);
        } ));
}

//-----------------------------------------------------------------------------

void IGuiContainer::setEnabled(bool isEnabled)
{
    ui::base::container::fwContainer::sptr container = m_viewRegistrar->getParent();
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
    ui::base::container::fwContainer::sptr container = m_viewRegistrar->getParent();
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

}
