/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <data/image.hpp>

#include <OGRE/OgreAxisAlignedBox.h>

namespace sight::viz::scene3d::helper
{

class image
{
public:

    /**
     * @brief Compute the bounding box of an image mask.
     * @param _mask[in] mask
     */
    static VIZ_SCENE3D_API Ogre::AxisAlignedBox compute_bounding_box_from_mask(const data::image::csptr _mask);
};

} // namespace sight::viz::scene3d::helper
