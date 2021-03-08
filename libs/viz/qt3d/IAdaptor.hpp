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

#include "viz/qt3d/config.hpp"
#include "viz/qt3d/SRender.hpp"

#include <service/IService.hpp>

namespace sight::viz::qt3d
{

/**
 * @brief Interface providing behavior of Qt3D adaptor services.
 *
 * @section Slot Slot
 * - \b updateVisibility(bool): shows or hides the mesh.
 * - \b toggleVisibility(): Toggle whether the adaptor is shown or not.
 */
class VIZ_QT3D_CLASS_API IAdaptor : public service::IService
{

public:

    SIGHT_DECLARE_CLASS(IAdaptor, sight::service::IService)

    /// Gets the render service using this adaptor.
    VIZ_QT3D_API SRender::sptr getRenderService() const;

protected:

    /// Creates the interface.
    VIZ_QT3D_API IAdaptor();

    /// Destroys the interface.
    VIZ_QT3D_API ~IAdaptor() override;

    /// Registers the adaptor into its SRender service.
    VIZ_QT3D_API void initialize();

    /**
     * @brief Sets whether the mesh is to be seen or not.
     * @param _visibility the visibility status of the volume.
     */
    VIZ_QT3D_API virtual void updateVisibility(bool _visibility);

    /// Toggles the visibility of the axis.
    VIZ_QT3D_API void toggleVisibility();

    /// Contains the render service this adaptor is attached to.
    viz::qt3d::SRender::wptr m_renderService;

    /// Enables the adaptor visibility.
    bool m_isVisible { true };

};

} // namespace sight::viz::qt3d.
