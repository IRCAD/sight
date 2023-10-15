/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "viz/scene2d/config.hpp"

#include <data/exception.hpp>
#include <data/object.hpp>

#include <optional>

namespace sight::viz::scene2d::data
{

/**
 * @brief Manage the current viewport of the fwRenderQt.
 */
class VIZ_SCENE2D_CLASS_API Viewport : public sight::data::object
{
public:

    SIGHT_DECLARE_CLASS(Viewport, sight::data::object);

    double x() const;
    double y() const;

    double x_or(double /*x*/) const;
    double y_or(double /*y*/) const;

    double width() const;
    double height() const;

    double width_or(double /*width*/) const;
    double height_or(double /*height*/) const;

    double left() const;
    double top() const;
    double right() const;
    double bottom() const;

    void setX(double _x);
    void setY(double _y);
    void setWidth(double _width);
    void setHeight(double _height);

    /// Equality comparison operators
    /// @{
    VIZ_SCENE2D_API bool operator==(const Viewport& other) const noexcept;
    VIZ_SCENE2D_API bool operator!=(const Viewport& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    VIZ_SCENE2D_API void shallow_copy(const sight::data::object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    VIZ_SCENE2D_API void deep_copy(
        const sight::data::object::csptr& source,
        const std::unique_ptr<sight::data::object::deep_copy_cache_t>& cache = std::make_unique<sight::data::object::deep_copy_cache_t>()
    )
    override;

private:

    std::optional<double> m_x;
    std::optional<double> m_y;
    std::optional<double> m_width;
    std::optional<double> m_height;
};

//------------------------------------------------------------------------------

inline double Viewport::x() const
{
    return *m_x;
}

//-----------------------------------------------------------------------------

inline double Viewport::y() const
{
    return *m_y;
}

//------------------------------------------------------------------------------

inline double Viewport::x_or(double x) const
{
    return m_x.value_or(x);
}

//------------------------------------------------------------------------------

inline double Viewport::y_or(double y) const
{
    return m_y.value_or(y);
}

//-----------------------------------------------------------------------------

inline double Viewport::width() const
{
    return *m_width;
}

//-----------------------------------------------------------------------------

inline double Viewport::height() const
{
    return *m_height;
}

//-----------------------------------------------------------------------------

inline double Viewport::width_or(double width) const
{
    return m_width.value_or(width);
}

//-----------------------------------------------------------------------------

inline double Viewport::height_or(double height) const
{
    return m_height.value_or(height);
}

//------------------------------------------------------------------------------

inline double Viewport::left() const
{
    return *m_x;
}

//------------------------------------------------------------------------------

inline double Viewport::top() const
{
    return *m_y;
}

//------------------------------------------------------------------------------

inline double Viewport::right() const
{
    return *m_x + *m_width;
}

//-----------------------------------------------------------------------------
inline double Viewport::bottom() const
{
    return *m_y + *m_height;
}

//-----------------------------------------------------------------------------

inline void Viewport::setX(double _x)
{
    m_x = _x;
}

//-----------------------------------------------------------------------------

inline void Viewport::setY(double _y)
{
    m_y = _y;
}

//-----------------------------------------------------------------------------

inline void Viewport::setWidth(double _width)
{
    m_width = _width;
}

//-----------------------------------------------------------------------------

inline void Viewport::setHeight(double _height)
{
    m_height = _height;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d::data
