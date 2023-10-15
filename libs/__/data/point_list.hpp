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
#include "data/point.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>

#include <vector>

namespace sight::data
{

/**
 * @brief   This class defines a list of points.
 * @see     Point
 */
class DATA_CLASS_API point_list final : public object
{
public:

    SIGHT_DECLARE_CLASS(point_list, object);

    using PointListContainer = std::vector<point::sptr>;

    DATA_API point_list();
    DATA_API ~point_list() noexcept override = default;

    /**
     * @brief Gets point vector
     * @return the vector of points
     */
    PointListContainer& getPoints();
    /**
     * @brief Gets point vector
     * @return the vector of points
     */
    const PointListContainer& getPoints() const;
    /**
     * @brief Sets point vector
     * @param[in] _vPoints The vector of points to set
     */
    void setPoints(const PointListContainer& _vPoints);
    /**
     * @brief Adds a Point in the pointlist
     * @param[in] _p The point to push
     */
    void pushBack(const point::sptr& _p);
    /**
     * @brief: Deletes a point at the specified index
     * @param[in] _index Index of point to delete
     **/
    void remove(std::size_t _index);
    /**
     * @brief Clears the list
     */
    void clear();

    /**
     * @name Signals
     * @{
     */
    /**
     * @brief Signal emitted when a Point is added
     */
    typedef core::com::signal<void (point::sptr)> PointAddedSignalType;
    DATA_API static const core::com::signals::key_t POINT_ADDED_SIG;
    /**
     * @brief Signal emitted when a Point is removed
     */
    typedef core::com::signal<void (point::sptr)> PointRemovedSignalType;
    DATA_API static const core::com::signals::key_t POINT_REMOVED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const point_list& other) const noexcept;
    DATA_API bool operator!=(const point_list& other) const noexcept;
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

    /**
     * @brief Points container
     */
    PointListContainer m_vPoints;
}; // end class point_list

//-----------------------------------------------------------------------------

inline point_list::PointListContainer& point_list::getPoints()
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline const point_list::PointListContainer& point_list::getPoints() const
{
    return this->m_vPoints;
}

//-----------------------------------------------------------------------------

inline void point_list::setPoints(const point_list::PointListContainer& _vPoints)
{
    this->m_vPoints = _vPoints;
}

//-----------------------------------------------------------------------------

inline void point_list::pushBack(const point::sptr& _p)
{
    this->m_vPoints.push_back(_p);
}

//-----------------------------------------------------------------------------

inline void point_list::remove(std::size_t _index)
{
    const auto it = m_vPoints.begin() + static_cast<ptrdiff_t>(_index);
    this->m_vPoints.erase(it);
}

//-----------------------------------------------------------------------------

inline void point_list::clear()
{
    this->m_vPoints.clear();
}

DATA_API std::ostream& operator<<(std::ostream& out, const point_list& pl);

} // end namespace sight::data
