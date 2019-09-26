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
 * - \b modifyVisibility(int) : called to show or hide the reconstruction.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::SReconstruction">
            <in key="reconstruction" uid="..." />
            <config transform="transform" autoresetcamera="autoresetcamera" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b reconstruction [::fwData::Reconstruction]: reconstruction to display.
 * @subsection Configuration Configuration:
 * - \b transform (optional) : the transformation matrix to associate to the adaptor.
 * - \b autoresetcamera (optional, default="yes"): reset the camera when this mesh is modified, "yes" or "no".
 * - \b queryFlags (optional) : Used for picking. Picked only by pickers with the same flag.
 */

class VISUOGREADAPTOR_CLASS_API SReconstruction : public ::fwRenderOgre::IAdaptor,
                                                  public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceMacro(SReconstruction, ::fwRenderOgre::IAdaptor);

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_CHANGE_MESH_SLOT;

    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_VISIBILITY_SLOT;
    /** @} */

    /// Constructor.
    VISUOGREADAPTOR_API SReconstruction() noexcept;

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SReconstruction() noexcept;

    /// Forces the Reconstruction to be hidden or not.
    VISUOGREADAPTOR_API void setForceHide(bool _hide);

    /// Active/Inactive automatic reset on camera for triangular mesh adaptor. By default =true.
    VISUOGREADAPTOR_API void setAutoResetCamera(bool _autoResetCamera);

    /// Changes the material's name
    VISUOGREADAPTOR_API void setMaterialTemplateName(const std::string& _materialName);

    VISUOGREADAPTOR_API ::visuOgreAdaptor::SMesh::sptr getMeshAdaptor();

    /// Returns proposals to connect service slots to associated object signals
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Set meshes vertex buffer to dynamic state (only has effect if called before service starting/update)
    VISUOGREADAPTOR_API void setDynamicVertices(bool _isDynamic);

    /// Set meshes and indices buffers to dynamic state (only has effect if called before service starting/update)
    VISUOGREADAPTOR_API void setDynamic(bool _isDynamic);

    /// Set query mask
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

    /// Changes the attached mesh
    void changeMesh( SPTR( ::fwData::Mesh) );

    /// modify if the
    void modifyVisibility();

    /// Creates the mesh service.
    void createMeshService();

    /// Pointer to the associated mesh service.
    ::fwRenderOgre::IAdaptor::wptr m_meshAdaptor;

    /// sets if the camera has to be reset automatically.
    bool m_autoResetCamera;

    /// Material's name
    std::string m_materialTemplateName;

    /// defines if the mesh changes dynamically
    bool m_isDynamic;

    /// defines if the vertices change dynamically
    bool m_isDynamicVertices;

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
