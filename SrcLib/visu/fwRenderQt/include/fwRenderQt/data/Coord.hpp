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

#ifndef __FWRENDERQT_DATA_COORD_HPP__
#define __FWRENDERQT_DATA_COORD_HPP__

#include "fwRenderQt/config.hpp"

namespace fwRenderQt
{
namespace data
{

class FWRENDERQT_CLASS_API Coord
{
public:

    Coord()
    {
        m_x = 0; m_y = 0;
    }
    Coord( double x, double y )
    {
        m_x = x; m_y = y;
    }
    //------------------------------------------------------------------------------

    double getX() const
    {
        return m_x;
    }
    //------------------------------------------------------------------------------

    void setX ( double x )
    {
        m_x = x;
    }
    //------------------------------------------------------------------------------

    double getY() const
    {
        return m_y;
    }
    //------------------------------------------------------------------------------

    void setY ( double y )
    {
        m_y = y;
    }

private:

    double m_x;
    double m_y;
};

} // namespace data
} // namespace fwRenderQt

#endif // __FWRENDERQT_DATA_COORD_HPP__

