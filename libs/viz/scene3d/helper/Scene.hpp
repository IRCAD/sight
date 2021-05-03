/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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
#include "viz/scene3d/SRender.hpp"

#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d
{

namespace helper
{

class Scene
{
public:

    /**
     * @brief Looks for a given scene node in a scene manager.
     * @param[in] _nodeId Identifier of the node
     * @param[in] _sceneMgr Scene manager
     * @return the Ogre scene node, null if not found
     */
    VIZ_SCENE3D_API static ::Ogre::SceneNode* getNodeById(viz::scene3d::SRender::OgreObjectIdType _nodeId,
                                                          ::Ogre::SceneNode* _sceneNode);
};

} // namespace helper

} // namespace sight::viz::scene3d
