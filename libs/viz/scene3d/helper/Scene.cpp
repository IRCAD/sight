/************************************************************************
 *
 * Copyright (C) 2015-2023 IRCAD France
 * Copyright (C) 2015-2021 IHU Strasbourg
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

#include "viz/scene3d/helper/Scene.hpp"

#include "Camera.hpp"

#include <stack>

namespace sight::viz::scene3d::helper
{

//------------------------------------------------------------------------------

Ogre::SceneNode* Scene::getNodeById(
    viz::scene3d::SRender::OgreObjectIdType _nodeId,
    Ogre::SceneNode* _sceneNode
)
{
    SIGHT_ASSERT("Scene node is null", _sceneNode);

    Ogre::SceneNode* foundNode = nullptr;

    // Use a vector as a stack to benefit from iterators.
    std::vector<Ogre::Node*> stack;
    stack.push_back(_sceneNode);

    // Recursive search in the graph
    while(!stack.empty())
    {
        Ogre::Node* const topNode = stack.back();

        if(topNode->getName() == _nodeId)
        {
            foundNode = dynamic_cast<Ogre::SceneNode*>(topNode);
            SIGHT_ASSERT("'" + _nodeId + "' is not a scene node.", foundNode);
            break;
        }

        const Ogre::Node::ChildNodeMap& nodeChildren = topNode->getChildren();
        stack.pop_back();

        std::copy(nodeChildren.cbegin(), nodeChildren.cend(), std::back_inserter(stack));
    }

    return foundNode;
}

//------------------------------------------------------------------------------

Ogre::AxisAlignedBox Scene::computeBoundingBox(const Ogre::SceneNode* rootSceneNode)
{
    // The bounding box in which all the object's bounding boxes will be merged
    Ogre::AxisAlignedBox worldCoordBoundingBox;

    // This stack will be used for iterate through the scene nodes graph
    std::stack<const Ogre::SceneNode*> childrenStack;
    childrenStack.push(rootSceneNode);

    while(!childrenStack.empty())
    {
        const Ogre::SceneNode* tempSceneNode = childrenStack.top();
        childrenStack.pop();

        // Retrieves an iterator pointing to the attached movable objects of the current scene node
        const Ogre::SceneNode::ObjectMap& entities = tempSceneNode->getAttachedObjects();
        for(auto* const movable : entities)
        {
            if(const Ogre::Entity* entity = dynamic_cast<Ogre::Entity*>(movable))
            {
                if(entity->isVisible())
                {
                    worldCoordBoundingBox.merge(entity->getWorldBoundingBox());
                }
            }
            else if(const Ogre::ManualObject* manualObject = dynamic_cast<Ogre::ManualObject*>(movable))
            {
                if(manualObject->isVisible())
                {
                    worldCoordBoundingBox.merge(manualObject->getWorldBoundingBox());
                }
            }
            else if(const Ogre::Camera* cameraObject = dynamic_cast<Ogre::Camera*>(movable))
            {
                if(cameraObject->isDebugDisplayEnabled())
                {
                    worldCoordBoundingBox.merge(cameraObject->getWorldBoundingBox());
                }
            }
        }

        for(auto* const childNode : tempSceneNode->getChildren())
        {
            // First, we must cast the Node* into a SceneNode*
            const Ogre::SceneNode* childSceneNode = dynamic_cast<Ogre::SceneNode*>(childNode);
            if(childSceneNode != nullptr)
            {
                // Push the current node into the stack in order to continue iteration
                childrenStack.push(childSceneNode);
            }
        }
    }

    return worldCoordBoundingBox;
}

//------------------------------------------------------------------------------

std::pair<Ogre::Vector2, Ogre::Vector2> Scene::computeBoundingRect(
    const Ogre::Camera& camera,
    const Ogre::SceneNode* rootSceneNode
)
{
    const Ogre::AxisAlignedBox boundingBox = computeBoundingBox(rootSceneNode);
    if(!boundingBox.isFinite())
    {
        const Ogre::Vector2 screenPos = Camera::convertWorldSpaceToScreenSpace(
            camera,
            rootSceneNode->_getDerivedPosition()
        );
        return {{screenPos.x, screenPos.y}, {screenPos.x, screenPos.y}};
    }

    const std::array corners                = boundingBox.getAllCorners();
    static constexpr std::size_t NB_CORNERS = std::tuple_size_v<decltype(corners)>;
    std::array<Ogre::Vector2, NB_CORNERS> cornersScreenPos;
    std::ranges::transform(
        corners,
        cornersScreenPos.begin(),
        [&camera](const Ogre::Vector3& worldPos)
        {
            return sight::viz::scene3d::helper::Camera::convertWorldSpaceToScreenSpace(camera, worldPos);
        });
    std::array<float, NB_CORNERS> screenX {};
    std::array<float, NB_CORNERS> screenY {};
    for(std::size_t i = 0 ; i < cornersScreenPos.size() ; i++)
    {
        screenX[i] = cornersScreenPos[i].x;
        screenY[i] = cornersScreenPos[i].y;
    }

    return {{std::ranges::min(screenX), std::ranges::min(screenY)
    }, {std::ranges::max(screenX), std::ranges::max(screenY)
        }
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
