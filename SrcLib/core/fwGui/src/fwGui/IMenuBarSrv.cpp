/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IMenuBarSrv.hpp"

namespace fwGui
{

IMenuBarSrv::IMenuBarSrv()
{}

//-----------------------------------------------------------------------------

IMenuBarSrv::~IMenuBarSrv()
{}

//-----------------------------------------------------------------------------

void IMenuBarSrv::initialize()
{
    m_registrar = ::fwGui::registrar::MenuBarRegistrar::NewSptr(this->getUUID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistrar = m_configuration->find("registrar");
    if(!vectRegistrar.empty())
    {
        m_registrarConfig = vectRegistrar.at(0);
    }
    m_registrar->initialize(m_registrarConfig);


    // find LayoutManager configuration
    std::vector < ConfigurationType > vectLayoutMng = m_configuration->find("registrar");
    if(!vectLayoutMng.empty())
    {
        m_layoutConfig = vectLayoutMng.at(0);
    }
    this->initializeLayoutManager(m_layoutConfig);

    ::fwGui::fwMenuBar::sptr menuBar = m_registrar->getParent();
    SLM_ASSERT("Parent menuBar is unknown.", menuBar);
    m_layoutManager->createLayout(menuBar);


    m_registrar->manage(m_layoutManager->getMenus());
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::menuServiceStopping(std::string menuSrvSID)
{
    SLM_FATAL("TODO: IMenuBarSrv::menuServiceStopping not yet implemented");
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::menuServiceStarting(std::string menuSrvSID)
{
    SLM_FATAL("TODO: IMenuBarSrv::menuServiceStarting not yet implemented");
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::initializeLayoutManager(ConfigurationType layoutConfig)
{
    OSLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
            layoutConfig->getName() == "layout");
    SLM_ASSERT("<layout> tag must have type attribute", layoutConfig->hasAttribute("type"));
    std::string layoutManagerClassName = layoutConfig->getAttributeValue("type");
    ::fwTools::Object::sptr layout = ::fwTools::Factory::New(layoutManagerClassName);
    OSLM_ASSERT("Unable to create "<< layoutManagerClassName, layout);
    m_layoutManager = ::fwGui::layoutManager::IMenuBarLayoutManager::dynamicCast(layout);
    OSLM_ASSERT("Unable to cast "<< layoutManagerClassName << " in layout manager", m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
