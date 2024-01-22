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

    using plane_list_t = std::vector<plane::sptr>;

    /**
     * @brief Constructor
     */
    DATA_API plane_list();

    /// Destructor
    DATA_API ~plane_list() noexcept override = default;

    /** @{
     *  @brief get/set container of all planes
     */
    plane_list_t& get_planes();
    const plane_list_t& get_planes() const;
    void set_planes(const plane_list_t& _v_planes);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when a plane is added
    using plane_added_signal_t = core::com::signal<void (plane::sptr)>;
    DATA_API static const core::com::signals::key_t PLANE_ADDED_SIG;

    /// Signal emitted when a plane is removed
    using plane_removed_signal_t = core::com::signal<void (plane::sptr)>;
    DATA_API static const core::com::signals::key_t PLANE_REMOVED_SIG;

    /// Signal emitted when the visibility changed
    using visibility_modified_signal_t = core::com::signal<void (bool)>;
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
    /// @param[in] _source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    //! Planes container
    plane_list_t m_v_planes;
}; // end class plane_list

//-----------------------------------------------------------------------------

inline plane_list::plane_list_t& plane_list::get_planes()
{
    return this->m_v_planes;
}

//-----------------------------------------------------------------------------

inline const plane_list::plane_list_t& plane_list::get_planes() const
{
    return this->m_v_planes;
}

//-----------------------------------------------------------------------------

inline void plane_list::set_planes(const plane_list::plane_list_t& _v_planes)
{
    this->m_v_planes = _v_planes;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
