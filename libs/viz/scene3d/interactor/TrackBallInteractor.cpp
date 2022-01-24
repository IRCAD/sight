/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "viz/scene3d/interactor/TrackballInteractor.hpp"
#include "viz/scene3d/Layer.hpp"
#include "viz/scene3d/registry/macros.hpp"

#include <core/com/Signal.hxx>

#include <OGRE/Ogre.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d
{

namespace interactor
{

// ----------------------------------------------------------------------------

TrackballInteractor::TrackballInteractor(Layer::sptr _layer, bool _layerOrderDependant) :
    IInteractor(_layer, _layerOrderDependant)
{
}

// ----------------------------------------------------------------------------

TrackballInteractor::~TrackballInteractor()
{
    // Join the animation thread if necessary
    if(m_timer)
    {
        m_animate = false;
        m_timer->stop();
        m_timer.reset();
    }
}

// ----------------------------------------------------------------------------

void TrackballInteractor::mouseMoveEvent(MouseButton button, Modifier, int, int, int dx, int dy)
{
    if(m_mouseMove)
    {
        if(button == LEFT)
        {
            cameraRotate(dx, dy);
        }
        else if(button == MIDDLE)
        {
            cameraTranslate(dx, dy);
        }
        else if(button == RIGHT)
        {
            ::Ogre::Camera* const camera = m_layer.lock()->getDefaultCamera();
            const ::Ogre::Vector3 transVec(0.f, 0.f, static_cast<float>(dy));

            camera->getParentNode()->translate(transVec, ::Ogre::Node::TS_LOCAL);
        }
    }
}

// ----------------------------------------------------------------------------

void TrackballInteractor::buttonPressEvent(IInteractor::MouseButton, Modifier, int _x, int _y)
{
    if(auto layer = m_layer.lock())
    {
        m_mouseMove = isInLayer(_x, _y, layer, m_layerOrderDependant);
    }
}

// ----------------------------------------------------------------------------

void TrackballInteractor::buttonReleaseEvent(IInteractor::MouseButton, Modifier, int, int)
{
    m_mouseMove = false;
}

// ----------------------------------------------------------------------------

void TrackballInteractor::wheelEvent(Modifier, int delta, int x, int y)
{
    if(auto layer = m_layer.lock())
    {
        if(isInLayer(x, y, layer, m_layerOrderDependant))
        {
            constexpr float mouseScale = 0.01f;

            // The zoom factor is reduced when coming closer and increased when going away
            const float newZoom = m_zoom * std::pow(0.85f, static_cast<float>(delta) * mouseScale);

            // Moreover we cannot pass through the center of the trackball
            const float z = (m_zoom - newZoom) * 200.f / (m_mouseScale);

            // Update the center of interest for future rotations
            m_lookAtZ -= z;

            this->updateCameraFocalLength();

            m_zoom = newZoom;

            // Translate the camera.
            ::Ogre::Camera* const camera     = layer->getDefaultCamera();
            ::Ogre::SceneNode* const camNode = camera->getParentSceneNode();
            camNode->translate(::Ogre::Vector3(0, 0, -1) * z, ::Ogre::Node::TS_LOCAL);

            layer->resetCameraClippingRange();
        }
    }
}

// ----------------------------------------------------------------------------

void TrackballInteractor::keyPressEvent(int key, Modifier, int _mouseX, int _mouseY)
{
    if(auto layer = m_layer.lock())
    {
        if(isInLayer(_mouseX, _mouseY, layer, m_layerOrderDependant))
        {
            if(key == 'R' || key == 'r')
            {
                layer->resetCameraCoordinates();
            }

            if(key == 'A' || key == 'a')
            {
                m_animate = !m_animate;

                if(!m_animate && m_timer)
                {
                    m_timer->stop();
                    m_timer.reset();
                }

                if(m_animate)
                {
                    // We use a timer on the main thread instead of a separate thread.
                    // OpenGL commands need to be sent from the same thread as the one on which the context is created.
                    const auto worker = core::thread::getDefaultWorker();
                    m_timer = worker->createTimer();

                    m_timer->setFunction(
                        [this, layer]()
                            {
                                this->cameraRotate(10, 0);
                                layer->requestRender();
                            });
                    m_timer->setDuration(std::chrono::milliseconds(33));
                    m_timer->start();
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------

void TrackballInteractor::resizeEvent(int, int)
{
    const ::Ogre::SceneManager* const sceneManager = m_layer.lock()->getSceneManager();
    ::Ogre::Camera* const camera                   =
        sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
    const float width  = static_cast<float>(camera->getViewport()->getActualWidth());
    const float height = static_cast<float>(camera->getViewport()->getActualHeight());

    const float aspectRatio = width / height;
    camera->setAspectRatio(aspectRatio);
}

// ----------------------------------------------------------------------------

void TrackballInteractor::cameraRotate(int dx, int dy)
{
    ::Ogre::Real wDelta = static_cast< ::Ogre::Real>(dx);
    ::Ogre::Real hDelta = static_cast< ::Ogre::Real>(dy);

    ::Ogre::Camera* const camera     = m_layer.lock()->getDefaultCamera();
    ::Ogre::SceneNode* const camNode = camera->getParentSceneNode();
    const ::Ogre::Viewport* const vp = camera->getViewport();

    const float height = static_cast<float>(vp->getActualHeight());
    const float width  = static_cast<float>(vp->getActualWidth());

    // Current orientation of the camera
    const ::Ogre::Quaternion orientation = camNode->getOrientation();
    const ::Ogre::Vector3 viewRight      = orientation.xAxis();
    const ::Ogre::Vector3 viewUp         = orientation.yAxis();

    // Rotate around the right vector according to the dy of the mouse
    {
        // 1 - Move to the center of the target
        camNode->translate(::Ogre::Vector3(0.f, 0.f, -m_lookAtZ), ::Ogre::Node::TS_LOCAL);

        // 2 - Find rotation axis. We project the mouse movement onto the right and up vectors of the camera
        // We take the absolute to get a positive axis, and then we invert the angle when needed to rotate smoothly
        // Otherwise we would get a weird inversion
        ::Ogre::Vector3 vecX(std::abs(hDelta), 0.f, 0.f);
        ::Ogre::Vector3 rotateX = vecX * viewRight;
        rotateX.normalise();

        // 3 - Now determine the rotation direction
        if(rotateX.dotProduct(::Ogre::Vector3(1.f, 0.f, 0.f)) < 0.f)
        {
            hDelta *= -1;
        }

        // 4 - Compute the angle so that we can rotate around 180 degrees by sliding the whole window
        const float angle = (hDelta * ::Ogre::Math::PI / height);

        // 5 - Apply the rotation on the scene node
        ::Ogre::Quaternion rotate(::Ogre::Radian(angle), rotateX);
        camNode->rotate(rotate);

        // 6 - Go backward in the inverse direction
        camNode->translate(::Ogre::Vector3(0.f, 0.f, m_lookAtZ), ::Ogre::Node::TS_LOCAL);
    }

    // Rotate around the up vector according to the dx of the mouse
    {
        // 1 - Move to the center of the target
        camNode->translate(::Ogre::Vector3(0.f, 0.f, -m_lookAtZ), ::Ogre::Node::TS_LOCAL);

        // 2 - Find rotation axis. We project the mouse movement onto the right and up vectors of the camera
        // We take the absolute to get a positive axis, and then we invert the angle when needed to rotate smoothly
        // Otherwise we would get a weird inversion
        ::Ogre::Vector3 vecY(0.f, std::abs(wDelta), 0.f);
        ::Ogre::Vector3 rotateY = vecY * viewUp;
        rotateY.normalise();

        // 3 - Now determine the rotation direction
        if(rotateY.dotProduct(::Ogre::Vector3(0.f, 1.f, 0.f)) < 0.f)
        {
            wDelta *= -1;
        }

        // 4 - Compute the angle so that we can rotate around 180 degrees by sliding the whole window
        const float angle = (wDelta * ::Ogre::Math::PI / width);

        // 5 - Apply the rotation on the scene node
        ::Ogre::Quaternion rotate(::Ogre::Radian(angle), rotateY);
        camNode->rotate(rotate);

        // 6 - Go backward in the inverse direction
        camNode->translate(::Ogre::Vector3(0.f, 0.f, m_lookAtZ), ::Ogre::Node::TS_LOCAL);
    }
}

// ----------------------------------------------------------------------------

void TrackballInteractor::cameraTranslate(int xmove, int ymove)
{
    float dx = static_cast<float>(xmove) / (m_mouseScale * 10.f);
    float dy = static_cast<float>(-ymove) / (m_mouseScale * 10.f);

    const ::Ogre::SceneManager* const sceneManager = m_layer.lock()->getSceneManager();
    ::Ogre::Camera* camera                         =
        sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
    ::Ogre::SceneNode* camNode = camera->getParentSceneNode();

    ::Ogre::Vector3 vec(dx, dy, 0.f);

    camNode->translate(vec, ::Ogre::Node::TS_LOCAL);
}

// ----------------------------------------------------------------------------

void TrackballInteractor::setSceneLength(float _sceneLength)
{
    m_mouseScale = static_cast<float>(MOUSE_SCALE_FACTOR) / _sceneLength;
    m_lookAtZ    = _sceneLength;
    m_zoom       = 1.f;

    this->updateCameraFocalLength();
}

// ----------------------------------------------------------------------------

void TrackballInteractor::updateCameraFocalLength()
{
    // Set the focal length using the point of interest of the interactor
    // This works well for the trackball but this would need to be adjusted for an another interactor type
    // For a FPS camera style for instance, we would fix the focal length once and for all according
    // to the scale of the world
    const float focalLength = std::max(0.001f, std::abs(m_lookAtZ));

    const ::Ogre::SceneManager* const sceneManager = m_layer.lock()->getSceneManager();
    ::Ogre::Camera* const camera                   =
        sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
    camera->setFocalLength(focalLength);
}

// ----------------------------------------------------------------------------

} // namespace interactor

} // namespace sight::viz::scene3d
