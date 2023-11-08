/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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
#include "viz/scene3d/render.hpp"

#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d::helper
{

class scene
{
public:

    /**
     * @brief Looks for a given scene node in a scene manager.
     * @param[in] _nodeId Identifier of the node
     * @param[in] _sceneMgr scene manager
     * @return the Ogre scene node, null if not found
     */
    VIZ_SCENE3D_API static Ogre::SceneNode* get_node_by_id(
        viz::scene3d::render::ogre_object_id_t _node_id,
        Ogre::SceneNode* _scene_node
    );

    /**
     * Computes the bounding box of the descendants of the provided node.
     * @param rootsceneNode The node whose bounding box is to be computed
     * @return The bounding box of the node
     */
    VIZ_SCENE3D_API static Ogre::AxisAlignedBox compute_bounding_box(const Ogre::SceneNode* _root_scene_node);

    /**
     * Computes the projection of the bounding box of the descendants of the provided node in screen space using the
     * provided camera.
     * @param camera The camera used for the projection
     * @param rootsceneNode The node whose bounding box is to be computed
     * @return The projection of the bounding box in screen space as a pair, with the first element being the minimum
     * (top left) and the second element the maximum (bottom right).
     */
    VIZ_SCENE3D_API static std::pair<Ogre::Vector2, Ogre::Vector2> compute_bounding_rect(
        const Ogre::Camera& _camera,
        const Ogre::SceneNode* _root_scene_node
    );

    /**
     * @brief Generates a color from a distance ID.
     * @param _id ID of the distance.
     * @return The generated color.
     */
    VIZ_SCENE3D_API static Ogre::ColourValue generate_color(int _color_index);

    /**
     * @brief Gets the formatted string used to display the length of a distance.
     * @return The formatted string.
     */
    VIZ_SCENE3D_API static std::string get_length(const Ogre::Vector3& _begin, const Ogre::Vector3& _end);

    /**
     * @brief Return the image spacing as a vector.
     * @return The output vector.
     */
    VIZ_SCENE3D_API static Ogre::Vector3 spacing_as_vector3(const sight::data::image::spacing_t& spacing);
};

} // namespace sight::viz::scene3d::helper
