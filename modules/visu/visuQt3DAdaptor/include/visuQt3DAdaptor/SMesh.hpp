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

#include "visuQt3DAdaptor/config.hpp"

#include <fwRenderQt3D/data/Mesh.hpp>
#include <fwRenderQt3D/IAdaptor.hpp>

namespace visuQt3DAdaptor
{

/**
 * @brief This adaptor shows individual meshes.
 *
 * This class handles the conversion of data::Mesh to Qt3D. It can handle triangles.
 *
 * @section Slot Slot
 * - \b updateVisibility(bool): shows or hides the mesh.
 * - \b toggleVisibility(): Toggle whether the adaptor is shown or not.
 * - \b modifyVertices(): called when the vertices are modified.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuQt3DAdaptor::SMesh" >
        <in key="mesh" uid="..." />
        <config autoresetcamera="true" visible="true" materialName="..." />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b mesh [data::Mesh]: adapted mesh.
 *
 * @subsection Configuration Configuration:
 *  - \b autoresetcamera (optional, bool, default=false): reset the camera when this mesh is modified.
 *  - \b visible (optional, bool, default=true): set the initial visibility of the mesh.
 *  - \b materialName (optional): need to be used when a material adaptor is defined in XML file.
 *        Must be set according to ::visuQt3D::SMaterial materialName property
 *        to retrieve the material managed by the adaptor.
 */
class VISUQT3DADAPTOR_CLASS_API SMesh : public ::fwRenderQt3D::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SMesh, ::fwRenderQt3D::IAdaptor)

    /// Sets default parameters and initializes necessary members.
    VISUQT3DADAPTOR_API SMesh() noexcept;

    /// Destroys the adaptor.
    VISUQT3DADAPTOR_API ~SMesh() noexcept override;

protected:

    /// Configures the adaptor.
    VISUQT3DADAPTOR_API void configuring() override;

    /// Starts the adaptor.
    VISUQT3DADAPTOR_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Image::s_MODIFIED_SIG of s_MESH_INOUT to s_UPDATE_SLOT.
     * Connect data::Image::s_VERTEX_MODIFIED_SIG of s_MESH_INOUT to s_MODIFY_VERTICES_SLOT.
     */
    VISUQT3DADAPTOR_API services::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates the mesh.
    VISUQT3DADAPTOR_API void updating() override;

    /// Does nothing.
    VISUQT3DADAPTOR_API void stopping() override;

private:

    /**
     * @brief Sets whether the mesh is to be seen or not.
     * @param _visibility the visibility status of the volume.
     */
    void updateVisibility(bool _visibility) override;

    /// Updates mesh vertices.
    void modifyVertices();

    /// Contains a Qt3D mesh.
    QPointer< ::fwRenderQt3Ddata::Mesh > m_mesh;

    /// Specifies the material adaptor used to configure mesh material.
    std::string m_materialName;

    /// Defines whether the camera must be auto reset when a mesh is updated or not.
    bool m_autoResetCamera { false };

};

} // namespace visuQt3DAdaptor.
