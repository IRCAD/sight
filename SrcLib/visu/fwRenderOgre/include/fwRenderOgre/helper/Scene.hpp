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

#ifndef __FWRENDEROGRE_HELPER_SCENE_HPP__
#define __FWRENDEROGRE_HELPER_SCENE_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/SRender.hpp"

#include <OGRE/OgreSceneNode.h>

namespace fwRenderOgre
{

namespace helper
{

class Scene
{
public:

    /**
     * @brief Look for a given scene node in a scene manager.
     * @param[in] _nodeId Identifier of the node
     * @param[in] _sceneMgr Scene manager
     * @return the Ogre scene node, null if not found
     */
    FWRENDEROGRE_API static ::Ogre::SceneNode* getNodeById(::fwRenderOgre::SRender::OgreObjectIdType _nodeId,
                                                           ::Ogre::SceneNode* _sceneNode);
};

} // namespace helper

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_HELPER_SCENE_HPP__

