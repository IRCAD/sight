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

#include "ui/base/IFrame.hpp"

#include "ui/base/Application.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>
#include <core/tools/fwID.hpp>

#include <service/macros.hpp>

#include <boost/lambda/lambda.hpp>

namespace sight::ui::base
{

const std::string IFrame::CLOSE_POLICY_EXIT    = "exit";
const std::string IFrame::CLOSE_POLICY_NOTIFY  = "notify";
const std::string IFrame::CLOSE_POLICY_MESSAGE = "message";

const core::com::Slots::SlotKeyType IFrame::s_SET_VISIBLE_SLOT = "setVisible";
const core::com::Slots::SlotKeyType IFrame::s_SHOW_SLOT        = "show";
const core::com::Slots::SlotKeyType IFrame::s_HIDE_SLOT        = "hide";

const core::com::Signals::SignalKeyType IFrame::s_CLOSED_SIG = "closed";

ui::base::container::fwContainer::wptr IFrame::m_progressWidget = std::weak_ptr<ui::base::container::fwContainer>();

//-----------------------------------------------------------------------------

IFrame::IFrame() :

    m_closePolicy("exit")
{
    m_sigClosed = newSignal<ClosedSignalType>(s_CLOSED_SIG);

    newSlot(s_SET_VISIBLE_SLOT, &IFrame::setVisible, this);
    newSlot(s_SHOW_SLOT, &IFrame::show, this);
    newSlot(s_HIDE_SLOT, &IFrame::hide, this);
}

//-----------------------------------------------------------------------------

void IFrame::initialize()
{
    m_viewRegistry = ui::base::registry::View::New(this->getID());

    const auto config = this->getConfiguration();
    const auto gui    = config.get_child_optional("gui");

    if(gui.has_value())
    {
        this->initializeLayoutManager(gui->get_child("frame"));

        if(const auto menuBar = gui->get_child_optional("menuBar"); menuBar.has_value())
        {
            this->initializeMenuBarBuilder(menuBar.value());

            m_hasMenuBar = true;
        }

        if(const auto toolBar = gui->get_child_optional("toolBar"); toolBar.has_value())
        {
            this->initializeToolBarBuilder(toolBar.value());

            m_hasToolBar = true;
        }

        // find ViewRegistryManager configuration
        if(const auto registry = config.get_child_optional("registry"); registry.has_value())
        {
            m_viewRegistry->initialize(registry.value());
        }
    }

    if(const auto onclose = config.get<std::string>("window.<xmlattr>.onclose", ""); !onclose.empty())
    {
        m_closePolicy = onclose;
        SIGHT_ASSERT(
            "[" + this->getID() + "' ] Invalid onclose value, actual: '" << m_closePolicy << "', accepted: '"
            + CLOSE_POLICY_NOTIFY + "', '" + CLOSE_POLICY_EXIT + "' or '" + CLOSE_POLICY_MESSAGE + "'",
            m_closePolicy == CLOSE_POLICY_NOTIFY || m_closePolicy == CLOSE_POLICY_EXIT
            || m_closePolicy == CLOSE_POLICY_MESSAGE
        );
    }
}

//-----------------------------------------------------------------------------

void IFrame::create()
{
    SIGHT_ASSERT(
        "[" + this->getID() + "' ] FrameLayoutManager must be initialized, don't forget to call 'initialize()' in "
                              "'configuring()' method.",
        m_frameLayoutManager
    );

    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            m_frameLayoutManager->createFrame();
        })
    ).wait();

    ui::base::container::fwContainer::sptr frame = m_frameLayoutManager->getFrame();
    if(m_progressWidget.expired())
    {
        m_progressWidget = frame;
    }

    ui::base::container::fwContainer::sptr container = m_frameLayoutManager->getContainer();
    std::vector<ui::base::container::fwContainer::sptr> subViews;
    subViews.push_back(container);
    m_viewRegistry->manage(subViews);

    ui::base::layoutManager::IFrameLayoutManager::CloseCallback fct;

    if(m_closePolicy == CLOSE_POLICY_EXIT)
    {
        fct = onCloseExit;
    }
    else if(m_closePolicy == CLOSE_POLICY_NOTIFY)
    {
        fct = [this](auto&& ...){onCloseNotify();};
    }
    else if(m_closePolicy == CLOSE_POLICY_MESSAGE)
    {
        fct = onCloseMessage;
        auto app = ui::base::Application::New();
        app->setConfirm(true);
    }

    m_frameLayoutManager->setCloseCallback(fct);

    if(m_hasMenuBar)
    {
        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_menuBarBuilder->createMenuBar(frame);
            })
        ).wait();

        m_viewRegistry->manageMenuBar(m_menuBarBuilder->getMenuBar());
    }

    if(m_hasToolBar)
    {
        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_toolBarBuilder->createToolBar(frame);
            })
        ).wait();

        m_viewRegistry->manageToolBar(m_toolBarBuilder->getToolBar());
    }
}

//-----------------------------------------------------------------------------

void IFrame::destroy()
{
    SIGHT_ASSERT("View must be initialized.", m_viewRegistry);

    if(m_hasToolBar)
    {
        m_viewRegistry->unmanageToolBar();
        SIGHT_ASSERT("ToolBarBuilder must be initialized.", m_toolBarBuilder);

        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_toolBarBuilder->destroyToolBar();
            })
        ).wait();
    }

    if(m_hasMenuBar)
    {
        m_viewRegistry->unmanageMenuBar();
        SIGHT_ASSERT("MenuBarBuilder must be initialized.", m_menuBarBuilder);

        core::thread::getDefaultWorker()->postTask<void>(
            std::function<void()>(
                [&]
            {
                m_menuBarBuilder->destroyMenuBar();
            })
        ).wait();
    }

    m_viewRegistry->unmanage();
    SIGHT_ASSERT("FrameLayoutManager must be initialized.", m_frameLayoutManager);

    core::thread::getDefaultWorker()->postTask<void>(
        std::function<void()>(
            [&]
        {
            m_frameLayoutManager->destroyFrame();
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void IFrame::initializeLayoutManager(const ui::base::config_t& layoutConfig)
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
        ui::base::layoutManager::IFrameLayoutManager::REGISTRY_KEY
    );
    m_frameLayoutManager = ui::base::layoutManager::IFrameLayoutManager::dynamicCast(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::base::layoutManager::IFrameLayoutManager::REGISTRY_KEY,
        m_frameLayoutManager
    );

    m_frameLayoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

void IFrame::initializeMenuBarBuilder(const ui::base::config_t& menuBarConfig)
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ui::base::builder::IMenuBarBuilder::REGISTRY_KEY);
    m_menuBarBuilder = ui::base::builder::IMenuBarBuilder::dynamicCast(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::base::builder::IMenuBarBuilder::REGISTRY_KEY,
        m_menuBarBuilder
    );

    m_menuBarBuilder->initialize(menuBarConfig);
}

//-----------------------------------------------------------------------------

void IFrame::initializeToolBarBuilder(const ui::base::config_t& toolBarConfig)
{
    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(ui::base::builder::IToolBarBuilder::REGISTRY_KEY);
    m_toolBarBuilder = ui::base::builder::IToolBarBuilder::dynamicCast(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::base::builder::IToolBarBuilder::REGISTRY_KEY,
        m_toolBarBuilder
    );

    m_toolBarBuilder->initialize(toolBarConfig);
}

//-----------------------------------------------------------------------------

void IFrame::onCloseExit()
{
    ui::base::Application::New()->exit(0);
}

//-----------------------------------------------------------------------------

void IFrame::onCloseNotify()
{
    m_sigClosed->asyncEmit();
}

//-----------------------------------------------------------------------------

void IFrame::onCloseMessage()
{
    auto app = ui::base::Application::New();
    app->exit(0);
}

//-----------------------------------------------------------------------------

ui::base::container::fwContainer::sptr IFrame::getProgressWidget()
{
    return m_progressWidget.lock();
}

//-----------------------------------------------------------------------------

void IFrame::setVisible(bool isVisible)
{
    ui::base::container::fwContainer::sptr container = m_frameLayoutManager->getFrame();
    container->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void IFrame::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void IFrame::hide()
{
    this->setVisible(false);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::base
