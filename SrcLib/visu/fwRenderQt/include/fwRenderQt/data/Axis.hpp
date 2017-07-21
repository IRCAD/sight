/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERQT_DATA_AXIS_HPP__
#define __FWRENDERQT_DATA_AXIS_HPP__

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

