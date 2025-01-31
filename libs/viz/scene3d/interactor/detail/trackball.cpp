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

#include "viz/scene3d/interactor/detail/trackball.hpp"

#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreViewport.h>

namespace sight::viz::scene3d::interactor::detail
{

//------------------------------------------------------------------------------

void camera_rotate(Ogre::Camera* _camera, int _dx, int _dy, float _look_at_z)
{
    auto w_delta = static_cast<Ogre::Real>(_dx);
    auto h_delta = static_cast<Ogre::Real>(_dy);

    Ogre::SceneNode* const cam_node = _camera->getParentSceneNode();
    const Ogre::Viewport* const vp  = _camera->getViewport();

    const auto height = static_cast<float>(vp->getActualHeight());
    const auto width  = static_cast<float>(vp->getActualWidth());

    // Rotate around the up vector of our world according to the dx of the mouse
    {
        const Ogre::Vector3 view_up(0., -1.0, 0.);

        // 1 - Move to the center of the target
        cam_node->translate(Ogre::Vector3(0.F, 0.F, -_look_at_z), Ogre::Node::TS_LOCAL);

        // 2 - Compute the angle so that we can rotate around 360 degrees by sliding the whole window
        const float angle = (w_delta * 2 * Ogre::Math::PI / width);

        // 3 - Apply the rotation on the scene node
        Ogre::Quaternion rotate(Ogre::Radian(angle), view_up);
        cam_node->rotate(rotate, Ogre::Node::TS_WORLD);

        // 4 - Go backward in the inverse direction
        cam_node->translate(Ogre::Vector3(0.F, 0.F, _look_at_z), Ogre::Node::TS_LOCAL);
    }

    // Rotate around the right vector according to the dy of the mouse
    {
        // Be sure to grab the orientation again because we may already have rotated around the view_up
        const Ogre::Quaternion orientation = cam_node->getOrientation();
        const Ogre::Vector3 view_right     = orientation.xAxis();

        // 1 - Move to the center of the target
        cam_node->translate(Ogre::Vector3(0.F, 0.F, -_look_at_z), Ogre::Node::TS_LOCAL);

        // 2 - Compute the angle so that we can rotate around 360 degrees by sliding the whole window
        const float angle = (h_delta * 2 * Ogre::Math::PI / height);

        // 3 - Apply the rotation on the scene node
        Ogre::Quaternion rotate(Ogre::Radian(angle), view_right);
        cam_node->rotate(rotate, Ogre::Node::TS_WORLD);

        // 4 - Go backward in the inverse direction
        cam_node->translate(Ogre::Vector3(0.F, 0.F, _look_at_z), Ogre::Node::TS_LOCAL);
    }
}

} // namespace sight::viz::scene3d::interactor::detail
