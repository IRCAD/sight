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

#include <memory>

namespace sight::viz::scene2d::data
{

/**
 * @brief This class manages an axis on the scene2D.
 */
class axis
{
public:

    using sptr = std::shared_ptr<axis>;

    enum scale_t
    {
        linear,
        log
    };

    /**
     * @brief Constructor, set origin to 0, scale to 1 and type to LINEAR.
     */
    axis()
    = default;

    /// Get m_origin attribute.
    [[nodiscard]] double origin() const;

    /// Set m_origin attribute.
    void set_origin(double _origin);

    /// Get m_scale attribute.
    [[nodiscard]] double get_scale() const;

    /// Set m_scale attribute.
    void set_scale(double _scale);

    /// Get m_scaleType attribute.
    [[nodiscard]] scale_t get_scale_type() const;

    /// Set m_scaleType attribute.
    void set_scale_type(scale_t _scale_type);

private:

    /// The origin and scale.
    double m_origin {0.F}, m_scale {1.F};

    /// The scaleType (LINEAR/LOG).
    scale_t m_scale_type {linear};
};

//-----------------------------------------------------------------------------

inline double axis::origin() const
{
    return m_origin;
}

//-----------------------------------------------------------------------------

inline void axis::set_origin(double _origin)
{
    m_origin = _origin;
}

//-----------------------------------------------------------------------------

inline double axis::get_scale() const
{
    return m_scale;
}

//-----------------------------------------------------------------------------

inline void axis::set_scale(double _scale)
{
    m_scale = _scale;
}

//-----------------------------------------------------------------------------

inline axis::scale_t axis::get_scale_type() const
{
    return m_scale_type;
}

//-----------------------------------------------------------------------------

inline void axis::set_scale_type(scale_t _scale_type)
{
    m_scale_type = _scale_type;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d::data
