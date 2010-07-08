/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IGuiContainerSrv.hpp"

namespace fwGui
{

IGuiContainerSrv::IGuiContainerSrv() : m_hasMenuBar(false)
{}

//-----------------------------------------------------------------------------

IGuiContainerSrv::~IGuiContainerSrv()
{}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::initialize()
{
    SLM_ASSERT("Service hasn't configuration", m_configuration);
    m_viewRegistrar = ::fwGui::registrar::ViewRegistrar::NewSptr(this->getUUID());
    // find ViewRegistrar configuration
    std::vector < ConfigurationType > vectViewMng = m_configuration->find("viewRegistrar");
    SLM_ASSERT("<viewRegistrar> xml element must exist", !vectViewMng.empty());

    m_viewRegistrarConfig = vectViewMng.at(0);
    m_viewRegistrar->initialize(m_viewRegistrarConfig);



    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    if(!vectGui.empty())
    {
        // find view LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        if(!vectLayoutMng.empty())
        {
            m_viewLayoutConfig = vectLayoutMng.at(0);
            this->initializeLayoutManager(m_viewLayoutConfig);
        }

        // find mebuBarBuilder configuration
        std::vector < ConfigurationType > vectMBBuilder = vectGui.at(0)->find("menuBar");
        if(!vectMBBuilder.empty())
        {
            m_menuBarConfig = vectMBBuilder.at(0);
            this->initializeMenuBarBuilder(m_menuBarConfig);

            m_hasMenuBar = true;
        }
    }
}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::create()
{
    SLM_ASSERT("ViewRegistrar must be initialized.",m_viewRegistrar);
    ::fwGui::fwContainer::sptr container = m_viewRegistrar->getParent();
    SLM_ASSERT("Parent container is unknown.", container);

    SLM_ASSERT("ViewLayoutManager must be initialized.",m_viewLayoutManager);
    m_viewLayoutManager->createLayout(container);

    m_viewRegistrar->manage(m_viewLayoutManager->getSubViews());

    if (m_hasMenuBar)
    {
        m_viewRegistrar->manageMenuBar(m_menuBarBuilder->getMenuBar());
    }
}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::destroy()
{
    SLM_ASSERT("ViewRegistrar must be initialized.",m_viewRegistrar);
    m_viewRegistrar->unmanage();

    SLM_ASSERT("ViewLayoutManager must be initialized.",m_viewLayoutManager);
    m_viewLayoutManager->destroyLayout();

    if (m_hasMenuBar)
    {
        SLM_ASSERT("MenuBarBuilder must be initialized.",m_menuBarBuilder);
        m_menuBarBuilder->destroyMenuBar();
    }
}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::initializeLayoutManager(ConfigurationType layoutConfig)
{
    OSLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
            layoutConfig->getName() == "layout");
    SLM_ASSERT("<layout> tag must have type attribute", layoutConfig->hasAttribute("type"));
    std::string layoutManagerClassName = layoutConfig->getAttributeValue("type");
    ::fwTools::Object::sptr layout = ::fwTools::Factory::New(layoutManagerClassName);
    OSLM_ASSERT("Unable to create "<< layoutManagerClassName, layout);
    m_viewLayoutManager = ::fwGui::layoutManager::IViewLayoutManager::dynamicCast(layout);
    OSLM_ASSERT("Unable to cast "<< layoutManagerClassName << " in layout manager", m_viewLayoutManager);

    m_viewLayoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::initializeMenuBarBuilder(ConfigurationType menuBarConfig)
{
    OSLM_ASSERT("Bad configuration name "<<menuBarConfig->getName()<< ", must be menuBar",
                menuBarConfig->getName() == "menuBar");
    SLM_ASSERT("<menuBar> tag must have type attribute", menuBarConfig->hasAttribute("type"));
    std::string menuBarClassName = menuBarConfig->getAttributeValue("type");
    ::fwTools::Object::sptr menuBarBuilder = ::fwTools::Factory::New(menuBarClassName);
    OSLM_ASSERT("Unable to create "<< menuBarClassName, menuBarBuilder);
    m_menuBarBuilder = ::fwGui::builder::IMenuBarBuilder::dynamicCast(menuBarBuilder);
    OSLM_ASSERT("Unable to cast "<< menuBarClassName << " in layout manager", m_menuBarBuilder);

    m_menuBarBuilder->initialize(menuBarConfig);
}
//-----------------------------------------------------------------------------

}
