/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020-2021 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"

#include <fwData/Mesh.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief TODO
 *
 * @section Slots Slots
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SOrientationMarker">
        <inout key="matrix" uid="..." />
        <config layer="default" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the mesh's layer.
 */
class VISUOGREADAPTOR_CLASS_API SOrientationMarker final : public ::fwRenderOgre::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SOrientationMarker, ::fwRenderOgre::IAdaptor)

    /// Constructor
    VISUOGREADAPTOR_API SOrientationMarker() noexcept;

    /// Desctructor
    VISUOGREADAPTOR_API ~SOrientationMarker() noexcept override;

protected:

    /// Configure the service parameters
    VISUOGREADAPTOR_API void configuring() override;

    /// Initialize and start child services
    VISUOGREADAPTOR_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::TransformationMatrix3D::s_MODIFIED_SIG to s_UPDATE_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Update the camera from the input transform
    VISUOGREADAPTOR_API void updating() override;

    /// Unregister child services
    VISUOGREADAPTOR_API void stopping() override;

    /**
     * @brief TODO
     * @param TODO
     */
    VISUOGREADAPTOR_API void setVisible(bool _visible) override;

private:
    /// Type of the 'visibilityUpdated' signal.
    typedef ::fwCom::Signal<void (bool)> VisibilityUpdatedSignalType;

    /// Update the internal camera matrix from the input transform
    void updateCameraMatrix();

    /// Human model
    ::fwData::Mesh::sptr m_mesh { nullptr };

    /// Internal matrix updating the mesh position
    std::shared_ptr< ::fwData::TransformationMatrix3D > m_cameraTransform;

    // Connections with child services
    ::fwCom::helper::SigSlotConnection m_connections;
};

} // namespace visuOgreAdaptor.
