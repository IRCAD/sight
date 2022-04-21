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

    Viewport() = default;
    Viewport(sight::data::Object::Key)
    {
    }

    /// Defines shallow copy
    VIZ_SCENE2D_API void shallowCopy(const sight::data::Object::csptr& _source) override;

    double getX() const;
    void setX(double _x);
    double getY() const;
    void setY(double _y);

    double getWidth() const;
    void setWidth(double _width);
    double getHeight() const;
    void setHeight(double _height);

    /// Equality comparison operators
    /// @{
    VIZ_SCENE2D_API bool operator==(const Viewport& other) const noexcept;
    VIZ_SCENE2D_API bool operator!=(const Viewport& other) const noexcept;
    /// @}

protected:

    /// Defines deep copy
    VIZ_SCENE2D_API void cachedDeepCopy(const sight::data::Object::csptr& _source, DeepCopyCacheType& cache) override;

private:

    double m_x {0};
    double m_y {0};
    double m_width {0};
    double m_height {0};
};

//------------------------------------------------------------------------------

inline double Viewport::getX() const
{
    return m_x;
}

//-----------------------------------------------------------------------------

inline void Viewport::setX(double _x)
{
    m_x = _x;
}

//-----------------------------------------------------------------------------

inline double Viewport::getY() const
{
    return m_y;
}

//-----------------------------------------------------------------------------

inline void Viewport::setY(double _y)
{
    m_y = _y;
}

//-----------------------------------------------------------------------------

inline double Viewport::getWidth() const
{
    return m_width;
}

//-----------------------------------------------------------------------------

inline void Viewport::setWidth(double _width)
{
    m_width = _width;
}

//-----------------------------------------------------------------------------

inline double Viewport::getHeight() const
{
    return m_height;
}

//-----------------------------------------------------------------------------

inline void Viewport::setHeight(double _height)
{
    m_height = _height;
}

//-----------------------------------------------------------------------------

} // namespace data

} // namespace sight::viz::scene2d
