/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include <OgreSceneNode.h>

namespace sight::viz::scene3d
{

/**
 * @brief Must be inherited by all classes containing a transformation matrix
 */
class VIZ_SCENE3D_CLASS_API ITransformable
{
public:

    /**
     * @brief Get the Ogre identifier of the transform in the scene manager.
     * @return This transform service Id.
     */
    VIZ_SCENE3D_API viz::scene3d::SRender::OgreObjectIdType getTransformId() const;

    /**
     * @brief Set the Ogre identifier of the transform in the scene manager.
     * @param _id The new transform service Id.
     */
    VIZ_SCENE3D_API void setTransformId(viz::scene3d::SRender::OgreObjectIdType _id);

    /**
     * @brief Retrieve the node matching m_transformId.
     * @param _parentNode The parent node of the created node.
     * @return The node matching m_transformId.
     */
    VIZ_SCENE3D_API Ogre::SceneNode* getTransformNode(Ogre::SceneNode* const _parentNode);

    /**
     * @brief Retrieve the node matching the given node name.
     * @param _name The name of the node.
     * @param _parentNode The parent node of the created node.
     * @return The node matching m_transformId.
     */
    VIZ_SCENE3D_API static Ogre::SceneNode* getTransformNode(
        const std::string& _name,
        Ogre::SceneNode* const _parentNode
    );

    /// The xml attribute to retrieve the transform's id.
    VIZ_SCENE3D_API static const std::string s_TRANSFORM_CONFIG;

private:

    /// Ogre transform node.
    Ogre::SceneNode* m_transformNode;

    /// This transform identifier
    viz::scene3d::SRender::OgreObjectIdType m_transformId;
};

} //namespace sight::viz::scene3d
