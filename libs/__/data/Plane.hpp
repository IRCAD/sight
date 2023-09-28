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
#include "data/Point.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <vector>

namespace sight::data
{

/**
 * @brief   This class defines a plane defined by tree points.
 * @see     Point
 */
class DATA_CLASS_API Plane final : public Object
{
public:

    SIGHT_DECLARE_CLASS(Plane, Object);

    typedef std::array<Point::sptr, 3> PointContainer;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Plane();

    /// Destructor
    DATA_API ~Plane() noexcept override = default;

    /// Re-initialize the plane with 3 points
    DATA_API void setValue(
        Point::sptr _point1,
        Point::sptr _point2,
        Point::sptr _point3
    );

    /** @{
     *  @brief get/set points container
     */
    PointContainer& getPoints();
    const PointContainer& getPoints() const;
    void setPoints(const PointContainer& _vPoints);
    /// @}

    /** @{
     *  @brief get/set flag if the plane is an intersection one (else an union one)
     */
    bool getIsIntersection() const;
    void setIsIntersection(bool _isIntersection);
    /// @}

    /**
     * @name Signals
     * @{
     */
    /// Signal emitted when plane is selected/deselected
    typedef core::com::signal<void (bool)> SelectedSignalType;
    DATA_API static const core::com::signals::key_t SELECTED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Plane& other) const noexcept;
    DATA_API bool operator!=(const Plane& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallow_copy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

protected:

    //! Points container
    PointContainer m_vPoints;

    //! flag if the plane is an intersection (else an union)
    bool m_isIntersection {true};
}; // end class Plane

//-----------------------------------------------------------------------------

inline Plane::PointContainer& Plane::getPoints()
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline const Plane::PointContainer& Plane::getPoints() const
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline void Plane::setPoints(const Plane::PointContainer& _vPoints)
{
    this->m_vPoints = _vPoints;
}

//-----------------------------------------------------------------------------

inline bool Plane::getIsIntersection() const
{
    return m_isIntersection;
}

//-----------------------------------------------------------------------------

inline void Plane::setIsIntersection(bool _isIntersection)
{
    this->m_isIntersection = _isIntersection;
}

//-----------------------------------------------------------------------------

} // end namespace sight::data
