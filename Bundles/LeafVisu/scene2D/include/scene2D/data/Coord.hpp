/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _SCENE2D_DATA_COORD_HPP_
#define _SCENE2D_DATA_COORD_HPP_

#include <fwData/Object.hpp>

#include "scene2D/config.hpp"

namespace scene2D
{
namespace data
{

class SCENE2D_CLASS_API Coord
{
public :

    SCENE2D_API Coord(){m_x=0;m_y=0;}
    SCENE2D_API Coord( double x , double y ){m_x=x;m_y=y;}
    SCENE2D_API double getX() const {return m_x;}
    SCENE2D_API void setX ( double x ){m_x=x;}
    SCENE2D_API double getY() const {return m_y;}
    SCENE2D_API void setY ( double y ){m_y=y;}

private :

    double m_x;
    double m_y;
};

} // namespace data
} // namespace scene2D


#endif // _SCENE2D_DATA_COORD_HPP_

