/************************************************************************
 *
 * Copyright (C) 2025 IRCAD France
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

#include <OGRE/OgreCamera.h>

namespace sight::viz::scene3d::interactor::detail
{

//------------------------------------------------------------------------------

/// @brief Compute the rotation of a camera on a trackball
/// @param _camera Ogre camera instance
/// @param _dx Relative movement in pixels, in the x axis
/// @param _dy Relative movement in pixels, in the y axis
/// @param _look_at_z Distance to the center of trackball
void camera_rotate(Ogre::Camera* _camera, int _dx, int _dy, float _look_at_z);

} // namespace sight::viz::scene3d::interactor::detail
