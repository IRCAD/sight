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
class DATA_CLASS_API plane final : public object
{
public:

    SIGHT_DECLARE_CLASS(plane, object);

    typedef std::array<point::sptr, 3> PointContainer;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API plane();

    /// Destructor
    DATA_API ~plane() noexcept override = default;

    /// Re-initialize the plane with 3 points
    DATA_API void setValue(
        point::sptr _point1,
        point::sptr _point2,
        point::sptr _point3
    );

    /** @{
     *  @brief get/set points container
     */
    PointContainer& getPoints();
    const PointContainer& getPoints() const;
    void setPoints(const PointContainer& _v_points);
    /// @}

    /** @{
     *  @brief get/set flag if the plane is an intersection one (else an union one)
     */
    bool getIsIntersection() const;
    void setIsIntersection(bool _is_intersection);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when plane is selected/deselected
    typedef core::com::signal<void (bool)> selected_signal_t;
    DATA_API static const core::com::signals::key_t SELECTED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const plane& _other) const noexcept;
    DATA_API bool operator!=(const plane& _other) const noexcept;
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

    //! Points container
    PointContainer m_vPoints;

    //! flag if the plane is an intersection (else an union)
    bool m_isIntersection {true};
}; // end class plane

//-----------------------------------------------------------------------------

inline plane::PointContainer& plane::getPoints()
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline const plane::PointContainer& plane::getPoints() const
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline void plane::setPoints(const plane::PointContainer& _v_points)
{
    this->m_vPoints = _v_points;
}

//-----------------------------------------------------------------------------

inline bool plane::getIsIntersection() const
{
    return m_isIntersection;
}

//-----------------------------------------------------------------------------

inline void plane::setIsIntersection(bool _is_intersection)
{
    this->m_isIntersection = _is_intersection;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
