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

#include "fwGui/IFrameSrv.hpp"

#include "fwGui/Application.hpp"

#include <core/include/core/thread/Worker.hpp>
#include <core/include/core/thread/Worker.hxx>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwTools/fwID.hpp>

#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>

namespace fwGui
{

const std::string IFrameSrv::CLOSE_POLICY_EXIT    = "exit";
const std::string IFrameSrv::CLOSE_POLICY_NOTIFY  = "notify";
const std::string IFrameSrv::CLOSE_POLICY_MESSAGE = "message";

const ::fwCom::Slots::SlotKeyType IFrameSrv::s_SET_VISIBLE_SLOT = "setVisible";
const ::fwCom::Slots::SlotKeyType IFrameSrv::s_SHOW_SLOT        = "show";
const ::fwCom::Slots::SlotKeyType IFrameSrv::s_HIDE_SLOT        = "hide";

const ::fwCom::Signals::SignalKeyType IFrameSrv::s_CLOSED_SIG = "closed";

::fwGui::container::fwContainer::wptr IFrameSrv::m_progressWidget =
    std::weak_ptr< ::fwGui::container::fwContainer >();

IFrameSrv::IFrameSrv() :
    m_hasMenuBar(false),
    m_hasToolBar(false),
    m_closePolicy("exit")
{
    m_sigClosed = newSignal< ClosedSignalType >(s_CLOSED_SIG);

    newSlot(s_SET_VISIBLE_SLOT, &IFrameSrv::setVisible, this);
    newSlot(s_SHOW_SLOT, &IFrameSrv::show, this);
    newSlot(s_HIDE_SLOT, &IFrameSrv::hide, this);
}

//-----------------------------------------------------------------------------

IFrameSrv::~IFrameSrv()
{
}

//-----------------------------------------------------------------------------

void IFrameSrv::initialize()
{
    // find gui configuration
    std::vector < ConfigurationType > vectGui    = m_configuration->find("gui");
    std::vector < ConfigurationType > vectWindow = m_configuration->find("window");

    if(!vectGui.empty())
    {
        // find LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("frame");
        SLM_ASSERT("["+this->getID()+"' ] <frame> element must exist", !vectLayoutMng.empty());
        m_frameConfig = vectLayoutMng.at(0);
        this->initializeLayoutManager(m_frameConfig);

        // find menuBarBuilder configuration
        std::vector < ConfigurationType > vectMBBuilder = vectGui.at(0)->find("menuBar");
        if(!vectMBBuilder.empty())
        {
            m_menuBarConfig = vectMBBuilder.at(0);
            this->initializeMenuBarBuilder(m_menuBarConfig);

            m_hasMenuBar = true;
        }

        // find toolBarBuilder configuration
        std::vector < ConfigurationType > vectTBBuilder = vectGui.at(0)->find("toolBar");
        if(!vectTBBuilder.empty())
        {
            m_toolBarConfig = vectTBBuilder.at(0);
            this->initializeToolBarBuilder(m_toolBarConfig);

            m_hasToolBar = true;
        }
    }

    if(!vectWindow.empty())
    {
        ConfigurationType window = vectWindow.at(0);
        std::string onclose      = window->getAttributeValue("onclose");
        if ( !onclose.empty() )
        {
            m_closePolicy = onclose;
        }
        SLM_ASSERT("["+this->getID()+"' ] Invalid onclose value, actual: '" << m_closePolicy << "', accepted: '"
                   +CLOSE_POLICY_NOTIFY+"', '" +CLOSE_POLICY_EXIT+"' or '"+CLOSE_POLICY_MESSAGE+"'",
                   m_closePolicy == CLOSE_POLICY_NOTIFY || m_closePolicy == CLOSE_POLICY_EXIT
                   || m_closePolicy == CLOSE_POLICY_MESSAGE);
    }

    m_viewRegistrar = ::fwGui::registrar::ViewRegistrar::New(this->getID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistrar = m_configuration->find("registry");
    if(!vectRegistrar.empty())
    {
        m_registrarConfig = vectRegistrar.at(0);
        m_viewRegistrar->initialize(m_registrarConfig);
    }
}

//-----------------------------------------------------------------------------

void IFrameSrv::create()
{
    SLM_ASSERT("["+this->getID()+"' ] FrameLayoutManager must be initialized, don't forget to call 'initialize()' in "
               "'configuring()' method.", m_frameLayoutManager);

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_frameLayoutManager->createFrame();
        })).wait();

    ::fwGui::container::fwContainer::sptr frame = m_frameLayoutManager->getFrame();
    if ( m_progressWidget.expired() )
    {
        m_progressWidget = frame;
    }

    ::fwGui::container::fwContainer::sptr container = m_frameLayoutManager->getContainer();
    std::vector< ::fwGui::container::fwContainer::sptr > subViews;
    subViews.push_back(container);
    m_viewRegistrar->manage(subViews);

    ::fwGui::layoutManager::IFrameLayoutManager::CloseCallback fct;

    if (m_closePolicy == CLOSE_POLICY_EXIT)
    {
        fct = std::bind( &::fwGui::IFrameSrv::onCloseExit, this);
    }
    else if (m_closePolicy == CLOSE_POLICY_NOTIFY)
    {
        fct = std::bind( &::fwGui::IFrameSrv::onCloseNotify, this);
    }
    else if(m_closePolicy == CLOSE_POLICY_MESSAGE)
    {
        fct = std::bind( &::fwGui::IFrameSrv::onCloseMessage, this);
        auto app = ::fwGui::Application::New();
        app->setConfirm(true);
    }

    m_frameLayoutManager->setCloseCallback(fct);

    if (m_hasMenuBar)
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_menuBarBuilder->createMenuBar(frame);
            })).wait();

        m_viewRegistrar->manageMenuBar(m_menuBarBuilder->getMenuBar());
    }

    if (m_hasToolBar)
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_toolBarBuilder->createToolBar(frame);
            })).wait();

        m_viewRegistrar->manageToolBar(m_toolBarBuilder->getToolBar());
    }
}

//-----------------------------------------------------------------------------

void IFrameSrv::destroy()
{
    SLM_ASSERT("ViewRegistrar must be initialized.", m_viewRegistrar);

    if (m_hasToolBar)
    {
        m_viewRegistrar->unmanageToolBar();
        SLM_ASSERT("ToolBarBuilder must be initialized.", m_toolBarBuilder);

        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_toolBarBuilder->destroyToolBar();
            })).wait();
    }

    if (m_hasMenuBar)
    {
        m_viewRegistrar->unmanageMenuBar();
        SLM_ASSERT("MenuBarBuilder must be initialized.", m_menuBarBuilder);

        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_menuBarBuilder->destroyMenuBar();
            })).wait();
    }

    m_viewRegistrar->unmanage();
    SLM_ASSERT("FrameLayoutManager must be initialized.", m_frameLayoutManager);

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_frameLayoutManager->destroyFrame();
        })).wait();
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeLayoutManager(ConfigurationType frameConfig)
{
    SLM_ASSERT("["+this->getID()+"' ] Wrong configuration name, expected: 'frame', actual: '"
               +frameConfig->getName()+ "'",
               frameConfig->getName() == "frame");
    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(
        ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY);
    m_frameLayoutManager = ::fwGui::layoutManager::IFrameLayoutManager::dynamicCast(guiObj);
    SLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY,
               m_frameLayoutManager);

    m_frameLayoutManager->initialize(frameConfig);
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeMenuBarBuilder(ConfigurationType menuBarConfig)
{
    SLM_ASSERT("["+this->getID()+"' ] Wrong configuration name, expected: 'menuBar', actual: '"
               +menuBarConfig->getName()+ "'",
               menuBarConfig->getName() == "menuBar");

    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(::fwGui::builder::IMenuBarBuilder::REGISTRY_KEY);
    m_menuBarBuilder                    = ::fwGui::builder::IMenuBarBuilder::dynamicCast(guiObj);
    SLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::builder::IMenuBarBuilder::REGISTRY_KEY,
               m_menuBarBuilder);

    m_menuBarBuilder->initialize(menuBarConfig);
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeToolBarBuilder(ConfigurationType toolBarConfig)
{
    SLM_ASSERT("["+this->getID()+"' ] Wrong configuration name, expected: 'toolBar', actual: '"
               +toolBarConfig->getName()+ "'",
               toolBarConfig->getName() == "toolBar");

    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(::fwGui::builder::IToolBarBuilder::REGISTRY_KEY);
    m_toolBarBuilder                    = ::fwGui::builder::IToolBarBuilder::dynamicCast(guiObj);
    SLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::builder::IToolBarBuilder::REGISTRY_KEY,
               m_toolBarBuilder);

    m_toolBarBuilder->initialize(toolBarConfig);
}

//-----------------------------------------------------------------------------

void IFrameSrv::onCloseExit()
{
    ::fwGui::Application::New()->exit(0);
}

//-----------------------------------------------------------------------------

void IFrameSrv::onCloseNotify()
{
    m_sigClosed->asyncEmit();
}

//-----------------------------------------------------------------------------

void IFrameSrv::onCloseMessage()
{
    auto app = ::fwGui::Application::New();
    app->exit(0);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwContainer::sptr IFrameSrv::getProgressWidget()
{
    return m_progressWidget.lock();
}

//-----------------------------------------------------------------------------

void IFrameSrv::setVisible(bool isVisible)
{
    ::fwGui::container::fwContainer::sptr container = m_frameLayoutManager->getFrame();
    container->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void IFrameSrv::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void IFrameSrv::hide()
{
    this->setVisible(false);
}

//-----------------------------------------------------------------------------

}
