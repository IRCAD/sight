/************************************************************************
 *
 * Copyright (C) 2015-2025 IRCAD France
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

#include "viz/scene3d/helper/scene.hpp"

#include "camera.hpp"

#include <stack>

namespace sight::viz::scene3d::helper
{

//------------------------------------------------------------------------------

Ogre::SceneNode* scene::get_node_by_id(
    viz::scene3d::render::ogre_object_id_t _node_id,
    Ogre::SceneNode* _scene_node
)
{
    SIGHT_ASSERT("scene node is null", _scene_node);

    Ogre::SceneNode* found_node = nullptr;

    // Use a vector as a stack to benefit from iterators.
    std::vector<Ogre::Node*> stack;
    stack.push_back(_scene_node);

    // Recursive search in the graph
    while(!stack.empty())
    {
        Ogre::Node* const top_node = stack.back();

        if(top_node->getName() == _node_id)
        {
            found_node = dynamic_cast<Ogre::SceneNode*>(top_node);
            SIGHT_ASSERT("'" + _node_id + "' is not a scene node.", found_node);
            break;
        }

        const Ogre::Node::ChildNodeMap& node_children = top_node->getChildren();
        stack.pop_back();

        std::copy(node_children.cbegin(), node_children.cend(), std::back_inserter(stack));
    }

    return found_node;
}

//------------------------------------------------------------------------------

Ogre::AxisAlignedBox scene::compute_bounding_box(const Ogre::SceneNode* _root_scene_node)
{
    // The bounding box in which all the object's bounding boxes will be merged
    Ogre::AxisAlignedBox world_coord_bounding_box;

    // This stack will be used for iterate through the scene nodes graph
    std::stack<const Ogre::SceneNode*> children_stack;
    children_stack.push(_root_scene_node);

    while(!children_stack.empty())
    {
        const Ogre::SceneNode* temp_scene_node = children_stack.top();
        children_stack.pop();

        // Retrieves an iterator pointing to the attached movable objects of the current scene node
        const Ogre::SceneNode::ObjectMap& entities = temp_scene_node->getAttachedObjects();
        for(auto* const movable : entities)
        {
            if(const Ogre::Entity* entity = dynamic_cast<Ogre::Entity*>(movable))
            {
                if(entity->isVisible())
                {
                    world_coord_bounding_box.merge(entity->getWorldBoundingBox());
                }
            }
            else if(const Ogre::ManualObject* manual_object = dynamic_cast<Ogre::ManualObject*>(movable))
            {
                if(manual_object->isVisible())
                {
                    world_coord_bounding_box.merge(manual_object->getWorldBoundingBox());
                }
            }
            else if(const Ogre::Camera* camera_object = dynamic_cast<Ogre::Camera*>(movable))
            {
                if(camera_object->isDebugDisplayEnabled())
                {
                    world_coord_bounding_box.merge(camera_object->getWorldBoundingBox());
                }
            }
        }

        for(auto* const child_node : temp_scene_node->getChildren())
        {
            // First, we must cast the Node* into a sceneNode*
            const Ogre::SceneNode* child_scene_node = dynamic_cast<Ogre::SceneNode*>(child_node);
            if(child_scene_node != nullptr)
            {
                // Push the current node into the stack in order to continue iteration
                children_stack.push(child_scene_node);
            }
        }
    }

    return world_coord_bounding_box;
}

//------------------------------------------------------------------------------

std::pair<Ogre::Vector2, Ogre::Vector2> scene::compute_bounding_rect(
    const Ogre::Camera& _camera,
    const Ogre::SceneNode* _root_scene_node
)
{
    const Ogre::AxisAlignedBox bounding_box = compute_bounding_box(_root_scene_node);
    if(!bounding_box.isFinite())
    {
        const Ogre::Vector2 screen_pos = camera::convert_world_space_to_screen_space(
            _camera,
            _root_scene_node->_getDerivedPosition()
        );
        return {{screen_pos.x, screen_pos.y}, {screen_pos.x, screen_pos.y}};
    }

    const std::array corners                  = bounding_box.getAllCorners();
    static constexpr std::size_t s_NB_CORNERS = std::tuple_size_v<decltype(corners)>;
    std::array<Ogre::Vector2, s_NB_CORNERS> corners_screen_pos;
    std::ranges::transform(
        corners,
        corners_screen_pos.begin(),
        [&_camera](const Ogre::Vector3& _world_pos)
        {
            return sight::viz::scene3d::helper::camera::convert_world_space_to_screen_space(_camera, _world_pos);
        });
    std::array<float, s_NB_CORNERS> screen_x {};
    std::array<float, s_NB_CORNERS> screen_y {};
    for(std::size_t i = 0 ; i < corners_screen_pos.size() ; i++)
    {
        screen_x[i] = corners_screen_pos[i].x;
        screen_y[i] = corners_screen_pos[i].y;
    }

    return {{std::ranges::min(screen_x), std::ranges::min(screen_y)
    }, {std::ranges::max(screen_x), std::ranges::max(screen_y)
        }
    };
}

//------------------------------------------------------------------------------

Ogre::ColourValue scene::generate_color(int _color_index)
{
    static std::vector<Ogre::ColourValue> colors = {
        Ogre::ColourValue(236 / 255.0F, 219 / 255.0F, 84 / 255.0F),
        Ogre::ColourValue(63 / 255.0F, 105 / 255.0F, 170 / 255.0F),
        Ogre::ColourValue(249 / 255.0F, 103 / 255.0F, 20 / 255.0F),
        Ogre::ColourValue(233 / 255.0F, 75 / 255.0F, 60 / 255.0F),
        Ogre::ColourValue(121 / 255.0F, 199 / 255.0F, 83 / 255.0F),
        Ogre::ColourValue(149 / 255.0F, 222 / 255.0F, 227 / 255.0F),
        Ogre::ColourValue(29 / 255.0F, 45 / 255.0F, 168 / 255.0F)
    };

    return colors.at(static_cast<std::size_t>(std::abs(_color_index)) % colors.size());
}

//------------------------------------------------------------------------------

std::string scene::get_length(const Ogre::Vector3& _begin, const Ogre::Vector3& _end)
{
    const int length = static_cast<int>(std::round((_end - _begin).length()));
    return std::to_string(length) + "mm";
}

//------------------------------------------------------------------------------

Ogre::Vector3 scene::spacing_as_vector3(const sight::data::image::spacing_t& _spacing)
{
    return {static_cast<Ogre::Real>(_spacing[0]),
            static_cast<Ogre::Real>(_spacing[1]),
            static_cast<Ogre::Real>(_spacing[2])
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
