/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKWX_VTKRENDERWINDOWINTERACTORMANAGER_HPP_
#define _VISUVTKWX_VTKRENDERWINDOWINTERACTORMANAGER_HPP_

#include <string>

#include <wx/aui/aui.h>

#include <vtkRenderWindowInteractor.h>

#include <vtkinria3d/wxVTKRenderWindowInteractor.h>

#include <fwCore/base.hpp>

#include <fwRenderVTK/IVtkRenderWindowInteractorManager.hpp>

#include <fwGui/fwContainer.hpp>

#include "visuVTKWx/config.hpp"

namespace visuVTKWx
{

/**
 * @brief   Defines a class to manage vtkRenderWindowInteractor in a window.
 * @class   VtkRenderWindowInteractorManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class VISUVTKWX_CLASS_API VtkRenderWindowInteractorManager : public ::fwRenderVTK::IVtkRenderWindowInteractorManager
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (VtkRenderWindowInteractorManager)(::fwRenderVTK::IVtkRenderWindowInteractorManager) )

    /// Constructor. Do nothing.
    VISUVTKWX_API VtkRenderWindowInteractorManager();

    /// Destructor. Do nothing.
    VISUVTKWX_API virtual ~VtkRenderWindowInteractorManager();

    /// Creates an interactor and installs it in window.
    VISUVTKWX_API virtual void installInteractor( ::fwGui::fwContainer::wptr _parent );

    /// Deletes interactor and manage correctly the window (removing layout).
    VISUVTKWX_API virtual void uninstallInteractor();

    /// Return a pointer on interactor
    VISUVTKWX_API virtual ::vtkRenderWindowInteractor * getInteractor();

private:

    wxAuiManager * m_wxmanager;

    wxVTKRenderWindowInteractor * m_interactor;

};

} // namespace visuVTKWx

#endif // _VISUVTKWX_VTKRENDERWINDOWINTERACTORMANAGER_HPP_


