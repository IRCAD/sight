/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_OFFSCREENINTERACTORMANAGER_HPP__
#define __FWRENDERVTK_OFFSCREENINTERACTORMANAGER_HPP__

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/IVtkRenderWindowInteractorManager.hpp"

#include <fwCore/base.hpp>

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>

namespace fwRenderVTK
{

/**
 * @brief   Defines a class to manage vtkRenderWindowInteractor in a off screen window.
 */
class FWRENDERVTK_CLASS_API OffScreenInteractorManager : public ::fwRenderVTK::IVtkRenderWindowInteractorManager
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((OffScreenInteractorManager)
                                           (::fwRenderVTK::IVtkRenderWindowInteractorManager),
                                           (()), new OffScreenInteractorManager)

    FWRENDERVTK_API OffScreenInteractorManager();

    FWRENDERVTK_API virtual ~OffScreenInteractorManager();

    /// Do NOT use, this interactor doens't need container. You must use installInteractor(width, height) instead.
    FWRENDERVTK_API virtual void installInteractor( ::fwGui::container::fwContainer::sptr _parent ) override;

    /// Creates a render window and an interactor.
    FWRENDERVTK_API virtual void installInteractor( unsigned int width, unsigned int height );

    /// Deletes interactor and manage correctly the window (removing layout).
    FWRENDERVTK_API virtual void uninstallInteractor() override;

    /// Return a pointer on interactor
    FWRENDERVTK_API virtual ::vtkRenderWindowInteractor* getInteractor() override;

private:

    vtkSmartPointer< vtkRenderWindow > m_renderWindow;
    vtkSmartPointer< vtkRenderWindowInteractor > m_interactor;

};

} // namespace fwRenderVTK

#endif // __FWRENDERVTK_OFFSCREENINTERACTORMANAGER_HPP__
