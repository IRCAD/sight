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

#include <OgreSceneNode.h>

namespace sight::viz::scene3d
{

/**
 * @brief Must be inherited by all classes containing a transformation matrix
 */
class VIZ_SCENE3D_CLASS_API transformable
{
public:

    /**
     * @brief Get the Ogre identifier of the transform in the scene manager.
     * @return This transform service Id.
     */
    [[nodiscard]] VIZ_SCENE3D_API viz::scene3d::render::ogre_object_id_t get_transform_id() const;

    /**
     * @brief Set the Ogre identifier of the transform in the scene manager.
     * @param _id The new transform service Id.
     */
    VIZ_SCENE3D_API void set_transform_id(viz::scene3d::render::ogre_object_id_t _id);

    /**
     * @brief Retrieve the node matching m_transformId or create it.
     * @param _parentNode The parent node, used if transform node doesn't exists.
     * @return The node matching m_transformId. (cannot be nullptr)
     */
    VIZ_SCENE3D_API Ogre::SceneNode* get_or_create_transform_node(Ogre::SceneNode* _parent_node);

    /**
     * @brief Retrieve the  transform node matching m_transformId.
     * @return The node matching m_transformId (can be nullptr if node doesn't exists)
     */
    [[nodiscard]] VIZ_SCENE3D_API Ogre::SceneNode* get_transform_node() const;

    /// The xml attribute to retrieve the transform's id.
    VIZ_SCENE3D_API static const std::string TRANSFORM_CONFIG;

private:

    /// Ogre transform node.
    Ogre::SceneNode* m_transform_node {nullptr};

    /// This transform identifier
    viz::scene3d::render::ogre_object_id_t m_transform_id;
};

} //namespace sight::viz::scene3d
