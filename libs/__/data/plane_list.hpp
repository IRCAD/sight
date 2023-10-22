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
#include "data/object.hpp"
#include "data/plane.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <vector>

namespace sight::data
{

/**
 * @brief   This class defines a list of planes.
 * @see     Point
 */
class DATA_CLASS_API plane_list final : public object
{
public:

    SIGHT_DECLARE_CLASS(plane_list, object);

    typedef std::vector<plane::sptr> PlaneListContainer;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API plane_list();

    /// Destructor
    DATA_API ~plane_list() noexcept override = default;

    /** @{
     *  @brief get/set container of all planes
     */
    PlaneListContainer& getPlanes();
    const PlaneListContainer& getPlanes() const;
    void setPlanes(const PlaneListContainer& _v_planes);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when a plane is added
    typedef core::com::signal<void (plane::sptr)> plane_added_signal_t;
    DATA_API static const core::com::signals::key_t PLANE_ADDED_SIG;

    /// Signal emitted when a plane is removed
    typedef core::com::signal<void (plane::sptr)> plane_removed_signal_t;
    DATA_API static const core::com::signals::key_t PLANE_REMOVED_SIG;

    /// Signal emitted when the visibility changed
    typedef core::com::signal<void (bool)> visibility_modified_signal_t;
    DATA_API static const core::com::signals::key_t VISIBILITY_MODIFIED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const plane_list& _other) const noexcept;
    DATA_API bool operator!=(const plane_list& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    //! Planes container
    PlaneListContainer m_vPlanes;
}; // end class plane_list

//-----------------------------------------------------------------------------

inline plane_list::PlaneListContainer& plane_list::getPlanes()
{
    return this->m_vPlanes;
}

//-----------------------------------------------------------------------------

inline const plane_list::PlaneListContainer& plane_list::getPlanes() const
{
    return this->m_vPlanes;
}

//-----------------------------------------------------------------------------

inline void plane_list::setPlanes(const plane_list::PlaneListContainer& _v_planes)
{
    this->m_vPlanes = _v_planes;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
