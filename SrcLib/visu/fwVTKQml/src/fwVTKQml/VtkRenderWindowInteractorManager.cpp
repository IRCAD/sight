/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVTKQml/VtkRenderWindowInteractorManager.hpp"

#include <fwRenderVTK/registry/macros.hpp>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

//-----------------------------------------------------------------------------

fwRenderVTKRegisterMacro( ::fwVTKQml::VtkRenderWindowInteractorManager,
                          ::fwRenderVTK::IVtkRenderWindowInteractorManager::REGISTRY_KEY );

//-----------------------------------------------------------------------------

namespace fwVTKQml
{

//-----------------------------------------------------------------------------

VtkRenderWindowInteractorManager::VtkRenderWindowInteractorManager(
    ::fwRenderVTK::IVtkRenderWindowInteractorManager::Key )
{
}

//-----------------------------------------------------------------------------

VtkRenderWindowInteractorManager::~VtkRenderWindowInteractorManager()
{
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::installInteractor( ::fwGui::container::fwContainer::sptr )
{

}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::uninstallInteractor()
{

}

//-----------------------------------------------------------------------------

vtkRenderWindowInteractor* VtkRenderWindowInteractorManager::getInteractor()
{
    OSLM_ASSERT("FrameBuffer is not defined", m_frameBufferItem);
    OSLM_ASSERT("RenderWindow is not defined", m_frameBufferItem->getRenderWindow());
    return m_frameBufferItem->getRenderWindow()->GetInteractor();
}

//-----------------------------------------------------------------------------

void VtkRenderWindowInteractorManager::setFrameBuffer(QPointer< ::fwVTKQml::FrameBufferItem > frameBuffer )
{
    m_frameBufferItem = frameBuffer;
}

//-----------------------------------------------------------------------------

} // namespace fwVTKQml
