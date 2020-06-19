/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWMATH_INTRASECTYPES_HPP__
#define __FWMATH_INTRASECTYPES_HPP__

#include <vector>
#include <array>

/**
 * @brief Definition of a type for a vector of tree double.
 */
typedef std::array<double, 3> fwVec3d;

/**
 * @brief Definition of a type for a line defined by a position and a direction
 */
typedef std::pair<fwVec3d, fwVec3d> fwLine;

/**
 * @brief Definition of type for a plane defined by a normal and a distance
 */
typedef std::array<double, 4> fwPlane;

/**
 * @brief Definition of a type for a matrix 4*4
 */
typedef std::array< std::array< double,4 >, 4> fwMatrix4x4;

/**
 * @brief Definition of a type for vertex positions
 */
typedef std::vector < std::vector <float> > fwVertexPosition;

/**
 * @brief Definition of a type for vertex index
 */
typedef std::vector < std::vector <int> > fwVertexIndex;


#endif /* __FWMATH_INTRASECTYPES_HPP__ */
