/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "viz/scene2d/config.hpp"

namespace sight::viz::scene2d
{

namespace data
{

class VIZ_SCENE2D_CLASS_API Coord
{
public:

    Coord()
    {
        m_x = 0;
        m_y = 0;
    }

    Coord(double x, double y)
    {
        m_x = x;
        m_y = y;
    }

    //------------------------------------------------------------------------------

    double getX() const
    {
        return m_x;
    }

    //------------------------------------------------------------------------------

    void setX(double x)
    {
        m_x = x;
    }

    //------------------------------------------------------------------------------

    double getY() const
    {
        return m_y;
    }

    //------------------------------------------------------------------------------

    void setY(double y)
    {
        m_y = y;
    }

private:

    double m_x;
    double m_y;
};

} // namespace data

} // namespace sight::viz::scene2d
