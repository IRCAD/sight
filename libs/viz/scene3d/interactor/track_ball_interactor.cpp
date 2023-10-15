/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "viz/scene3d/interactor/trackball_interactor.hpp"
#include "viz/scene3d/Layer.hpp"
#include "viz/scene3d/registry/macros.hpp"

#include <core/com/signal.hxx>

#include <OGRE/Ogre.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d::interactor
{

// ----------------------------------------------------------------------------

trackball_interactor::trackball_interactor(Layer::sptr _layer, bool _layerOrderDependant) :
    base(_layer, _layerOrderDependant)
{
}

// ----------------------------------------------------------------------------

trackball_interactor::~trackball_interactor()
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

void trackball_interactor::mouseMoveEvent(
    MouseButton button,
    Modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/,
    int dx,
    int dy
)
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
            Ogre::Camera* const camera = m_layer.lock()->getDefaultCamera();
            const Ogre::Vector3 transVec(0.F, 0.F, static_cast<float>(dy));

            camera->getParentNode()->translate(transVec, Ogre::Node::TS_LOCAL);
        }

        m_layer.lock()->requestRender();
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::buttonPressEvent(base::MouseButton /*_button*/, Modifier /*_mods*/, int _x, int _y)
{
    if(auto layer = m_layer.lock())
    {
        m_mouseMove = isInLayer(_x, _y, layer, m_layerOrderDependant);
        layer->requestRender();
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::buttonReleaseEvent(
    base::MouseButton /*_button*/,
    Modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/
)
{
    m_mouseMove = false;
}

// ----------------------------------------------------------------------------

void trackball_interactor::wheelEvent(Modifier /*_mods*/, double delta, int x, int y)
{
    if(auto layer = m_layer.lock())
    {
        if(isInLayer(x, y, layer, m_layerOrderDependant))
        {
            constexpr float mouseScale = 0.01F;

            // The zoom factor is reduced when coming closer and increased when going away
            const float newZoom = m_zoom * std::pow(0.85F, static_cast<float>(delta) * mouseScale);

            // Moreover we cannot pass through the center of the trackball
            const float z = (m_zoom - newZoom) * 200.F / (m_mouseScale);

            // Update the center of interest for future rotations
            m_lookAtZ -= z;

            this->updateCameraFocalLength();

            m_zoom = newZoom;

            // Translate the camera.
            Ogre::Camera* const camera     = layer->getDefaultCamera();
            Ogre::SceneNode* const camNode = camera->getParentSceneNode();
            camNode->translate(Ogre::Vector3(0, 0, -1) * z, Ogre::Node::TS_LOCAL);

            layer->requestRender();
        }
    }
}

//------------------------------------------------------------------------------

void trackball_interactor::pinchGestureEvent(double _scaleFactor, int _centerX, int _centerY)
{
    // The pinch gesture is converted to a wheel event
    wheelEvent({}, _scaleFactor * 12, _centerX, _centerY);
}

//------------------------------------------------------------------------------

void trackball_interactor::panGestureMoveEvent(int _x, int _y, int _dx, int _dy)
{
    if(auto layer = m_layer.lock(); layer)
    {
        if(isInLayer(_x, _y, layer, m_layerOrderDependant))
        {
            cameraTranslate(_dx, _dy);
        }
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::keyPressEvent(int key, Modifier /*_mods*/, int _mouseX, int _mouseY)
{
    if(auto layer = m_layer.lock())
    {
        if(isInLayer(_mouseX, _mouseY, layer, m_layerOrderDependant))
        {
            if(key == 'R' || key == 'r')
            {
                layer->resetCameraCoordinates();
                layer->requestRender();
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
                    const auto worker = core::thread::get_default_worker();
                    m_timer = worker->create_timer();

                    m_timer->set_function(
                        [this, layer]()
                        {
                            this->cameraRotate(10, 0);
                            layer->requestRender();
                        });
                    m_timer->set_duration(std::chrono::milliseconds(33));
                    m_timer->start();
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::resizeEvent(int _width, int _height)
{
    const Ogre::SceneManager* const sceneManager = m_layer.lock()->getSceneManager();
    Ogre::Camera* const camera                   =
        sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);

    SIGHT_ASSERT("Width and height should be strictly positive", _width > 0 && _height > 0);
    const float aspectRatio = static_cast<float>(_width) / static_cast<float>(_height);
    camera->setAspectRatio(aspectRatio);
}

// ----------------------------------------------------------------------------

void trackball_interactor::cameraRotate(int dx, int dy)
{
    auto wDelta = static_cast<Ogre::Real>(dx);
    auto hDelta = static_cast<Ogre::Real>(dy);

    Ogre::Camera* const camera     = m_layer.lock()->getDefaultCamera();
    Ogre::SceneNode* const camNode = camera->getParentSceneNode();
    const Ogre::Viewport* const vp = camera->getViewport();

    const auto height = static_cast<float>(vp->getActualHeight());
    const auto width  = static_cast<float>(vp->getActualWidth());

    // Current orientation of the camera
    const Ogre::Quaternion orientation = camNode->getOrientation();
    const Ogre::Vector3 viewRight      = orientation.xAxis();
    const Ogre::Vector3 viewUp         = orientation.yAxis();

    // Rotate around the right vector according to the dy of the mouse
    {
        // 1 - Move to the center of the target
        camNode->translate(Ogre::Vector3(0.F, 0.F, -m_lookAtZ), Ogre::Node::TS_LOCAL);

        // 2 - Find rotation axis. We project the mouse movement onto the right and up vectors of the camera
        // We take the absolute to get a positive axis, and then we invert the angle when needed to rotate smoothly
        // Otherwise we would get a weird inversion
        Ogre::Vector3 vecX(std::abs(hDelta), 0.F, 0.F);
        Ogre::Vector3 rotateX = vecX * viewRight;
        rotateX.normalise();

        // 3 - Now determine the rotation direction
        if(rotateX.dotProduct(Ogre::Vector3(1.F, 0.F, 0.F)) < 0.F)
        {
            hDelta *= -1;
        }

        // 4 - Compute the angle so that we can rotate around 180 degrees by sliding the whole window
        const float angle = (hDelta * Ogre::Math::PI / height);

        // 5 - Apply the rotation on the scene node
        Ogre::Quaternion rotate(Ogre::Radian(angle), rotateX);
        camNode->rotate(rotate);

        // 6 - Go backward in the inverse direction
        camNode->translate(Ogre::Vector3(0.F, 0.F, m_lookAtZ), Ogre::Node::TS_LOCAL);
    }

    // Rotate around the up vector according to the dx of the mouse
    {
        // 1 - Move to the center of the target
        camNode->translate(Ogre::Vector3(0.F, 0.F, -m_lookAtZ), Ogre::Node::TS_LOCAL);

        // 2 - Find rotation axis. We project the mouse movement onto the right and up vectors of the camera
        // We take the absolute to get a positive axis, and then we invert the angle when needed to rotate smoothly
        // Otherwise we would get a weird inversion
        Ogre::Vector3 vecY(0.F, std::abs(wDelta), 0.F);
        Ogre::Vector3 rotateY = vecY * viewUp;
        rotateY.normalise();

        // 3 - Now determine the rotation direction
        if(rotateY.dotProduct(Ogre::Vector3(0.F, 1.F, 0.F)) < 0.F)
        {
            wDelta *= -1;
        }

        // 4 - Compute the angle so that we can rotate around 180 degrees by sliding the whole window
        const float angle = (wDelta * Ogre::Math::PI / width);

        // 5 - Apply the rotation on the scene node
        Ogre::Quaternion rotate(Ogre::Radian(angle), rotateY);
        camNode->rotate(rotate);

        // 6 - Go backward in the inverse direction
        camNode->translate(Ogre::Vector3(0.F, 0.F, m_lookAtZ), Ogre::Node::TS_LOCAL);
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::cameraTranslate(int x_move, int y_move)
{
    float dx = static_cast<float>(x_move) / (m_mouseScale * 10.F);
    float dy = static_cast<float>(-y_move) / (m_mouseScale * 10.F);

    const Ogre::SceneManager* const sceneManager = m_layer.lock()->getSceneManager();
    Ogre::Camera* camera                         =
        sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
    Ogre::SceneNode* camNode = camera->getParentSceneNode();

    Ogre::Vector3 vec(dx, dy, 0.F);

    camNode->translate(vec, Ogre::Node::TS_LOCAL);
}

// ----------------------------------------------------------------------------

void trackball_interactor::setSceneLength(float _sceneLength)
{
    m_mouseScale = static_cast<float>(MOUSE_SCALE_FACTOR) / _sceneLength;
    m_lookAtZ    = _sceneLength;
    m_zoom       = 1.F;

    this->updateCameraFocalLength();
}

// ----------------------------------------------------------------------------

void trackball_interactor::updateCameraFocalLength()
{
    // Set the focal length using the point of interest of the interactor
    // This works well for the trackball but this would need to be adjusted for an another interactor type
    // For a FPS camera style for instance, we would fix the focal length once and for all according
    // to the scale of the world
    const float focalLength = std::max(0.001F, std::abs(m_lookAtZ));

    const Ogre::SceneManager* const sceneManager = m_layer.lock()->getSceneManager();
    Ogre::Camera* const camera                   =
        sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);
    camera->setFocalLength(focalLength);
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::interactor
