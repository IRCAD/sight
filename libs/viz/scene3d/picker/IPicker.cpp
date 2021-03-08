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

#include "viz/scene3d/picker/IPicker.hpp"

#include "viz/scene3d/collisionTools/CollisionTools.hpp"
#include "viz/scene3d/helper/Camera.hpp"
#include "viz/scene3d/Layer.hpp"
#include "viz/scene3d/Utils.hpp"

#include <service/helper/Config.hpp>
#include <service/macros.hpp>

#include <OGRE/OgreManualObject.h>
#include <OGRE/OgreMovableObject.h>
#include <OGRE/OgreRay.h>
#include <OGRE/OgreSceneNode.h>

#include <cmath>
#include <tuple>

namespace sight::viz::scene3d
{

namespace picker
{

// ----------------------------------------------------------------------------

IPicker::IPicker()
{
}

// ----------------------------------------------------------------------------

IPicker::~IPicker()
{
}

//------------------------------------------------------------------------------

bool IPicker::executeRaySceneQuery(int _x, int _y, std::uint32_t _queryMask)
{
    const auto* const camera = m_sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
    const auto vpPos         = helper::Camera::convertFromWindowToViewportSpace(*camera, _x, _y);
    const ::Ogre::Ray vpRay  = camera->getCameraToViewportRay(vpPos.x, vpPos.y);

#ifdef SHOW_BOUNDS
    if (m_selectedObject)
    {
        m_selectedObject->getParentSceneNode()->showBoundingBox(false);
    }
#endif

    viz::scene3d::CollisionTools tool = viz::scene3d::CollisionTools(m_sceneManager, _queryMask);

    float distance(-1.f);
    bool entityFound(false);
    std::tie(entityFound, m_rayIntersect, m_selectedObject, distance) = tool.raycast(vpRay, _queryMask);

    if (entityFound)
    {

#ifdef SHOW_BOUNDS
        m_selectedObject->getParentSceneNode()->showBoundingBox(true);
#endif

        SIGHT_DEBUG("Entity find and intersect at " << getIntersectionInWorldSpace() << "(WS)");

        SIGHT_DEBUG("Entity find and intersect at " << getIntersectionInViewSpace() << "(VS)");

        SIGHT_DEBUG("Entity find and intersect at " << getIntersectionInPixel() << "(Px)");

    }
    else
    {
        m_selectedObject = nullptr;
    }

    return entityFound;
}

//-----------------------------------------------------------------------------

::Ogre::SceneNode* IPicker::getCameraSceneNode() const
{
    SIGHT_ASSERT("The associated SceneManager is not instanciated", m_sceneManager);
    return m_sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME)->getParentSceneNode();
}

//-----------------------------------------------------------------------------

::Ogre::Vector3 IPicker::getIntersectionInWorldSpace() const
{
    return m_rayIntersect;
}

//-----------------------------------------------------------------------------

::Ogre::Vector2 IPicker::getIntersectionInViewSpace() const
{
    ::Ogre::Camera* cam        = m_sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
    ::Ogre::Matrix4 viewMatrix = cam->getViewMatrix();
    ::Ogre::Matrix4 projMatrix = cam->getProjectionMatrixWithRSDepth();

    ::Ogre::Vector3 point = projMatrix * (viewMatrix * m_rayIntersect);

    ::Ogre::Vector2 screenSpacePoint = ::Ogre::Vector2::ZERO;
    screenSpacePoint.x               = (point.x / 2.f) + 0.5f;
    screenSpacePoint.y               = (point.y / 2.f) + 0.5f;

    return screenSpacePoint;
}

//-----------------------------------------------------------------------------

::Ogre::Vector2 IPicker::getIntersectionInPixel() const
{
    ::Ogre::Vector2 screenSpacePoint = getIntersectionInViewSpace();

    ::Ogre::Camera* cam        = m_sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
    ::Ogre::Viewport* viewport = cam->getViewport();

    /// We need to round the result to get the right pixel
    screenSpacePoint.x = std::round(screenSpacePoint.x * static_cast< ::Ogre::Real>(viewport->getActualWidth()));
    screenSpacePoint.y = std::round(screenSpacePoint.y * static_cast< ::Ogre::Real>(viewport->getActualHeight()));

    return screenSpacePoint;
}

//-----------------------------------------------------------------------------

void IPicker::setSceneManager(::Ogre::SceneManager* _sceneMgr)
{
    m_sceneManager    = _sceneMgr;
    m_hasSceneManager = true;
}

//-----------------------------------------------------------------------------

bool IPicker::hasSceneManager()
{
    return m_hasSceneManager;
}

// ----------------------------------------------------------------------------

} // namespace interactor
} // namespace sight::viz::scene3d
