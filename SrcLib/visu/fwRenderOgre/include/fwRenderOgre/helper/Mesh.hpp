/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

