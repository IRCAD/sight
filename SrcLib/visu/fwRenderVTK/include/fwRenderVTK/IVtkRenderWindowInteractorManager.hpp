/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRENDERVTK_IVTKRENDERWINDOWINTERACTORMANAGER_HPP_
#define _FWRENDERVTK_IVTKRENDERWINDOWINTERACTORMANAGER_HPP_

#include <string>

#include <vtkRenderWindowInteractor.h>

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>

#include "fwRenderVTK/config.hpp"

namespace fwRenderVTK
{

/**
 * @brief   Defines a class to manage vtkRenderWindowInteractor in a window.
 * @class   IVtkRenderWindowInteractorManager.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWRENDERVTK_CLASS_API IVtkRenderWindowInteractorManager : public ::fwCore::BaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IVtkRenderWindowInteractorManager)(::fwCore::BaseObject) )

    typedef std::string FactoryRegistryKeyType;

    FWRENDERVTK_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWRENDERVTK_API static IVtkRenderWindowInteractorManager::sptr createManager();

    /// Constructor. Do nothing.
    FWRENDERVTK_API IVtkRenderWindowInteractorManager();

    /// Destructor. Do nothing.
    FWRENDERVTK_API virtual ~IVtkRenderWindowInteractorManager();

    /// Creates an interactor and installs it in window.
    FWRENDERVTK_API virtual void installInteractor( ::fwGui::container::fwContainer::sptr _parent ) = 0;

    /// Deletes interactor and manage correctly the window (removing layout).
    FWRENDERVTK_API virtual void uninstallInteractor() = 0;

    /// Return a pointer on interactor
    FWRENDERVTK_API virtual ::vtkRenderWindowInteractor * getInteractor() = 0;

};

} // namespace fwRenderVTK

#endif // _FWRENDERVTK_IVTKRENDERWINDOWINTERACTORMANAGER_HPP_


