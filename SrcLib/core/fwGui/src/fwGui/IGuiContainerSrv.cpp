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
{}

//-----------------------------------------------------------------------------

IGuiContainerSrv::~IGuiContainerSrv()
{}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::createLayoutManager()
{
    m_viewManager = ::fwGui::registrar::ViewRegistrar::NewSptr(this->getUUID());
    // find ViewRegistrar configuration
    std::vector < ConfigurationType > vectViewMng = m_configuration->find("viewManager");
    if(!vectViewMng.empty())
    {
        m_viewMngConfig = vectViewMng.at(0);
    }
    m_viewManager->initialize(m_viewMngConfig);


    // find LayoutManager configuration
    std::vector < ConfigurationType > vectLayoutMng = m_configuration->find("viewManager");
    if(!vectLayoutMng.empty())
    {
        m_layoutConfig = vectLayoutMng.at(0);
    }
    this->initializeLayoutManager(m_layoutConfig);

    ::fwGui::fwContainer::sptr container = m_viewManager->getParent();
    SLM_ASSERT("Parent container is unknown.", container);
    m_layoutManager->createLayout(container);


    m_viewManager->manage(m_layoutManager->getSubViews());
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
    m_layoutManager = ::fwGui::layoutManager::IViewLayoutManager::dynamicCast(layout);
    OSLM_ASSERT("Unable to cast "<< layoutManagerClassName << " in layout manager", m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
