/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include <core/BaseObject.hpp>

#include <OGRE/Ogre.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgrePrerequisites.h>

namespace sight::viz::scene3d
{

namespace picker
{

class VIZ_SCENE3D_CLASS_API IPicker
{
public:

    /// Construct all members.
    VIZ_SCENE3D_API IPicker();

    /// Destruct all members.
    VIZ_SCENE3D_API virtual ~IPicker();

    /**
     * @brief Executes the RaySceneQuery object and returns whether an object has been selected or not.
     * @param _x screen width position.
     * @param _y screen height position.
     * @param _queryMask Mask used to query entities with matching flags.
     * @return True if an object has been selected.
     */
    VIZ_SCENE3D_API bool executeRaySceneQuery(int _x, int _y, std::uint32_t _queryMask);

    /**
     * @brief Get the selected object if there is one.
     * @return The selected object.
     */
    VIZ_SCENE3D_API ::Ogre::MovableObject* getSelectedObject() const;

    /**
     * @brief Get the intersection position in the world space.
     * @return The intersection position.
     */
    VIZ_SCENE3D_API ::Ogre::Vector3 getIntersectionInWorldSpace() const;

    /**
     * @brief Get the intersection position in the view space.
     * @return The intersection position.
     */
    VIZ_SCENE3D_API ::Ogre::Vector2 getIntersectionInViewSpace() const;

    /**
     * @brief Get the intersection position in the screen space.
     * @return The intersection position.
     */
    VIZ_SCENE3D_API ::Ogre::Vector2 getIntersectionInPixel() const;

    /**
     * @brief Set the scene manager.
     * @param _sceneMgr The scene manager.
     */
    VIZ_SCENE3D_API void setSceneManager(::Ogre::SceneManager* _sceneMgr);

    /**
     * @brief Check if there is a scene manager.
     * @return True if a scene manager is setted.
     */
    VIZ_SCENE3D_API bool hasSceneManager();

protected:

    /// Current scene manager.
    ::Ogre::SceneManager* m_sceneManager {nullptr};

    /// Selected movable object.
    ::Ogre::MovableObject* m_selectedObject {nullptr};

    /// True if a scene manager is setted.
    bool m_hasSceneManager {false};

    /// Intersection between the ray and the selected object.
    ::Ogre::Vector3 m_rayIntersect {};

    /// Returns the scene node containing the default camera.
    ::Ogre::SceneNode* getCameraSceneNode() const;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ::Ogre::MovableObject* IPicker::getSelectedObject() const
{
    return m_selectedObject;
}

//-----------------------------------------------------------------------------

} // namespace picker

} // namespace sight::viz
