/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWRENDERQT_DATA_AXIS_HPP__
#define __FWRENDERQT_DATA_AXIS_HPP__

#include <memory>

namespace fwRenderQt
{
namespace data
{

/**
 * @brief This class manages an axis on the scene2D.
 */
class Axis
{

public:

    typedef std::shared_ptr < Axis > sptr;

    typedef enum
    {
        LINEAR,
        LOG
    }  ScaleType;

    /**
     * @brief Constructor, set origin to 0, scale to 1 and type to LINEAR.
     */
    Axis() :
        m_origin(0.f),
        m_scale(1.f),
        m_scaleType(LINEAR)
    {
    }

    /// Get m_origin attribute.
    float getOrigin() const;

    /// Set m_origin attribute.
    void setOrigin (float origin);

    /// Get m_scale attribute.
    float getScale() const;

    /// Set m_scale attribute.
    void setScale (float scale);

    /// Get m_scaleType attribute.
    ScaleType getScaleType() const;

    /// Set m_scaleType attribute.
    void setScaleType(ScaleType scaleType);

private:
    /// The origin and scale.
    float m_origin, m_scale;

    /// The scaleType (LINEAR/LOG).
    ScaleType m_scaleType;

};

//-----------------------------------------------------------------------------

inline float Axis::getOrigin() const
{
    return m_origin;
}

//-----------------------------------------------------------------------------

inline void Axis::setOrigin (float origin)
{
    m_origin = origin;
}

//-----------------------------------------------------------------------------

inline float Axis::getScale() const
{
    return m_scale;
}

//-----------------------------------------------------------------------------

inline void Axis::setScale (float scale)
{
    m_scale = scale;
}

//-----------------------------------------------------------------------------

inline Axis::ScaleType Axis::getScaleType() const
{
    return m_scaleType;
}

//-----------------------------------------------------------------------------

inline void Axis::setScaleType(ScaleType scaleType)
{
    m_scaleType = scaleType;
}

//-----------------------------------------------------------------------------

} // namespace data
} // namespace fwRenderQt

#endif // __FWRENDERQT_DATA_AXIS_HPP__

