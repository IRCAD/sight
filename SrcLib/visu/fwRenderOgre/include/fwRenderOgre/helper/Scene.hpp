/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

