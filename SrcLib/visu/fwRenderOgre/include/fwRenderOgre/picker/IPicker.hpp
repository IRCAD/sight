/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"

#include <fwCore/BaseObject.hpp>

#include <OGRE/OgreMaterial.h>
#include <OGRE/OgrePrerequisites.h>
#include <OGRE/OgreVector3.h>

namespace fwRenderOgre
{

namespace picker
{

class FWRENDEROGRE_CLASS_API IPicker
{

public:

    FWRENDEROGRE_API IPicker();
    FWRENDEROGRE_API virtual ~IPicker();

    /// Executes the RaySceneQuery object and returns true or false wether an object has been selected or not
    FWRENDEROGRE_API bool executeRaySceneQuery(int x, int y, int width, int height, std::uint32_t _queryMask);

    FWRENDEROGRE_API ::Ogre::MovableObject* getSelectedObject() const;
    FWRENDEROGRE_API ::Ogre::Vector3 getIntersectionInWorldSpace() const;
    FWRENDEROGRE_API ::Ogre::Vector2 getIntersectionInViewSpace() const;
    FWRENDEROGRE_API ::Ogre::Vector2 getIntersectionInPixel() const;

    FWRENDEROGRE_API void setSceneManager(::Ogre::SceneManager*);
    FWRENDEROGRE_API bool hasSceneManager();

protected:

    /// Current scene manager
    ::Ogre::SceneManager* m_sceneManager;

    /// Selected movable object
    ::Ogre::MovableObject* m_selectedObject;

    bool m_hasSceneManager;

    /// Intersection between the ray and the selected object
    ::Ogre::Vector3 m_rayIntersect;

    /// Returns the scene node containing the default camera
    ::Ogre::SceneNode* getCameraSceneNode() const;

};

//-----------------------------------------------------------------------------
// Inline method(s)

inline ::Ogre::MovableObject* IPicker::getSelectedObject() const
{
    return m_selectedObject;
}

//-----------------------------------------------------------------------------

}
}
