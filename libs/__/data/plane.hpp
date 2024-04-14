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
#include "data/point.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <vector>

namespace sight::data
{

/**
 * @brief   This class defines a plane defined by tree points.
 * @see     Point
 */
class SIGHT_DATA_CLASS_API plane final : public object
{
public:

    SIGHT_DECLARE_CLASS(plane, object);

    using point_container = std::array<point::sptr, 3>;

    /**
     * @brief Constructor
     */
    SIGHT_DATA_API plane();

    /// Destructor
    SIGHT_DATA_API ~plane() noexcept override = default;

    /// Re-initialize the plane with 3 points
    SIGHT_DATA_API void set_value(
        point::sptr _point1,
        point::sptr _point2,
        point::sptr _point3
    );

    /** @{
     *  @brief get/set points container
     */
    point_container& get_points();
    const point_container& get_points() const;
    void set_points(const point_container& _v_points);
    /// @}

    /** @{
     *  @brief get/set flag if the plane is an intersection one (else an union one)
     */
    bool get_is_intersection() const;
    void set_is_intersection(bool _is_intersection);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when plane is selected/deselected
    using selected_signal_t = core::com::signal<void (bool)>;
    SIGHT_DATA_API static const core::com::signals::key_t SELECTED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const plane& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const plane& _other) const noexcept;
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

    //! Points container
    point_container m_v_points;

    //! flag if the plane is an intersection (else an union)
    bool m_is_intersection {true};
}; // end class plane

//-----------------------------------------------------------------------------

inline plane::point_container& plane::get_points()
{
    return this->m_v_points;
}

//-----------------------------------------------------------------------------

inline const plane::point_container& plane::get_points() const
{
    return this->m_v_points;
}

//-----------------------------------------------------------------------------

inline void plane::set_points(const plane::point_container& _v_points)
{
    this->m_v_points = _v_points;
}

//-----------------------------------------------------------------------------

inline bool plane::get_is_intersection() const
{
    return m_is_intersection;
}

//-----------------------------------------------------------------------------

inline void plane::set_is_intersection(bool _is_intersection)
{
    this->m_is_intersection = _is_intersection;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
