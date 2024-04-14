/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/data/config.hpp>

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
class SIGHT_DATA_CLASS_API reconstruction final : public object
{
public:

    SIGHT_DECLARE_CLASS(reconstruction, object);

    SIGHT_ALLOW_SHARED_FROM_THIS()

    /**
     * @brief Constructor
     */
    SIGHT_DATA_API reconstruction();

    /// Destructor
    SIGHT_DATA_API ~reconstruction() noexcept override = default;

    /// Constant to inform that mask volume has not been computed yet.
    SIGHT_DATA_API static const double NO_COMPUTED_MASK_VOLUME;

    /**
     * @{
     * @brief Get/Set value of the bIsVisible.
     */
    bool get_is_visible() const;
    void set_is_visible(bool _b_is_visible);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the organName.
     */
    std::string get_organ_name() const;
    void set_organ_name(const std::string& _s_organ_name);
    /// @}

    /**
     * @{
     * @brief Get/Set value of the structureType.
     */
    std::string get_structure_type() const;
    void set_structure_type(const std::string& _s_structure_type);
    /// @}

    /**
     * @brief Get/Set the image associated with the acquisition
     */
    image::sptr get_image();
    image::csptr get_image() const;
    void set_image(const image::sptr& _val);

    /**
     * @brief Get/Set the mesh associated with the acquisition
     */
    mesh::sptr get_mesh();
    mesh::csptr get_mesh() const;
    void set_mesh(const mesh::sptr& _val);
    /// @}

    /**
     * @brief Get/Set the material associated with the acquisition
     */
    material::sptr get_material();
    material::csptr get_material() const;
    void set_material(const material::sptr& _val);
    /// @}

    /**
     * @brief Get/Set the volume from mask
     */
    double get_computed_mask_volume() const;
    void set_computed_mask_volume(double _val);
    /// @}

    /***
     * @name Signals
     * @{
     */

    /// Type of signal when the mesh pointer has changed, mesh parameter is used to store old mesh
    using mesh_changed_signal_t = core::com::signal<void (mesh::sptr)>;

    /// Key in m_signals map of signal m_sigMeshModified
    SIGHT_DATA_API static const core::com::signals::key_t MESH_CHANGED_SIG;

    /// Type of signal when mesh is modified
    using visibility_modified_signal_t = core::com::signal<void (bool)>;

    /// Key in m_signals map of signal m_sigVisibilityModified
    SIGHT_DATA_API static const core::com::signals::key_t VISIBILITY_MODIFIED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const reconstruction& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const reconstruction& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    SIGHT_DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    SIGHT_DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    //! true if this reconstruction is visible
    bool m_b_is_visible {false};

    //! Organ name
    std::string m_s_organ_name;

    //! Structure type
    std::string m_s_structure_type;

    //--------------------------------------------------------------------------

    //! reconstruction's material
    material::sptr m_material;

    //! reconstruction's mask
    image::sptr m_image;

    //! reconstruction's mesh
    mesh::sptr m_mesh;

    //! reconstruction's mask volume
    double m_computed_mask_volume;

    /**
     * @name Signals
     * @{
     */

    /// Signal emitted when mesh changed, mesh parameter is used to store old mesh
    mesh_changed_signal_t::sptr m_sig_mesh_changed;

    /// Signal emitted  when visibility is modified
    visibility_modified_signal_t::sptr m_sig_visibility_modified;
    /**
     * @}
     */
};

//-----------------------------------------------------------------------------

inline bool reconstruction::get_is_visible() const
{
    return m_b_is_visible;
}

//-----------------------------------------------------------------------------

inline void reconstruction::set_is_visible(const bool _b_is_visible)
{
    m_b_is_visible = _b_is_visible;
}

//-----------------------------------------------------------------------------

inline std::string reconstruction::get_organ_name() const
{
    return m_s_organ_name;
}

//-----------------------------------------------------------------------------

inline void reconstruction::set_organ_name(const std::string& _s_organ_name)
{
    m_s_organ_name = _s_organ_name;
}

//-----------------------------------------------------------------------------

inline std::string reconstruction::get_structure_type() const
{
    return this->m_s_structure_type;
}

//-----------------------------------------------------------------------------

inline void reconstruction::set_structure_type(const std::string& _s_structure_type)
{
    this->m_s_structure_type = _s_structure_type;
}

//-----------------------------------------------------------------------------

inline image::sptr reconstruction::get_image()
{
    return m_image;
}

//-----------------------------------------------------------------------------

inline image::csptr reconstruction::get_image() const
{
    return m_image;
}

//-----------------------------------------------------------------------------

inline void reconstruction::set_image(const image::sptr& _val)
{
    m_image = _val;
}

//-----------------------------------------------------------------------------

inline mesh::sptr reconstruction::get_mesh()
{
    return m_mesh;
}

//-----------------------------------------------------------------------------

inline mesh::csptr reconstruction::get_mesh() const
{
    return m_mesh;
}

//-----------------------------------------------------------------------------

inline void reconstruction::set_mesh(const mesh::sptr& _val)
{
    m_mesh = _val;
}

//-----------------------------------------------------------------------------

inline material::sptr reconstruction::get_material()
{
    return m_material;
}

//-----------------------------------------------------------------------------

inline material::csptr reconstruction::get_material() const
{
    return m_material;
}

//-----------------------------------------------------------------------------

inline void reconstruction::set_material(const material::sptr& _val)
{
    m_material = _val;
}

//-----------------------------------------------------------------------------

inline double reconstruction::get_computed_mask_volume() const
{
    return m_computed_mask_volume;
}

//-----------------------------------------------------------------------------

inline void reconstruction::set_computed_mask_volume(double _val)
{
    m_computed_mask_volume = _val;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
