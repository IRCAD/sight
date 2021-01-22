/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwRenderQt3D/config.hpp"
#include "fwRenderQt3D/factory/new.hpp"
#include "fwRenderQt3D/registry/detail.hpp"

#include <core/base.hpp>

#include <fwGui/container/fwContainer.hpp>

#include <fwServices/IService.hpp>

namespace Qt3DExtras
{
class Qt3DWindow;
}

namespace fwRenderQt3D
{

/**
 * @brief Defines an abstract class to manage Qt3D RenderWindow in a qt container.
 *
 */
class FWRENDERQT3D_CLASS_API IRenderWindowInteractorManager : public core::BaseObject
{

public:

    typedef ::fwRenderQt3D::factory::Key Key;

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
            ::fwRenderQt3D::registry::get()->addFactory(functorKey, &::fwRenderQt3D::factory::New<T>);
        }
    };

    fwCoreClassMacro(IRenderWindowInteractorManager, core::BaseObject)

    typedef std::string FactoryRegistryKeyType;

    FWRENDERQT3D_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWRENDERQT3D_API static IRenderWindowInteractorManager::sptr createManager();

    /// Constructor. Do nothing.
    FWRENDERQT3D_API IRenderWindowInteractorManager();

    /// Destructor. Do nothing.
    FWRENDERQT3D_API virtual ~IRenderWindowInteractorManager();

    /// Creates an interactor and installs it in window.
    FWRENDERQT3D_API virtual void createContainer( ::fwGui::container::fwContainer::sptr _parent) = 0;

    /// Converts 3DView to QWidget and attach it to the container.
    FWRENDERQT3D_API virtual void set3DView(Qt3DExtras::Qt3DWindow* _3dView) = 0;

    /// Set the render service using the IRenderWindowInteractor
    virtual void setRenderService(::fwServices::IService::sptr srv)
    {
        m_renderService = srv;
    }

protected:
    ::fwServices::IService::wptr m_renderService;
};

} // namespace fwRenderQt3D
