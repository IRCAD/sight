/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_PICKER_IPICKER_HPP__
#define __FWRENDEROGRE_PICKER_IPICKER_HPP__

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
    FWRENDEROGRE_API bool executeRaySceneQuery(int x, int y, int width, int height);

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
#endif // __FWRENDEROGRE_PICKER_IPICKER_HPP__
