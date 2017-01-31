/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/interactor/TrackballInteractor.hpp"
#include <fwRenderOgre/Layer.hpp>
#include <fwRenderOgre/registry/macros.hpp>

#include <fwCom/Signal.hxx>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneNode.h>
#include <OGRE/OgreVector3.h>

fwRenderOgreRegisterInteractorMacro( ::fwRenderOgre::interactor::TrackballInteractor );

namespace fwRenderOgre
{
namespace interactor
{

// ----------------------------------------------------------------------------

TrackballInteractor::TrackballInteractor() :
    IMovementInteractor(),
    m_width(1),
    m_height(1)
{
}

// ----------------------------------------------------------------------------

void TrackballInteractor::mouseMoveEvent(MouseButton button, int, int, int dx, int dy)
{
    if(button == LEFT)
    {
        cameraRotate(dx, dy);
    }
    else if(button == MIDDLE)
    {
        cameraTranslate(dx, dy);
    }
}

// ----------------------------------------------------------------------------

void TrackballInteractor::wheelEvent(int delta, int x, int y)
{
    // The zoom factor is reduced when coming closer and increased when going away
    const float fNewZoom = (delta > 0) ? m_fZoom * 0.85f : m_fZoom / 0.85f;

    // Moreover we cannot pass through the center of the trackball
    const float z = (m_fZoom - fNewZoom) * 200.f / (m_mouseScale );

    // Update the center of interest for future rotations
    m_lookAtZ -= z;

    m_fZoom = fNewZoom;

    // Last, translate the camera
    ::Ogre::Camera* camera     = m_sceneManager->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);
    ::Ogre::SceneNode* camNode = camera->getParentSceneNode();
    ::Ogre::Vector3 direction  = camera->getDirection();
    direction                  = direction * z;
    camNode->translate( direction, ::Ogre::Node::TS_LOCAL );
}

// ----------------------------------------------------------------------------

void TrackballInteractor::buttonReleaseEvent(IInteractor::MouseButton, int, int)
{

}

// ----------------------------------------------------------------------------

void TrackballInteractor::buttonPressEvent(IInteractor::MouseButton, int, int)
{

}

// ----------------------------------------------------------------------------

void TrackballInteractor::resizeEvent(int x, int y)
{
    m_width  = x;
    m_height = y;
}

// ----------------------------------------------------------------------------

void TrackballInteractor::keyPressEvent(int key)
{
    if(key == 'R' || key == 'r')
    {
        auto sig = this->signal<ResetCameraSignalType>( s_RESET_CAMERA_SIG );
        sig->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

void TrackballInteractor::cameraRotate(int dx, int dy)
{
    ::Ogre::Real dx_float = static_cast< ::Ogre::Real>(dx);
    ::Ogre::Real dy_float = static_cast< ::Ogre::Real>(dy);

    ::Ogre::Camera* camera     = m_sceneManager->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);
    ::Ogre::SceneNode* camNode = camera->getParentSceneNode();

    // Current orientation of the camera
    ::Ogre::Quaternion orientation = camNode->getOrientation();
    ::Ogre::Vector3 viewRight      = orientation.xAxis();
    ::Ogre::Vector3 viewUp         = orientation.yAxis();

    // Rotate around the right vector according to the dy of the mouse
    {
        // 1 - Move to the center of the target
        camNode->translate(Ogre::Vector3(0.f, 0.f, -m_lookAtZ), ::Ogre::Node::TS_LOCAL);

        // 2 - Find rotation axis. We project the mouse movement onto the right and up vectors of the camera
        // We take the absolute to get a positive axis, and then we invert the angle when needed to rotate smoothly
        // Otherwise we would get a weird inversion
        ::Ogre::Vector3 vecX(std::abs(dy_float), 0.f, 0.f);
        ::Ogre::Vector3 rotateX = vecX * viewRight;
        rotateX.normalise();

        // 3 - Now determine the rotation direction
        if(rotateX.dotProduct(Ogre::Vector3(1.f, 0.f, 0.f)) < 0.f)
        {
            dy_float *= -1;
        }

        // 4 - Compute the angle so that we can rotate around 180 degrees by sliding the whole window
        float angle = (dy_float * ::Ogre::Math::PI / static_cast< float>(m_height));

        // 5 - Apply the rotation on the scene node
        ::Ogre::Quaternion rotate(::Ogre::Radian(angle), rotateX);
        camNode->rotate( rotate );

        // 6 - Go backward in the inverse direction
        camNode->translate(Ogre::Vector3(0.f, 0.f, m_lookAtZ), ::Ogre::Node::TS_LOCAL);
    }

    // Rotate around the up vector according to the dx of the mouse
    {
        // 1 - Move to the center of the target
        camNode->translate(Ogre::Vector3(0.f, 0.f, -m_lookAtZ), ::Ogre::Node::TS_LOCAL);

        // 2 - Find rotation axis. We project the mouse movement onto the right and up vectors of the camera
        // We take the absolute to get a positive axis, and then we invert the angle when needed to rotate smoothly
        // Otherwise we would get a weird inversion
        ::Ogre::Vector3 vecY(0.f, std::abs(dx_float), 0.f);
        ::Ogre::Vector3 rotateY = vecY * viewUp;
        rotateY.normalise();

        // 3 - Now determine the rotation direction
        if(rotateY.dotProduct(Ogre::Vector3(0.f, 1.f, 0.f)) < 0.f)
        {
            dx_float *= -1;
        }

        // 4 - Compute the angle so that we can rotate around 180 degrees by sliding the whole window
        float angle = (dx_float * ::Ogre::Math::PI / static_cast< float>(m_width));

        // 5 - Apply the rotation on the scene node
        ::Ogre::Quaternion rotate(::Ogre::Radian(angle), rotateY);
        camNode->rotate( rotate );

        // 6 - Go backward in the inverse direction
        camNode->translate(Ogre::Vector3(0.f, 0.f, m_lookAtZ), ::Ogre::Node::TS_LOCAL);
    }
}

// ----------------------------------------------------------------------------

void TrackballInteractor::cameraTranslate(int xmove, int ymove)
{
    float dx = static_cast<float>(xmove) / (m_mouseScale * 10.f);
    float dy = static_cast<float>(-ymove) / (m_mouseScale * 10.f);
    ::Ogre::Camera* camera     = m_sceneManager->getCamera(::fwRenderOgre::Layer::DEFAULT_CAMERA_NAME);
    ::Ogre::SceneNode* camNode = camera->getParentSceneNode();

    ::Ogre::Vector3 vec(dx, dy, 0.f);

    camNode->translate(vec, ::Ogre::Node::TS_LOCAL);

}

// ----------------------------------------------------------------------------

} // namespace interactor
} // namespace fwRenderOgre
