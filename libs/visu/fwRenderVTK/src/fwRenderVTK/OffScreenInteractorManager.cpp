/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

void OffScreenInteractorManager::installInteractor( ::fwGui::container::fwContainer::sptr )
{
    SLM_FATAL("This interactor doesn't need a container, use installInteractor(width, height).");
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
