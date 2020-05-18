/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#ifndef __FWRENDEROGRE_HELPER_MESH_HPP__
#define __FWRENDEROGRE_HELPER_MESH_HPP__

#include "fwRenderOgre/config.hpp"

#include <OGRE/OgreColourValue.h>

#include <cstdint>

namespace fwRenderOgre
{

namespace helper
{

class Mesh
{
public:

    /**
     * @brief Copy the color layer of a mesh.
     * @param _dest[out] destination points
     * @param _src[in] source points
     * @param _numPoints[in] number of points
     * @param _numComponents[in] number of components, must be 3 or 4
     */
    FWRENDEROGRE_API static void copyColors(::Ogre::RGBA* _dest, const std::uint8_t* _src, size_t _numPoints,
                                            size_t _numComponents);

};

} // namespace helper

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_HELPER_MESH_HPP__

