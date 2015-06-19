/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiDroid/SManager.hpp"

#include <fwServices/Base.hpp>

namespace guiDroid
{

fwServicesRegisterMacro( ::fwServices::IService, ::guiDroid::SManager, ::fwData::Composite);

//------------------------------------------------------------------------------
SManager::SManager() throw()
{
}

//------------------------------------------------------------------------------

SManager::~SManager() throw()
{
}

//------------------------------------------------------------------------------

void SManager::configuring() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SManager::starting() throw(::fwTools::Failed)
{
    SLM_ASSERT("Service hasn't configuration", m_configuration);

    // Create initializeLayoutManager
    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    if( !vectGui.empty() )
    {
        SLM_ASSERT("Sorry, no <registry> tag is allowed in <gui> section", vectGui.at(0)->find("registry").empty());

        // find view LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        if( !vectLayoutMng.empty() )
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

//------------------------------------------------------------------------------

void SManager::stopping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SManager::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

} // namespace guiDroid
