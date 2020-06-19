/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/factory/new.hpp"
#include "fwRenderVTK/registry/detail.hpp"

#include <fwCore/base.hpp>

#include <fwGui/container/fwContainer.hpp>

#include <fwServices/IService.hpp>

#include <vtkRenderWindowInteractor.h>

#include <string>

namespace fwRenderVTK
{

/**
 * @brief   Defines a class to manage vtkRenderWindowInteractor in a window.
 */
class FWRENDERVTK_CLASS_API IVtkRenderWindowInteractorManager : public ::fwCore::BaseObject
{

public:

    typedef ::fwRenderVTK::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::fwRenderVTK::registry::get()->addFactory(functorKey, &::fwRenderVTK::factory::New<T>);
        }
    };

    fwCoreClassMacro(IVtkRenderWindowInteractorManager, ::fwCore::BaseObject);

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
    FWRENDERVTK_API virtual ::vtkRenderWindowInteractor* getInteractor() = 0;

    //------------------------------------------------------------------------------

    virtual void setRenderService(::fwServices::IService::sptr srv)
    {
        m_renderService = srv;
    }

protected:
    ::fwServices::IService::wptr m_renderService;
};

} // namespace fwRenderVTK
