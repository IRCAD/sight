/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "viz/scene2d/config.hpp"

#include <data/Object.hpp>

namespace sight::viz::scene2d
{

namespace data
{

class VIZ_SCENE2D_CLASS_API Size
{
public:

    Size() :
        m_w(0.),
        m_h(0.)
    {
    }

    Size(double w, double h)
    {
        m_w = w;
        m_h = h;
    }

    //------------------------------------------------------------------------------

    double getWidth() const
    {
        return m_w;
    }

    //------------------------------------------------------------------------------

    void setWidth(double w)
    {
        m_w = w;
    }

    //------------------------------------------------------------------------------

    double getHeight() const
    {
        return m_h;
    }

    //------------------------------------------------------------------------------

    void setHeight(double h)
    {
        m_h = h;
    }

private:

    double m_w;
    double m_h;
};

} // namespace data

} // namespace sight::viz::scene2d
