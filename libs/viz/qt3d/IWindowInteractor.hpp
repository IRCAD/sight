/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "viz/qt3d/config.hpp"
#include "viz/qt3d/factory/new.hpp"
#include "viz/qt3d/registry/detail.hpp"

#include <core/base.hpp>

#include <service/IService.hpp>

#include <ui/base/container/fwContainer.hpp>

namespace Qt3DExtras
{

class Qt3DWindow;

} // namespace Qt3DExtras

namespace sight::viz::qt3d
{

/**
 * @brief Defines an abstract class to manage Qt3D RenderWindow in a qt container.
 *
 */
class VIZ_QT3D_CLASS_API IWindowInteractor : public sight::core::BaseObject
{
public:

    typedef viz::qt3d::factory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template<typename T>
    class Registry
    {
    public:

        Registry(std::string functorKey)
        {
            viz::qt3d::registry::get()->addFactory(functorKey, &sight::viz::qt3d::factory::New<T>);
        }
    };

    SIGHT_DECLARE_CLASS(IWindowInteractor, sight::core::BaseObject);

    typedef std::string FactoryRegistryKeyType;

    VIZ_QT3D_API static const FactoryRegistryKeyType REGISTRY_KEY;

    VIZ_QT3D_API static IWindowInteractor::sptr createManager();

    /// Constructor. Do nothing.
    VIZ_QT3D_API IWindowInteractor();

    /// Destructor. Do nothing.
    VIZ_QT3D_API ~IWindowInteractor() override;

    /// Creates an interactor and installs it in window.
    VIZ_QT3D_API virtual void createContainer(ui::base::container::fwContainer::sptr _parent) = 0;

    /// Converts 3DView to QWidget and attach it to the container.
    VIZ_QT3D_API virtual void set3DView(Qt3DExtras::Qt3DWindow* _3dView) = 0;

    /// Set the render service using the IRenderWindowInteractor
    virtual void setRenderService(service::IService::sptr srv)
    {
        m_renderService = srv;
    }

protected:

    service::IService::wptr m_renderService;
};

} // namespace sight::viz::qt3d
