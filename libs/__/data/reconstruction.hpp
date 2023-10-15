/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/image.hpp"
#include "data/material.hpp"
#include "data/mesh.hpp"
#include "data/object.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

namespace sight::data
{

/**
 * @brief This class defines a reconstruction object.
 *
 * A reconstruction is represented by a mesh, a material and an image (mask).
 */
class DATA_CLASS_API reconstruction final : public object
{
public:

    SIGHT_DECLARE_CLASS(reconstruction, object);

    SIGHT_ALLOW_SHARED_FROM_THIS()

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API reconstruction();

    /// Destructor
    DATA_API ~reconstruction() noexcept override = default;

    /// Constant to inform that mask volume has not been computed yet.
    DATA_API static const double s_NO_COMPUTED_MASK_VOLUME;

    /**
     * @{
     * @brief Get/Set value of the bIsVisible.
     */
    bool getIsVisible() const;
    void setIsVisible(bool _bIsVisible);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the organName.
     */
    std::string getOrganName() const;
    void setOrganName(const std::string& _sOrganName);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the structureType.
     */
    std::string getStructureType() const;
    void setStructureType(const std::string& _sStructureType);
    /// @}

    /**
     * @brief Get/Set the image associated with the acquisition
     */
    image::sptr getImage();
    image::csptr getImage() const;
    void setImage(const image::sptr& val);

    /**
     * @brief Get/Set the mesh associated with the acquisition
     */
    mesh::sptr getMesh();
    mesh::csptr getMesh() const;
    void setMesh(const mesh::sptr& val);
    /// @}

    /**
     * @brief Get/Set the material associated with the acquisition
     */
    material::sptr getMaterial();
    material::csptr getMaterial() const;
    void setMaterial(const material::sptr& val);
    /// @}

    /**
     * @brief Get/Set the volume from mask
     */
    double getComputedMaskVolume() const;
    void setComputedMaskVolume(double val);
    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when the mesh pointer has changed, mesh parameter is used to store old mesh
    typedef core::com::signal<void (mesh::sptr)> MeshChangedSignalType;

    /// Key in m_signals map of signal m_sigMeshModified
    DATA_API static const core::com::signals::key_t MESH_CHANGED_SIG;

    /// Type of signal when mesh is modified
    typedef core::com::signal<void (bool)> VisibilityModifiedSignalType;

    /// Key in m_signals map of signal m_sigVisibilityModified
    DATA_API static const core::com::signals::key_t VISIBILITY_MODIFIED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const reconstruction& other) const noexcept;
    DATA_API bool operator!=(const reconstruction& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& source,
        const std::unique_ptr<deep_copy_cache_t>& cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    //! true if this reconstruction is visible
    bool m_bIsVisible {false};

    //! Organ name
    std::string m_sOrganName;

    //! Structure type
    std::string m_sStructureType;

    //--------------------------------------------------------------------------

    //! reconstruction's material
    material::sptr m_material;

    //! reconstruction's mask
    image::sptr m_image;

    //! reconstruction's mesh
    mesh::sptr m_mesh;

    //! reconstruction's mask volume
    double m_computedMaskVolume;

    /**
     * @name Signals
     * @{
     */

    /// Signal emitted when mesh changed, mesh parameter is used to store old mesh
    MeshChangedSignalType::sptr m_sigMeshChanged;

    /// Signal emitted  when visibility is modified
    VisibilityModifiedSignalType::sptr m_sigVisibilityModified;
    /**
     * @}
     */
};

//-----------------------------------------------------------------------------

inline bool reconstruction::getIsVisible() const
{
    return m_bIsVisible;
}

//-----------------------------------------------------------------------------

inline void reconstruction::setIsVisible(const bool _bIsVisible)
{
    m_bIsVisible = _bIsVisible;
}

//-----------------------------------------------------------------------------

inline std::string reconstruction::getOrganName() const
{
    return m_sOrganName;
}

//-----------------------------------------------------------------------------

inline void reconstruction::setOrganName(const std::string& _sOrganName)
{
    m_sOrganName = _sOrganName;
}

//-----------------------------------------------------------------------------

inline std::string reconstruction::getStructureType() const
{
    return this->m_sStructureType;
}

//-----------------------------------------------------------------------------

inline void reconstruction::setStructureType(const std::string& _sStructureType)
{
    this->m_sStructureType = _sStructureType;
}

//-----------------------------------------------------------------------------

inline image::sptr reconstruction::getImage()
{
    return m_image;
}

//-----------------------------------------------------------------------------

inline image::csptr reconstruction::getImage() const
{
    return m_image;
}

//-----------------------------------------------------------------------------

inline void reconstruction::setImage(const image::sptr& val)
{
    m_image = val;
}

//-----------------------------------------------------------------------------

inline mesh::sptr reconstruction::getMesh()
{
    return m_mesh;
}

//-----------------------------------------------------------------------------

inline mesh::csptr reconstruction::getMesh() const
{
    return m_mesh;
}

//-----------------------------------------------------------------------------

inline void reconstruction::setMesh(const mesh::sptr& val)
{
    m_mesh = val;
}

//-----------------------------------------------------------------------------

inline material::sptr reconstruction::getMaterial()
{
    return m_material;
}

//-----------------------------------------------------------------------------

inline material::csptr reconstruction::getMaterial() const
{
    return m_material;
}

//-----------------------------------------------------------------------------

inline void reconstruction::setMaterial(const material::sptr& val)
{
    m_material = val;
}

//-----------------------------------------------------------------------------

inline double reconstruction::getComputedMaskVolume() const
{
    return m_computedMaskVolume;
}

//-----------------------------------------------------------------------------

inline void reconstruction::setComputedMaskVolume(double val)
{
    m_computedMaskVolume = val;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
