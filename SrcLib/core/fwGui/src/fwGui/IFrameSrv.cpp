/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGui/Application.hpp"
#include "fwGui/IFrameSrv.hpp"

#include <fwCore/base.hpp>
#include <fwTools/fwID.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

namespace fwGui
{

const std::string IFrameSrv::CLOSE_POLICY_EXIT    = "exit";
const std::string IFrameSrv::CLOSE_POLICY_NOTIFY  = "notify";
const std::string IFrameSrv::CLOSE_POLICY_MESSAGE = "message";

::fwGui::container::fwContainer::wptr IFrameSrv::m_progressWidget =
    std::weak_ptr< ::fwGui::container::fwContainer >();

IFrameSrv::IFrameSrv() :
    m_hasMenuBar(false),
    m_hasToolBar(false),
    m_closePolicy("exit")
{
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
        SLM_ASSERT("<frame> xml element must exist", !vectLayoutMng.empty());
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
        SLM_ASSERT("Invalid onclose value : " << m_closePolicy << ". Should be 'exit', 'notify' or 'message'",
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
    SLM_ASSERT("FrameLayoutManager must be initialized.",m_frameLayoutManager);
    m_frameLayoutManager->createFrame();
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
        fct = ::boost::bind( &::fwGui::IFrameSrv::onCloseExit, this);
    }
    else if (m_closePolicy == CLOSE_POLICY_NOTIFY)
    {
        fct = ::boost::bind( &::fwGui::IFrameSrv::onCloseNotify, this);
    }
    else if(m_closePolicy == CLOSE_POLICY_MESSAGE)
    {
        fct = ::boost::bind( &::fwGui::IFrameSrv::onCloseMessage, this);
        auto app = ::fwGui::Application::New();
        app->setConfirm(true);
    }

    m_frameLayoutManager->setCloseCallback(fct);

    if (m_hasMenuBar)
    {
        m_menuBarBuilder->createMenuBar(frame);
        m_viewRegistrar->manageMenuBar(m_menuBarBuilder->getMenuBar());
    }

    if (m_hasToolBar)
    {
        m_toolBarBuilder->createToolBar(frame);
        m_viewRegistrar->manageToolBar(m_toolBarBuilder->getToolBar());
    }
}

//-----------------------------------------------------------------------------

void IFrameSrv::destroy()
{
    SLM_ASSERT("ViewRegistrar must be initialized.",m_viewRegistrar);

    if (m_hasToolBar)
    {
        m_viewRegistrar->unmanageToolBar();
        SLM_ASSERT("ToolBarBuilder must be initialized.",m_toolBarBuilder);
        m_toolBarBuilder->destroyToolBar();
    }

    if (m_hasMenuBar)
    {
        m_viewRegistrar->unmanageMenuBar();
        SLM_ASSERT("MenuBarBuilder must be initialized.",m_menuBarBuilder);
        m_menuBarBuilder->destroyMenuBar();
    }

    m_viewRegistrar->unmanage();
    SLM_ASSERT("FrameLayoutManager must be initialized.",m_frameLayoutManager);
    m_frameLayoutManager->destroyFrame();
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeLayoutManager(ConfigurationType frameConfig)
{
    OSLM_ASSERT("Bad configuration name "<<frameConfig->getName()<< ", must be frame",
                frameConfig->getName() == "frame");
    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(
        ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY);
    m_frameLayoutManager = ::fwGui::layoutManager::IFrameLayoutManager::dynamicCast(guiObj);
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY,
                m_frameLayoutManager);

    m_frameLayoutManager->initialize(frameConfig);
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeMenuBarBuilder(ConfigurationType menuBarConfig)
{
    OSLM_ASSERT("Bad configuration name "<<menuBarConfig->getName()<< ", must be menuBar",
                menuBarConfig->getName() == "menuBar");

    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(::fwGui::builder::IMenuBarBuilder::REGISTRY_KEY);
    m_menuBarBuilder                    = ::fwGui::builder::IMenuBarBuilder::dynamicCast(guiObj);
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::builder::IMenuBarBuilder::REGISTRY_KEY,
                m_menuBarBuilder);

    m_menuBarBuilder->initialize(menuBarConfig);
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeToolBarBuilder(ConfigurationType toolBarConfig)
{
    OSLM_ASSERT("Bad configuration name "<<toolBarConfig->getName()<< ", must be toolBar",
                toolBarConfig->getName() == "toolBar");

    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(::fwGui::builder::IToolBarBuilder::REGISTRY_KEY);
    m_toolBarBuilder                    = ::fwGui::builder::IToolBarBuilder::dynamicCast(guiObj);
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::builder::IToolBarBuilder::REGISTRY_KEY,
                m_toolBarBuilder);

    m_toolBarBuilder->initialize(toolBarConfig);
}

//-----------------------------------------------------------------------------

void IFrameSrv::onCloseExit()
{
    SLM_TRACE_FUNC();
    ::fwGui::Application::New()->exit(0);
}

//-----------------------------------------------------------------------------

void IFrameSrv::onCloseNotify()
{
    SLM_TRACE_FUNC();
    ::fwServices::ObjectMsg::sptr objectMsg = ::fwServices::ObjectMsg::New();
    ::fwData::Object::sptr srvObj           = this->getObject();
    objectMsg->addEvent( "WINDOW_CLOSED" );
    objectMsg->setSource(this->getSptr());
    objectMsg->setSubject( srvObj);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = srvObj->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( objectMsg);
    }
}

//-----------------------------------------------------------------------------

void IFrameSrv::onCloseMessage()
{
    SLM_TRACE_FUNC();
    auto app = ::fwGui::Application::New();
    app->exit(0);
}

//-----------------------------------------------------------------------------

::fwGui::container::fwContainer::sptr IFrameSrv::getProgressWidget()
{
    return m_progressWidget.lock();
}

//-----------------------------------------------------------------------------

}
