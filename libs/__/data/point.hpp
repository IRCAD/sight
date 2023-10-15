/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/object.hpp"

#include <array>

namespace sight::data
{

/**
 * @brief   This class define a 3D point.
 */
class DATA_CLASS_API point final : public object
{
public:

    typedef double PointCoordType;
    typedef std::array<double, 3> PointCoordArrayType;

    SIGHT_DECLARE_CLASS(point, object);

    /// point factory
    DATA_API point(float x, float y   = 0.F, float z = 0.F);
    DATA_API point(double x, double y = 0., double z = 0.);
    DATA_API point(const PointCoordArrayType& coord);
    DATA_API point(const point::sptr& p);

    DATA_API point();

    /// Destructor
    DATA_API ~point() noexcept override = default;

    /// @brief get/set point coordinates
    /// @{
    PointCoordArrayType& getCoord();
    const PointCoordArrayType& getCoord() const;
    void setCoord(const PointCoordArrayType& _vCoord);
    /// @}

    DATA_API std::string getLabel() const;

    DATA_API void setLabel(const std::string& _label);

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const point& other) const noexcept;
    DATA_API bool operator!=(const point& other) const noexcept;
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

    /// point coordinates
    PointCoordArrayType m_vCoord {};
}; // end class point

//-----------------------------------------------------------------------------

inline point::PointCoordArrayType& point::getCoord()
{
    return this->m_vCoord;
}

//-----------------------------------------------------------------------------

inline const point::PointCoordArrayType& point::getCoord() const
{
    return this->m_vCoord;
}

//-----------------------------------------------------------------------------

inline void point::setCoord(const PointCoordArrayType& _vCoord)
{
    this->m_vCoord = _vCoord;
}

DATA_API std::ostream& operator<<(std::ostream& out, const point& p);

//-----------------------------------------------------------------------------

} // end namespace sight::data
