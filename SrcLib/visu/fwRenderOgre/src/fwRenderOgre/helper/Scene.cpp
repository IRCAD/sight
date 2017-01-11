/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/helper/Scene.hpp"

#include <stack>

namespace fwRenderOgre
{

namespace helper
{

//------------------------------------------------------------------------------

::Ogre::SceneNode* Scene::getNodeById(::fwRenderOgre::SRender::OgreObjectIdType _nodeId,
                                      ::Ogre::SceneNode* _sceneNode)
{
    SLM_ASSERT("Scene node is null", _sceneNode);

    ::Ogre::SceneNode* sceneNode                 = nullptr;
    ::Ogre::SceneNode::ChildNodeIterator rootMap = _sceneNode->getChildIterator();

    std::stack< ::Ogre::SceneNode::ChildNodeIterator > stack;
    stack.push(rootMap);

    // Recursive search in the graph
    do
    {
        ::Ogre::SceneNode::ChildNodeIterator map = stack.top();
        stack.pop();
        for(auto it : map)
        {
            if (it.first == _nodeId)
            {
                sceneNode = static_cast< ::Ogre::SceneNode* >(it.second);
                break;
            }
            stack.push(it.second->getChildIterator());
        }
    }
    while( !stack.empty() );

    return sceneNode;
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace fwRenderOgre
