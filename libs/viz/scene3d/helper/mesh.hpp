/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2021 IHU Strasbourg
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

#include "viz/scene3d/config.hpp"

#include <OGRE/OgreColourValue.h>

#include <cstdint>

namespace sight::viz::scene3d::helper
{

class mesh
{
public:

    /**
     * @brief Copy the color layer of a mesh.
     * @param _dest[out] destination points
     * @param _src[in] source points
     * @param _num_points[in] number of points
     * @param _num_components[in] number of components, must be 3 or 4
     */
    VIZ_SCENE3D_API static void copy_colors(
        Ogre::RGBA* _dest,
        const std::uint8_t* _src,
        std::size_t _num_points,
        std::size_t _num_components
    );
};

} // namespace sight::viz::scene3d::helper
