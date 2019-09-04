/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

    fwCoreClassDefinitionsWithFactoryMacro( (Viewport)(::fwData::Object), ::fwData::factory::New< Viewport >);

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
