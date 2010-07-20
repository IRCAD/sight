/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/aui/aui.h>

#include <fwTools/ClassRegistrar.hpp>

#include <fwGuiWx/container/WxContainer.hpp>

#include "visuVTKWx/VtkRenderWindowInteractorManager.hpp"


//-----------------------------------------------------------------------------

REGISTER_BINDING(   ::fwRenderVTK::IVtkRenderWindowInteractorManager,
        ::visuVTKWx::VtkRenderWindowInteractorManager,
         ::fwRenderVTK::IVtkRenderWindowInteractorManager::FactoryRegistryKeyType,
          ::fwRenderVTK::IVtkRenderWindowInteractorManager::REGISTRY_KEY );

//-----------------------------------------------------------------------------

namespace visuVTKWx
{

//-----------------------------------------------------------------------------

VtkRenderWindowInteractorManager::VtkRenderWindowInteractorManager()
{}

//-----------------------------------------------------------------------------

VtkRenderWindowInteractorManager::~VtkRenderWindowInteractorManager()
{}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::installInteractor( ::fwGui::fwContainer::sptr _parent )
{
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( _parent );
    wxWindow* const container = wxContainer->getWxContainer();
    SLM_ASSERT("The container is not a wxContainer.", container ) ;

    // Create a VTK-compliant window and insert it
    m_interactor = new ::wxVTKRenderWindowInteractor( container, -1 );

    // Manage wxManager
    m_wxmanager = new wxAuiManager( container );

    // Add interactor in layout
    m_wxmanager->AddPane( m_interactor, wxAuiPaneInfo().CentrePane() );
    m_wxmanager->Update();
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::uninstallInteractor()
{
    wxWindow* container = m_wxmanager->GetManagedWindow();

    m_wxmanager->DetachPane(m_interactor);
    m_wxmanager->UnInit();

    // Destroy interactor
    SLM_ASSERT("Vtk interactor must be defined.", m_interactor );
    m_interactor->GetRenderWindow()->Finalize();

    m_interactor->Delete();
    m_interactor = 0;

    // Destroy wxmanager
    SLM_ASSERT("wxmanager must be defined.", m_wxmanager );
    delete m_wxmanager;
    m_wxmanager = 0;

    // wxAuiManager creates frame in container
    container->DestroyChildren();
}

//-----------------------------------------------------------------------------

::vtkRenderWindowInteractor * VtkRenderWindowInteractorManager::getInteractor()
{
    return m_interactor;
}

//-----------------------------------------------------------------------------

} // namespace visuVTKWx




