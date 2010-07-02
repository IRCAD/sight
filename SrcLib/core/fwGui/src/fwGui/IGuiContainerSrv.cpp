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
{
    m_viewManager = ::fwGui::ViewManager::NewSptr(this->getUUID());
}

//-----------------------------------------------------------------------------

IGuiContainerSrv::~IGuiContainerSrv()
{}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::createViewManager( ::fwRuntime::ConfigurationElement::sptr viewMngConfig )
{
    m_viewManager->initialize(viewMngConfig);
}

//-----------------------------------------------------------------------------

void IGuiContainerSrv::createLayoutManager( ::fwRuntime::ConfigurationElement::sptr layoutConfig )
{
    SLM_FATAL("TODO: manage not yet available");
    // TODO:
    // - use Factory to instanced a LayoutManager implementation
    // - m_layoutManager = instanced_LayoutManager_implementation
    // - m_layoutManager->initialize(layoutConfig);
    // - m_layoutManager->createLayout(m_viewManager->getParent());
    // - m_viewManager->manage(m_layoutManager->getSubViews());
}

}
