/************************************************************************
 *
 * Copyright (C) 2015-2019 IRCAD France
 * Copyright (C) 2015-2019 IHU Strasbourg
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

    ::Ogre::SceneNode* sceneNode = nullptr;
    const auto& rootMap = _sceneNode->getChildren();

    // Use a vector as a stack to benefit from iterators.
    std::vector< ::Ogre::Node* > stack;
    std::copy(rootMap.cbegin(), rootMap.cend(), std::back_inserter(stack));

    // Recursive search in the graph
    do
    {
        ::Ogre::Node* const topNode = stack.back();

        if (topNode->getName() == _nodeId)
        {
            sceneNode = static_cast< ::Ogre::SceneNode* >(topNode);
            break;
        }

        const ::Ogre::Node::ChildNodeMap& nodeChildren = topNode->getChildren();
        stack.pop_back();

        std::copy(nodeChildren.cbegin(), nodeChildren.cend(), std::back_inserter(stack));
    }
    while( !stack.empty() );

    return sceneNode;
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace fwRenderOgre
