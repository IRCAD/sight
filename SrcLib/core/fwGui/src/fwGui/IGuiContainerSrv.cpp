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

IGuiContainerSrv::IGuiContainerSrv()
    : m_viewLayoutManagerIsCreated (false),
      m_hasToolBar(false)
{}

//-----------------------------------------------------------------------------

IGuiContainerSrv::~IGuiContainerSrv()
{}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::initialize()
{
    SLM_ASSERT("Service hasn't configuration", m_configuration);

    // Create view registrar
    m_viewRegistrar = ::fwGui::registrar::ViewRegistrar::NewSptr( this->getUUID() );
    // find ViewRegistrar configuration
    std::vector < ConfigurationType > vectViewMng = m_configuration->find("registry");
    if ( ! vectViewMng.empty() )
    {
        m_viewRegistrar = ::fwGui::registrar::ViewRegistrar::NewSptr( this->getUUID() );
        m_viewRegistrarConfig = vectViewMng.at(0);
        m_viewRegistrar->initialize(m_viewRegistrarConfig);
    }

    // Create initializeLayoutManager
    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    if( ! vectGui.empty() )
    {
        // find view LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        if( ! vectLayoutMng.empty() )
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
    }
}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::create()
{
    if ( m_viewLayoutManagerIsCreated )
    {
        SLM_ASSERT("ViewRegistrar must be initialized.",m_viewRegistrar);
        ::fwGui::fwContainer::sptr container = m_viewRegistrar->getParent();
        SLM_ASSERT("Parent container is unknown.", container);

        if (m_hasToolBar)
        {
            m_toolBarBuilder->createToolBar(container);
            m_viewRegistrar->manageToolBar(m_toolBarBuilder->getToolBar());
        }

        m_viewLayoutManager->createLayout(container);
        m_viewRegistrar->manage(m_viewLayoutManager->getSubViews());
    }
}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::destroy()
{
    SLM_ASSERT("ViewRegistrar must be initialized.",m_viewRegistrar);

    if ( m_viewLayoutManagerIsCreated )
    {
        m_viewRegistrar->unmanage();

        if (m_hasToolBar)
        {
            SLM_ASSERT("ToolBarBuilder must be initialized.",m_toolBarBuilder);
            m_toolBarBuilder->destroyToolBar();
        }

        SLM_ASSERT("ViewLayoutManager must be initialized.",m_viewLayoutManager);
        m_viewLayoutManager->destroyLayout();
    }
}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::initializeLayoutManager(ConfigurationType layoutConfig)
{
    OSLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
            layoutConfig->getName() == "layout");
    SLM_ASSERT("<layout> tag must have type attribute", layoutConfig->hasAttribute("type"));
    const std::string layoutManagerClassName = layoutConfig->getAttributeValue("type");

    m_viewLayoutManager = ::fwTools::ClassFactoryRegistry::create< ::fwGui::layoutManager::IViewLayoutManager >( layoutManagerClassName);
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< layoutManagerClassName, m_viewLayoutManager);

    m_viewLayoutManager->initialize(layoutConfig);
}


//-----------------------------------------------------------------------------

void IGuiContainerSrv::initializeToolBarBuilder(ConfigurationType toolBarConfig)
{
    OSLM_ASSERT("Bad configuration name "<<toolBarConfig->getName()<< ", must be toolBar",
                toolBarConfig->getName() == "toolBar");

    m_toolBarBuilder = ::fwTools::ClassFactoryRegistry::create< ::fwGui::builder::IToolBarBuilder >( ::fwGui::builder::IToolBarBuilder::REGISTRY_KEY );
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::builder::IToolBarBuilder::REGISTRY_KEY, m_toolBarBuilder);

    m_toolBarBuilder->initialize(toolBarConfig);
}

//-----------------------------------------------------------------------------

::fwGui::fwContainer::sptr IGuiContainerSrv::getContainer()
{
    return m_viewRegistrar->getParent();
}

//-----------------------------------------------------------------------------

}
