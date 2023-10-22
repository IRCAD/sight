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

ui::container::widget::wptr frame::m_progressWidget = std::weak_ptr<ui::container::widget>();

//-----------------------------------------------------------------------------

frame::frame() :

    m_closePolicy("exit")
{
    m_sigClosed = new_signal<closed_signal_t>(CLOSED_SIG);

    new_slot(SET_VISIBLE_SLOT, &frame::setVisible, this);
    new_slot(SHOW_SLOT, &frame::show, this);
    new_slot(HIDE_SLOT, &frame::hide, this);
}

//-----------------------------------------------------------------------------

void frame::initialize()
{
    m_viewRegistry = ui::detail::registry::view::make(this->get_id());

    const auto config = this->get_config();
    const auto gui    = config.get_child_optional("gui");

    if(gui.has_value())
    {
        this->initializeLayoutManager(gui->get_child("frame"));

        if(const auto menu_bar = gui->get_child_optional("menuBar"); menu_bar.has_value())
        {
            this->initializeMenuBarBuilder(menu_bar.value());

            m_hasMenuBar = true;
        }

        if(const auto tool_bar = gui->get_child_optional("toolBar"); tool_bar.has_value())
        {
            this->initializeToolBarBuilder(tool_bar.value());

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
            "[" + this->get_id() + "' ] Invalid onclose value, actual: '" << m_closePolicy << "', accepted: '"
            + CLOSE_POLICY_NOTIFY + "', '" + CLOSE_POLICY_EXIT + "' or '" + CLOSE_POLICY_MESSAGE + "'",
            m_closePolicy == CLOSE_POLICY_NOTIFY || m_closePolicy == CLOSE_POLICY_EXIT
            || m_closePolicy == CLOSE_POLICY_MESSAGE
        );
    }
}

//-----------------------------------------------------------------------------

void frame::create()
{
    SIGHT_ASSERT(
        "[" + this->get_id() + "' ] frame must be initialized, don't forget to call 'initialize()' in "
                               "'configuring()' method.",
        m_frameLayoutManager
    );

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_frameLayoutManager->createFrame();
        })
    ).wait();

    ui::container::widget::sptr frame = m_frameLayoutManager->getFrame();
    if(m_progressWidget.expired())
    {
        m_progressWidget = frame;
    }

    ui::container::widget::sptr container = m_frameLayoutManager->getContainer();
    std::vector<ui::container::widget::sptr> sub_views;
    sub_views.push_back(container);
    m_viewRegistry->manage(sub_views);

    ui::layout::frame_manager::CloseCallback fct;

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
        auto app = sight::ui::application::get();
        app->setConfirm(true);
    }

    m_frameLayoutManager->setCloseCallback(fct);

    if(m_hasMenuBar)
    {
        core::thread::get_default_worker()->post_task<void>(
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
        core::thread::get_default_worker()->post_task<void>(
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

void frame::destroy()
{
    SIGHT_ASSERT("view must be initialized.", m_viewRegistry);

    if(m_hasToolBar)
    {
        m_viewRegistry->unmanageToolBar();
        SIGHT_ASSERT("toolbar must be initialized.", m_toolBarBuilder);

        core::thread::get_default_worker()->post_task<void>(
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
        SIGHT_ASSERT("menubar must be initialized.", m_menuBarBuilder);

        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_menuBarBuilder->destroyMenuBar();
            })
        ).wait();
    }

    m_viewRegistry->unmanage();
    SIGHT_ASSERT("frame must be initialized.", m_frameLayoutManager);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_frameLayoutManager->destroyFrame();
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void frame::initializeLayoutManager(const ui::config_t& _layout_config)
{
    ui::object::sptr gui_obj = ui::factory::make(
        ui::layout::frame_manager::REGISTRY_KEY
    );
    m_frameLayoutManager = std::dynamic_pointer_cast<ui::layout::frame_manager>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::layout::frame_manager::REGISTRY_KEY,
        m_frameLayoutManager
    );

    m_frameLayoutManager->initialize(_layout_config);
}

//-----------------------------------------------------------------------------

void frame::initializeMenuBarBuilder(const ui::config_t& _menu_bar_config)
{
    ui::object::sptr gui_obj = ui::factory::make(ui::builder::menubar::REGISTRY_KEY);
    m_menuBarBuilder = std::dynamic_pointer_cast<ui::builder::menubar>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::builder::menubar::REGISTRY_KEY,
        m_menuBarBuilder
    );

    m_menuBarBuilder->initialize(_menu_bar_config);
}

//-----------------------------------------------------------------------------

void frame::initializeToolBarBuilder(const ui::config_t& _tool_bar_config)
{
    ui::object::sptr gui_obj = ui::factory::make(ui::builder::toolbar::REGISTRY_KEY);
    m_toolBarBuilder = std::dynamic_pointer_cast<ui::builder::toolbar>(gui_obj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::builder::toolbar::REGISTRY_KEY,
        m_toolBarBuilder
    );

    m_toolBarBuilder->initialize(_tool_bar_config);
}

//-----------------------------------------------------------------------------

void frame::onCloseExit()
{
    sight::ui::application::get()->exit(0);
}

//-----------------------------------------------------------------------------

void frame::onCloseNotify()
{
    m_sigClosed->async_emit();
}

//-----------------------------------------------------------------------------

void frame::onCloseMessage()
{
    auto app = sight::ui::application::get();
    app->exit(0);
}

//-----------------------------------------------------------------------------

ui::container::widget::sptr frame::getProgressWidget()
{
    return m_progressWidget.lock();
}

//-----------------------------------------------------------------------------

void frame::setVisible(bool _is_visible)
{
    ui::container::widget::sptr container = m_frameLayoutManager->getFrame();
    container->setVisible(_is_visible);
}

//-----------------------------------------------------------------------------

void frame::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void frame::hide()
{
    this->setVisible(false);
}

//-----------------------------------------------------------------------------

} // namespace sight::ui
