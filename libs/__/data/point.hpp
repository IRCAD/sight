/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <array>

namespace sight::data
{

/**
 * @brief   This class define a 3D point.
 */
class SIGHT_DATA_CLASS_API point final : public object
{
public:

    using point_coord_t       = double;
    using point_coord_array_t = std::array<double, 3>;

    SIGHT_DECLARE_CLASS(point, object);

    /// point factory
    SIGHT_DATA_API point(float _x, float _y   = 0.F, float _z = 0.F);
    SIGHT_DATA_API point(double _x, double _y = 0., double _z = 0.);
    SIGHT_DATA_API point(const point_coord_array_t& _coord);
    SIGHT_DATA_API point(const point::sptr& _p);

    SIGHT_DATA_API point();

    /// Destructor
    SIGHT_DATA_API ~point() noexcept override = default;

    /// @brief get/set point coordinates
    /// @{
    point_coord_array_t& get_coord();
    const point_coord_array_t& get_coord() const;
    void set_coord(const point_coord_array_t& _v_coord);
    /// @}

    SIGHT_DATA_API std::string get_label() const;

    SIGHT_DATA_API void set_label(const std::string& _label);

    /// Equality comparison operators
    /// @{
    SIGHT_DATA_API bool operator==(const point& _other) const noexcept;
    SIGHT_DATA_API bool operator!=(const point& _other) const noexcept;
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

    /// point coordinates
    point_coord_array_t m_v_coord {};
}; // end class point

//-----------------------------------------------------------------------------

inline point::point_coord_array_t& point::get_coord()
{
    return this->m_v_coord;
}

//-----------------------------------------------------------------------------

inline const point::point_coord_array_t& point::get_coord() const
{
    return this->m_v_coord;
}

//-----------------------------------------------------------------------------

inline void point::set_coord(const point_coord_array_t& _v_coord)
{
    this->m_v_coord = _v_coord;
}

SIGHT_DATA_API std::ostream& operator<<(std::ostream& _out, const point& _p);

//-----------------------------------------------------------------------------

} // end namespace sight::data
