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
class SIGHT_DATA_CLASS_API point_list final : public object
{
public:

    SIGHT_DECLARE_CLASS(point_list, object);

    using container_t = std::vector<point::sptr>;

    SIGHT_DATA_API point_list();
    SIGHT_DATA_API ~point_list() noexcept override = default;

    /**
     * @brief Gets point vector
     * @return the vector of points
     */
    container_t& get_points();
    /**
     * @brief Gets point vector
     * @return the vector of points
     */
    const container_t& get_points() const;
    /**
     * @brief Sets point vector
     * @param[in] _v_points The vector of points to set
     */
    void set_points(const container_t& _v_points);
    /**
     * @brief Adds a Point in the pointlist
     * @param[in] _p The point to push
     */
    void push_back(const point::sptr& _p);
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
    using point_added_signal_t = core::com::signal<void (point::sptr)>;
    SIGHT_DATA_API static const core::com::signals::key_t POINT_ADDED_SIG;
    /**
     * @brief Signal emitted when a Point is removed
     */
    using point_removed_signal_t = core::com::signal<void (point::sptr)>;
    SIGHT_DATA_API static const core::com::signals::key_t POINT_REMOVED_SIG;
/**
 * @}
 */

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const point_list& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const point_list& _other) const noexcept;
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

    /**
     * @brief Points container
     */
    container_t m_v_points;
}; // end class point_list

//-----------------------------------------------------------------------------

inline point_list::container_t& point_list::get_points()
{
    return this->m_v_points;
}

//-----------------------------------------------------------------------------

inline const point_list::container_t& point_list::get_points() const
{
    return this->m_v_points;
}

//-----------------------------------------------------------------------------

inline void point_list::set_points(const point_list::container_t& _v_points)
{
    this->m_v_points = _v_points;
}

//-----------------------------------------------------------------------------

inline void point_list::push_back(const point::sptr& _p)
{
    this->m_v_points.push_back(_p);
}

//-----------------------------------------------------------------------------

inline void point_list::remove(std::size_t _index)
{
    const auto it = m_v_points.begin() + static_cast<ptrdiff_t>(_index);
    this->m_v_points.erase(it);
}

//-----------------------------------------------------------------------------

inline void point_list::clear()
{
    this->m_v_points.clear();
}

SIGHT_DATA_API std::ostream& operator<<(std::ostream& _out, const point_list& _pl);

} // end namespace sight::data
