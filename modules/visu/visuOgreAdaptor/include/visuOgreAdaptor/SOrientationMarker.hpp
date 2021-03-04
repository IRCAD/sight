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
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief Displays an orientation "marker", marker is represented by a human body mesh,
 * its orientation follows camera movement.
 *
 * @section Slots Slots
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SOrientationMarker" autoConnect="yes">
        <in key="matrix" uid="..." />
        <config layer="default" resource="..." depth="-32.0" />
    </service>
   @endcode
 *
 * @subsection Input Input
 *  - \b matrix [::fwData::TransformationMatrix3D]: matrix to follow (usually SCamera Matrix).
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the mesh's layer.
 * - \b resource (optional, string): name of the resource to use for the marker.
 * - \b depth (optionnal, float): value of depth (z) where marker will be positioned, greater value to zoom-in , lower
 * to zoom-out.
 */
class VISUOGREADAPTOR_CLASS_API SOrientationMarker final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::ITransformable
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SOrientationMarker, ::fwRenderOgre::IAdaptor)

    /// Constructor
    VISUOGREADAPTOR_API SOrientationMarker() noexcept;

    /// Destructor
    VISUOGREADAPTOR_API ~SOrientationMarker() noexcept override;

protected:

    /// Configures the service's parameters
    VISUOGREADAPTOR_API void configuring() override;

    /// Initializes and starts child services
    VISUOGREADAPTOR_API void starting() override;

    /// Updates internal matrix from the input transform
    VISUOGREADAPTOR_API void updating() override;

    /// Unregisters child services
    VISUOGREADAPTOR_API void stopping() override;

    /// Sets the visibility of the adaptor
    VISUOGREADAPTOR_API void setVisible(bool _visible) override;

    /// Connects input matrix S_MODIFIED to UPDATE slot.
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    /// Contains the material data.
    ::fwData::Material::sptr m_material { nullptr };

    /// Updates the internal camera matrix from the input transform
    void updateCameraMatrix();

    /// Contains the scene node where all of manual objects are attached.
    ::Ogre::SceneNode* m_sceneNode { nullptr };

    /// Stores the entity associated to the marker mesh
    ::Ogre::Entity* m_patientMesh { nullptr };

    /// Resource used for the marker
    std::string m_patientMeshRc { "human.mesh" };

    /// Z coordinate of marker position, increase to zoom in, decrease to zoom out.
    float m_markerDepth = -32.f;

};

} // namespace visuOgreAdaptor.
