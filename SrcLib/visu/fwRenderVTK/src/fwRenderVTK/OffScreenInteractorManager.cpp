/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/OffScreenInteractorManager.hpp"

#include <fwRenderVTK/registry/macros.hpp>

#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

//-----------------------------------------------------------------------------

namespace fwRenderVTK
{

//-----------------------------------------------------------------------------

/**
 * @brief   Defines a class to redefine a vtkRenderWindowInteractor that can be used in a off screen window.
 *          In offscreen rendering, we don't really need an interactor since the window is not displayed.
 *          But we need this vtkRenderWindowInteractor to keep compatibility with regular windows.
 *          Indeed we often use interactorManager->GetInteractor()->...
 *
 *          On Linux, the default vtkXRenderWindowInteractor creates a X window when we trigger the rendering.
 *          We don't want this behavior. So we instantiate our own empty implementation that does not create a X window.
 *
 *
 */
class vtkOffscreenRenderWindowInteractor : public vtkRenderWindowInteractor
{
public:
    static vtkOffscreenRenderWindowInteractor* New();
    vtkTypeMacro(vtkOffscreenRenderWindowInteractor, vtkRenderWindowInteractor)

protected:
    vtkOffscreenRenderWindowInteractor()
    {
    }
    ~vtkOffscreenRenderWindowInteractor()
    {
    }

private:
    vtkOffscreenRenderWindowInteractor(const vtkOffscreenRenderWindowInteractor&); // Not implemented.
    void operator=(const vtkOffscreenRenderWindowInteractor&); // Not implemented.
};

vtkStandardNewMacro(vtkOffscreenRenderWindowInteractor);

//-----------------------------------------------------------------------------

OffScreenInteractorManager::OffScreenInteractorManager()
{
}

//-----------------------------------------------------------------------------

OffScreenInteractorManager::~OffScreenInteractorManager()
{
}

//-----------------------------------------------------------------------------

void OffScreenInteractorManager::installInteractor( ::fwGui::container::fwContainer::sptr _parent )
{
    SLM_FATAL("This interactor doesn't need container, use installInteractor(width, height).");
}

//-----------------------------------------------------------------------------

void OffScreenInteractorManager::installInteractor(unsigned int width, unsigned int height)
{
    m_renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    m_renderWindow->SetOffScreenRendering( 1 );
    m_renderWindow->SetSize(static_cast<int>(width), static_cast<int>(height));

    m_interactor = vtkSmartPointer<vtkOffscreenRenderWindowInteractor>::New();
    m_interactor->SetRenderWindow(m_renderWindow);
}

//-----------------------------------------------------------------------------

void OffScreenInteractorManager::uninstallInteractor()
{
    m_renderWindow = nullptr;
    m_interactor   = nullptr;
}

//-----------------------------------------------------------------------------

::vtkRenderWindowInteractor* OffScreenInteractorManager::getInteractor()
{
    return m_interactor;
}

//-----------------------------------------------------------------------------

} // namespace fwRenderVTK
