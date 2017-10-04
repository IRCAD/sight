/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERQT_DATA_VIEWPORT_HPP__
#define __FWRENDERQT_DATA_VIEWPORT_HPP__

#include "fwRenderQt/config.hpp"

#include <fwData/Exception.hpp>
#include <fwData/Object.hpp>

namespace fwRenderQt
{
namespace data
{

/**
 * @brief Manage the current viewport of the fwRenderQt.
 */
class FWRENDERQT_CLASS_API Viewport : public ::fwData::Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Viewport)(::fwData::Object), (()), ::fwData::factory::New< Viewport >);

    /**
     * @brief Constructor
     */
    Viewport(::fwData::Object::Key key) :
        m_x(200.f),
        m_y(200.f),
        m_width(400.f),
        m_height(400.f)
    {
    }

    /// Defines shallow copy
    FWRENDERQT_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWRENDERQT_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    float getX() const;
    void setX (float _x);
    float getY() const;
    void setY (float _y);

    float getWidth() const;
    void setWidth (float _width);
    float getHeight() const;
    void setHeight (float _height);

private:
    float m_x, m_y, m_width, m_height;

};

//------------------------------------------------------------------------------

inline float Viewport::getX() const
{
    return m_x;
}

//-----------------------------------------------------------------------------

inline void Viewport::setX (float _x)
{
    m_x = _x;
}

//-----------------------------------------------------------------------------

inline float Viewport::getY() const
{
    return m_y;
}

//-----------------------------------------------------------------------------

inline void Viewport::setY (float _y)
{
    m_y = _y;
}

//-----------------------------------------------------------------------------

inline float Viewport::getWidth() const
{
    return m_width;
}

//-----------------------------------------------------------------------------

inline void Viewport::setWidth (float _width)
{
    m_width = _width;
}

//-----------------------------------------------------------------------------

inline float Viewport::getHeight() const
{
    return m_height;
}

//-----------------------------------------------------------------------------

inline void Viewport::setHeight (float _height)
{
    m_height = _height;
}

//-----------------------------------------------------------------------------

} // namespace data
} // namespace fwRenderQt

#endif // __FWRENDERQT_DATA_VIEWPORT_HPP__

