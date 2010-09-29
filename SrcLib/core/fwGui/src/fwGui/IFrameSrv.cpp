/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/ObjectMsg.hpp>

#include "fwGui/IFrameSrv.hpp"

namespace fwGui
{

const std::string IFrameSrv::CLOSE_POLICY_EXIT   = "exit";
const std::string IFrameSrv::CLOSE_POLICY_NOTIFY = "notify";

IFrameSrv::IFrameSrv() :
        m_hasMenuBar(false),
        m_hasToolBar(false),
        m_closePolicy("exit")
{}

//-----------------------------------------------------------------------------

IFrameSrv::~IFrameSrv()
{}

//-----------------------------------------------------------------------------

void IFrameSrv::initialize()
{
    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
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
        std::string onclose = window->getAttributeValue("onclose");
        if ( !onclose.empty() )
        {
            m_closePolicy = onclose;
        }
        SLM_ASSERT("Invalid onclose value : " << m_closePolicy << ". Should be 'exit' or 'notify'", m_closePolicy == CLOSE_POLICY_NOTIFY || m_closePolicy == CLOSE_POLICY_EXIT);
    }

    m_viewRegistrar = ::fwGui::registrar::ViewRegistrar::NewSptr(this->getUUID());
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
    std::vector< ::fwGui::container::fwContainer::sptr > subViews;
    subViews.push_back(frame);
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
    m_viewRegistrar->unmanage();

    if (m_hasMenuBar)
    {
        SLM_ASSERT("MenuBarBuilder must be initialized.",m_menuBarBuilder);
        m_menuBarBuilder->destroyMenuBar();
    }

    if (m_hasToolBar)
    {
        SLM_ASSERT("ToolBarBuilder must be initialized.",m_toolBarBuilder);
        m_toolBarBuilder->destroyToolBar();
    }

    SLM_ASSERT("FrameLayoutManager must be initialized.",m_frameLayoutManager);
    m_frameLayoutManager->destroyFrame();
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeLayoutManager(ConfigurationType frameConfig)
{
    OSLM_ASSERT("Bad configuration name "<<frameConfig->getName()<< ", must be frame",
            frameConfig->getName() == "frame");

    m_frameLayoutManager = ::fwTools::ClassFactoryRegistry::create< ::fwGui::layoutManager::IFrameLayoutManager >( ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY );
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY, m_frameLayoutManager);

    m_frameLayoutManager->initialize(frameConfig);
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeMenuBarBuilder(ConfigurationType menuBarConfig)
{
    OSLM_ASSERT("Bad configuration name "<<menuBarConfig->getName()<< ", must be menuBar",
                menuBarConfig->getName() == "menuBar");

    m_menuBarBuilder = ::fwTools::ClassFactoryRegistry::create< ::fwGui::builder::IMenuBarBuilder >( ::fwGui::builder::IMenuBarBuilder::REGISTRY_KEY );
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::builder::IMenuBarBuilder::REGISTRY_KEY, m_menuBarBuilder);

    m_menuBarBuilder->initialize(menuBarConfig);
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeToolBarBuilder(ConfigurationType toolBarConfig)
{
    OSLM_ASSERT("Bad configuration name "<<toolBarConfig->getName()<< ", must be toolBar",
                toolBarConfig->getName() == "toolBar");

    m_toolBarBuilder = ::fwTools::ClassFactoryRegistry::create< ::fwGui::builder::IToolBarBuilder >( ::fwGui::builder::IToolBarBuilder::REGISTRY_KEY );
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::builder::IToolBarBuilder::REGISTRY_KEY, m_toolBarBuilder);

    m_toolBarBuilder->initialize(toolBarConfig);
}

//-----------------------------------------------------------------------------

void IFrameSrv::onCloseExit()
{
    SLM_TRACE_FUNC();
    ::fwServices::OSR::uninitializeRootObject();
}

//-----------------------------------------------------------------------------

void IFrameSrv::onCloseNotify()
{
    SLM_TRACE_FUNC();
    ::fwServices::ObjectMsg::NewSptr objectMsg;
    ::fwTools::Object::sptr srvObj = this->getObject();
    objectMsg->addEvent( "WINDOW_CLOSED" );
    ::fwServices::IEditionService::notify(this->getSptr(), srvObj, objectMsg);
}

//-----------------------------------------------------------------------------

}
