/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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
#include "visuOgreAdaptor/SMesh.hpp"

#include <fwCom/Slot.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

namespace fwData
{

class Mesh;
}

namespace visuOgreAdaptor
{
/**
 * @brief
 *
 * @section Slots Slots
 * - \b changeMesh(::fwData::Mesh::sptr) : called when the associated mesh changes.
 * - \b modifyVisibility() : called to show or hide the reconstruction.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::SReconstruction">
            <in key="reconstruction" uid="..." />
            <config layer="..." transform="..." autoresetcamera="yes" queryFlags="0x40000000" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b reconstruction [::fwData::Reconstruction]: reconstruction to display.
 * @subsection Configuration Configuration:
 * - \b layer (mandatory) : defines the mesh's layer.
 * - \b transform (optional) : the transformation matrix to associate to the adaptor.
 * - \b autoresetcamera (optional, default="yes"): reset the camera when this mesh is modified, "yes" or "no".
 * - \b queryFlags (optional, default=0x40000000) : Used for picking. Picked only by pickers whose mask that match the
 * flag.
 */
class VISUOGREADAPTOR_CLASS_API SReconstruction : public ::fwRenderOgre::IAdaptor,
                                                  public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceMacro(SReconstruction, ::fwRenderOgre::IAdaptor)

    /// Slot used when the mesh has changed.
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_MESH_SLOT;

    /// Slot used to change the reconstruciton visibility.
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_VISIBILITY_SLOT;

    /// Initialise slots.
    VISUOGREADAPTOR_API SReconstruction() noexcept;

    /// Does nothing
    VISUOGREADAPTOR_API virtual ~SReconstruction() noexcept;

    /**
     * @brief Forces the reconstruction to be hidden or not.
     * @param _hide Set tot true to force the reconstruction to be hidden.
     */
    VISUOGREADAPTOR_API void setForceHide(bool _hide);

    /**
     * @brief Active/Inactive automatic reset on camera.
     * @param _autoResetCamera Use true to activate it.
     */
    VISUOGREADAPTOR_API void setAutoResetCamera(bool _autoResetCamera);

    /**
     * @brief Sets the material template Name.
     * @param _materialName The material name.
     */
    VISUOGREADAPTOR_API void setMaterialTemplateName(const std::string& _materialName);

    /**
     * @brief Get the mesh adaptor.
     * @return The mesh adaptor.
     */
    VISUOGREADAPTOR_API ::visuOgreAdaptor::SMesh::sptr getMeshAdaptor();

    /**
     * @brief Returns proposals to connect service slots to associated object signals.
     * @return The connection map proposals.
     */
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Set meshes vertex buffer to dynamic state (only has effect if called before service starting/update).
     * @param _isDynamic Set to true to use dynamic vertex buffer.
     */
    VISUOGREADAPTOR_API void setDynamicVertices(bool _isDynamic);

    /**
     * @brief Set meshes and indices buffers to dynamic state (only has effect if called before service
     * starting/update).
     * @param _isDynamic Set to true to use dynamic vertex and indices buffer.
     */
    VISUOGREADAPTOR_API void setDynamic(bool _isDynamic);

    /**
     * @brief Set the query flag.
     * @param _queryFlags The value of the query flag.
     */
    VISUOGREADAPTOR_API void setQueryFlags(std::uint32_t _queryFlags);

protected:

    /// Configure the Reconstruction adaptor.
    VISUOGREADAPTOR_API void configuring() override;

    /// starts the service; creates the mesh service.
    VISUOGREADAPTOR_API void starting() override;

    /// stops and unregisters the service.
    VISUOGREADAPTOR_API void stopping() override;

    /// Updates the mesh adaptor according to the reconstruction or creates it if it hasn't been yet.
    VISUOGREADAPTOR_API void updating() override;

private:

    /// Changes the attached mesh.
    void changeMesh(::fwData::Mesh::sptr);

    /// Modify the visibility.
    void modifyVisibility();

    /// Creates the mesh service.
    void createMeshService();

    /// Pointer to the associated mesh service.
    ::fwRenderOgre::IAdaptor::wptr m_meshAdaptor;

    /// sets if the camera has to be reset automatically.
    bool m_autoResetCamera {true};

    /// Material's name
    std::string m_materialTemplateName {::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME};

    /// defines if the mesh changes dynamically
    bool m_isDynamic {false};

    /// defines if the vertices change dynamically
    bool m_isDynamicVertices {false};

    /// Mask for picking requests
    std::uint32_t m_queryFlags {::Ogre::SceneManager::ENTITY_TYPE_MASK};
};

//------------------------------------------------------------------------------
// Inline function(s)

inline void SReconstruction::setAutoResetCamera(bool _autoResetCamera)
{
    m_autoResetCamera = _autoResetCamera;
}

//------------------------------------------------------------------------------

inline void SReconstruction::setMaterialTemplateName(const std::string& _materialName)
{
    m_materialTemplateName = _materialName;
}

//------------------------------------------------------------------------------

inline void SReconstruction::setDynamicVertices(bool _isDynamic)
{
    m_isDynamicVertices = _isDynamic;
}

//------------------------------------------------------------------------------

inline void SReconstruction::setDynamic(bool _isDynamic)
{
    m_isDynamic = _isDynamic;
}

//------------------------------------------------------------------------------

inline void SReconstruction::setQueryFlags(uint32_t _queryFlags)
{
    m_queryFlags = _queryFlags;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
