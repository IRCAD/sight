/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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
