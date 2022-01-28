/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <data/Exception.hpp>
#include <data/Object.hpp>

namespace sight::viz::scene2d
{

namespace data
{

/**
 * @brief Manage the current viewport of the fwRenderQt.
 */
class VIZ_SCENE2D_CLASS_API Viewport : public sight::data::Object
{
public:

    SIGHT_DECLARE_CLASS(Viewport, sight::data::Object, sight::data::factory::New<Viewport>);

    /**
     * @brief Constructor
     */
    Viewport(sight::data::Object::Key) :
        m_x(200.f),
        m_y(200.f),
        m_width(400.f),
        m_height(400.f)
    {
    }

    /// Defines shallow copy
    VIZ_SCENE2D_API void shallowCopy(const sight::data::Object::csptr& _source) override;

    float getX() const;
    void setX(float _x);
    float getY() const;
    void setY(float _y);

    float getWidth() const;
    void setWidth(float _width);
    float getHeight() const;
    void setHeight(float _height);

    /// Equality comparison operators
    /// @{
    VIZ_SCENE2D_API bool operator==(const Viewport& other) const noexcept;
    VIZ_SCENE2D_API bool operator!=(const Viewport& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    VIZ_SCENE2D_API void cachedDeepCopy(const sight::data::Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    float m_x, m_y, m_width, m_height;
};

//------------------------------------------------------------------------------

inline float Viewport::getX() const
{
    return m_x;
}

//-----------------------------------------------------------------------------

inline void Viewport::setX(float _x)
{
    m_x = _x;
}

//-----------------------------------------------------------------------------

inline float Viewport::getY() const
{
    return m_y;
}

//-----------------------------------------------------------------------------

inline void Viewport::setY(float _y)
{
    m_y = _y;
}

//-----------------------------------------------------------------------------

inline float Viewport::getWidth() const
{
    return m_width;
}

//-----------------------------------------------------------------------------

inline void Viewport::setWidth(float _width)
{
    m_width = _width;
}

//-----------------------------------------------------------------------------

inline float Viewport::getHeight() const
{
    return m_height;
}

//-----------------------------------------------------------------------------

inline void Viewport::setHeight(float _height)
{
    m_height = _height;
}

//-----------------------------------------------------------------------------

} // namespace data

} // namespace sight::viz::scene2d
