/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

void VtkRenderWindowInteractorManager::installInteractor( ::fwGui::container::fwContainer::sptr _parent )
{
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( _parent );
    SLM_ASSERT("dynamicCast fwContainer to WxContainer failed", wxContainer);

    wxWindow* container = wxContainer->getWxContainer();
    SLM_ASSERT("Parent container is not available.", container);

    // Create a VTK-compliant window and insert it
    m_interactor = new ::wxVTKRenderWindowInteractor( container, -1 );

    m_wxsizer = new wxBoxSizer( wxVERTICAL );
    m_wxsizer->Add( m_interactor, 1, wxEXPAND | wxALL, 1);
    container->SetSizer( m_wxsizer );

    container->Layout();
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::uninstallInteractor()
{
    SLM_ASSERT("Vtk interactor must be defined.", m_interactor );
    wxWindow* container = m_interactor->GetParent();
    SLM_ASSERT("Parent container is not available.", container);

    // Destroy interactor
    m_interactor->GetRenderWindow()->Finalize();
    m_interactor->Delete();
    m_interactor = 0;

    container->SetSizer(NULL);
}

//-----------------------------------------------------------------------------

::vtkRenderWindowInteractor * VtkRenderWindowInteractorManager::getInteractor()
{
    return m_interactor;
}

//-----------------------------------------------------------------------------

} // namespace visuVTKWx




