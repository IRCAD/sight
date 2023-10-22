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
class Axis
{
public:

    typedef std::shared_ptr<Axis> sptr;

    typedef enum
    {
        LINEAR,
        LOG
    } scale_t;

    /**
     * @brief Constructor, set origin to 0, scale to 1 and type to LINEAR.
     */
    Axis()
    = default;

    /// Get m_origin attribute.
    [[nodiscard]] double getOrigin() const;

    /// Set m_origin attribute.
    void setOrigin(double _origin);

    /// Get m_scale attribute.
    [[nodiscard]] double getScale() const;

    /// Set m_scale attribute.
    void setScale(double _scale);

    /// Get m_scaleType attribute.
    [[nodiscard]] scale_t get_scale_type() const;

    /// Set m_scaleType attribute.
    void set_scale_type(scale_t _scale_type);

private:

    /// The origin and scale.
    double m_origin {0.F}, m_scale {1.F};

    /// The scaleType (LINEAR/LOG).
    scale_t m_scaleType {LINEAR};
};

//-----------------------------------------------------------------------------

inline double Axis::getOrigin() const
{
    return m_origin;
}

//-----------------------------------------------------------------------------

inline void Axis::setOrigin(double _origin)
{
    m_origin = _origin;
}

//-----------------------------------------------------------------------------

inline double Axis::getScale() const
{
    return m_scale;
}

//-----------------------------------------------------------------------------

inline void Axis::setScale(double _scale)
{
    m_scale = _scale;
}

//-----------------------------------------------------------------------------

inline Axis::scale_t Axis::get_scale_type() const
{
    return m_scaleType;
}

//-----------------------------------------------------------------------------

inline void Axis::set_scale_type(scale_t _scale_type)
{
    m_scaleType = _scale_type;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d::data
