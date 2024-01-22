/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
class VIZ_SCENE2D_CLASS_API viewport : public sight::data::object
{
public:

    SIGHT_DECLARE_CLASS(viewport, sight::data::object);

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

    void set_x(double _x);
    void set_y(double _y);
    void set_width(double _width);
    void set_height(double _height);

    /// Equality comparison operators
    /// @{
    VIZ_SCENE2D_API bool operator==(const viewport& _other) const noexcept;
    VIZ_SCENE2D_API bool operator!=(const viewport& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    VIZ_SCENE2D_API void shallow_copy(const sight::data::object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    VIZ_SCENE2D_API void deep_copy(
        const sight::data::object::csptr& _source,
        const std::unique_ptr<sight::data::object::deep_copy_cache_t>& _cache = std::make_unique<sight::data::object::deep_copy_cache_t>()
    )
    override;

private:

    std::optional<double> m_x;
    std::optional<double> m_y;
    std::optional<double> m_width;
    std::optional<double> m_height;
};

//------------------------------------------------------------------------------

inline double viewport::x() const
{
    return *m_x;
}

//-----------------------------------------------------------------------------

inline double viewport::y() const
{
    return *m_y;
}

//------------------------------------------------------------------------------

inline double viewport::x_or(double _x) const
{
    return m_x.value_or(_x);
}

//------------------------------------------------------------------------------

inline double viewport::y_or(double _y) const
{
    return m_y.value_or(_y);
}

//-----------------------------------------------------------------------------

inline double viewport::width() const
{
    return *m_width;
}

//-----------------------------------------------------------------------------

inline double viewport::height() const
{
    return *m_height;
}

//-----------------------------------------------------------------------------

inline double viewport::width_or(double _width) const
{
    return m_width.value_or(_width);
}

//-----------------------------------------------------------------------------

inline double viewport::height_or(double _height) const
{
    return m_height.value_or(_height);
}

//------------------------------------------------------------------------------

inline double viewport::left() const
{
    return *m_x;
}

//------------------------------------------------------------------------------

inline double viewport::top() const
{
    return *m_y;
}

//------------------------------------------------------------------------------

inline double viewport::right() const
{
    return *m_x + *m_width;
}

//-----------------------------------------------------------------------------
inline double viewport::bottom() const
{
    return *m_y + *m_height;
}

//-----------------------------------------------------------------------------

inline void viewport::set_x(double _x)
{
    m_x = _x;
}

//-----------------------------------------------------------------------------

inline void viewport::set_y(double _y)
{
    m_y = _y;
}

//-----------------------------------------------------------------------------

inline void viewport::set_width(double _width)
{
    m_width = _width;
}

//-----------------------------------------------------------------------------

inline void viewport::set_height(double _height)
{
    m_height = _height;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d::data
