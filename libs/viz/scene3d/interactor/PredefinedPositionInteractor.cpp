/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "viz/scene3d/interactor/PredefinedPositionInteractor.hpp"

#include "viz/scene3d/registry/macros.hpp"

#include <core/com/signal.hxx>

#include <OGRE/Ogre.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneNode.h>

#include <algorithm>

// cspell:ignore Slerp

namespace sight::viz::scene3d::interactor
{

// ----------------------------------------------------------------------------

PredefinedPositionInteractor::PredefinedPositionInteractor(
    Layer::sptr _layer,
    bool _layerOrderDependant,
    std::vector<predefined_position_t> _positions,
    const std::optional<std::string>& _default_position,
    bool _animate
) :
    base(_layer, _layerOrderDependant),
    m_timer(core::thread::get_default_worker()->create_timer()),
    m_predefined_positions(std::move(_positions)),
    m_animate(_animate)
{
    this->init();

    if(_default_position)
    {
        if(const auto& found = std::find_if(
               m_predefined_positions.begin(),
               m_predefined_positions.end(),
               [&](auto& _pos)
            {
                return _pos.name == *_default_position;
            });
           found != m_predefined_positions.end())
        {
            toPredefinedPosition(std::size_t(found - m_predefined_positions.begin()), false);
        }
    }
}

// ----------------------------------------------------------------------------

PredefinedPositionInteractor::~PredefinedPositionInteractor()
{
    // Join the animation thread if necessary
    if(m_timer)
    {
        m_timer->stop();
        m_timer.reset();
    }
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::mouseMoveEvent(
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
            if(!m_timer->is_running())
            {
                this->cameraRotateByMouse(dx, dy);
                m_layer.lock()->requestRender();
            }
            else
            {
                m_layer.lock()->cancelFurtherInteraction();
            }
        }
    }
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::wheelEvent(Modifier /*_mods*/, double delta, int x, int y)
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

            m_layer.lock()->requestRender();
        }
    }
}

//------------------------------------------------------------------------------

void PredefinedPositionInteractor::pinchGestureEvent(double _scaleFactor, int _centerX, int _centerY)
{
    wheelEvent({}, _scaleFactor * 8, _centerX, _centerY);
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::keyPressEvent(int key, Modifier /*_mods*/, int _mouseX, int _mouseY)
{
    if(key == 'R' || key == 'r')
    {
        if(auto layer = m_layer.lock())
        {
            if(isInLayer(_mouseX, _mouseY, layer, m_layerOrderDependant) && !m_timer->is_running())
            {
                layer->resetCameraCoordinates();
                this->init();
                this->followTransform();
            }
        }
    }
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::resizeEvent(int _width, int _height)
{
    const Ogre::SceneManager* const sceneManager = m_layer.lock()->getSceneManager();
    Ogre::Camera* const camera                   =
        sceneManager->getCamera(viz::scene3d::Layer::s_DEFAULT_CAMERA_NAME);

    SIGHT_ASSERT("Width and height should be strictly positive", _width > 0 && _height > 0);
    const float aspectRatio = static_cast<float>(_width) / static_cast<float>(_height);
    camera->setAspectRatio(aspectRatio);
    m_layer.lock()->requestRender();
}

//------------------------------------------------------------------------------

void PredefinedPositionInteractor::init()
{
    Ogre::Camera* const camera     = m_layer.lock()->getDefaultCamera();
    Ogre::SceneNode* const camNode = camera->getParentSceneNode();

    // Reset camera
    camNode->setPosition(Ogre::Vector3(0, 0, m_lookAtZ));
    camNode->lookAt(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_WORLD);

    // Rotate camera to its initial wanted orientation (180 on -X)
    this->rotateCamera(camNode, m_cameraInitRotation);

    m_current_position_idx = std::nullopt;
    m_currentOrientation   = camNode->getOrientation();
}

//------------------------------------------------------------------------------

void PredefinedPositionInteractor::cameraRotateByMouse(int dx, int dy)
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

    // Computes the final position according to mouse displacement.

    // X
    const Ogre::Vector3 vecX(std::abs(hDelta), 0.F, 0.F);
    Ogre::Vector3 rotateX = vecX * viewRight;
    rotateX.normalise();

    if(rotateX.dotProduct(Ogre::Vector3(1.F, 0.F, 0.F)) < 0.F)
    {
        hDelta *= -1;
    }

    const float angleX = (hDelta * Ogre::Math::PI / height);
    const Ogre::Quaternion rx(Ogre::Radian(angleX), rotateX);

    // Y
    const Ogre::Vector3 vecY(0.F, std::abs(wDelta), 0.F);
    Ogre::Vector3 rotateY = vecY * viewUp;
    rotateY.normalise();

    if(rotateY.dotProduct(Ogre::Vector3(0.F, 1.F, 0.F)) < 0.F)
    {
        wDelta *= -1;
    }

    const float angleY = (wDelta * Ogre::Math::PI / width);
    const Ogre::Quaternion ry(Ogre::Radian(angleY), rotateY);

    const auto destination = orientation * ry * rx;

    // Apply
    this->rotateCamera(camNode, destination);
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::setSceneLength(float _sceneLength)
{
    m_mouseScale = static_cast<float>(MOUSE_SCALE_FACTOR) / _sceneLength;
    m_lookAtZ    = _sceneLength;
    m_zoom       = 1.F;

    this->updateCameraFocalLength();
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::updateCameraFocalLength()
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
    m_layer.lock()->requestRender();
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::nextPosition()
{
    // Initialization and loop over all positions.
    if(!m_current_position_idx || m_current_position_idx.value() >= m_predefined_positions.size() - 1)
    {
        m_current_position_idx = 0;
    }
    else
    {
        m_current_position_idx.value()++;
    }

    this->toPredefinedPosition(m_current_position_idx.value(), m_animate);
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::previousPosition()
{
    // Initialization and loop over all positions.
    if(!m_current_position_idx || m_current_position_idx.value() == 0)
    {
        m_current_position_idx = m_predefined_positions.size() - 1;
    }
    else
    {
        m_current_position_idx.value()--;
    }

    this->toPredefinedPosition(m_current_position_idx.value(), m_animate);
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::toPredefinedPosition(std::size_t _idx, bool _animate)
{
    SIGHT_ASSERT(
        "Cannot move to position '" << _idx << " / " << m_predefined_positions.size() << "'",
        _idx < m_predefined_positions.size()
    );

    if(auto layer = m_layer.lock())
    {
        // 1. Stop timer if needed
        if(m_timer->is_running())
        {
            m_timer->stop();
        }

        // Reset the percentage
        m_percentage = 0.F;

        Ogre::Camera* const camera     = layer->getDefaultCamera();
        Ogre::SceneNode* const camNode = camera->getParentSceneNode();

        // Get destination orientation.
        const auto& pos = m_predefined_positions[_idx];

        // Relative destination in  regard of reference (this->init()).
        const Ogre::Quaternion rotateX(Ogre::Degree(pos.rx), Ogre::Vector3(1, 0, 0));
        const Ogre::Quaternion rotateY(Ogre::Degree(pos.ry), Ogre::Vector3(0, 1, 0));
        const Ogre::Quaternion rotateZ(Ogre::Degree(pos.rz), Ogre::Vector3(0, 0, 1));
        const auto destination = this->transformQuaternion() * m_cameraInitRotation * rotateZ * rotateY * rotateX;

        // Get current orientation.
        const auto origin = camNode->getOrientation();

        // TODO: find a threshold.
        if(destination.equals(origin, Ogre::Degree(0.1F)))
        {
            return;
        }

        if(_animate)
        {
            // Compute the number of step.
            const auto rotation_path = destination * origin.Inverse();

            Ogre::Degree angle;
            Ogre::Vector3 axis;
            rotation_path.ToAngleAxis(angle, axis);

            // Convert to short angle if needed.
            const Ogre::Degree shortAngle = angle > Ogre::Degree(180) ? Ogre::Degree(360) - angle : angle;
            const float nb_step           = std::ceil(shortAngle.valueDegrees() * 100.F / 180.F);

            // Avoid to have gigantic step.
            const float step = (nb_step > 0.001F) ? 1.F / nb_step : 1.F;

            m_timer->set_function(
                [this, layer, origin, destination, camNode, step]()
                {
                    if(m_timer->is_running())
                    {
                        m_timer->stop();
                    }

                    m_percentage += step;

                    const bool is_last_step        = m_percentage >= 1.F;
                    const auto percentage_to_apply = is_last_step ? 1.F : m_percentage;

                    const auto rotation = Ogre::Quaternion::Slerp(
                        Ogre::Real(percentage_to_apply),
                        origin,
                        destination,
                        true
                    );

                    this->rotateCamera(camNode, rotation);

                    m_currentOrientation = camNode->getOrientation();

                    layer->requestRender();

                    if(is_last_step)
                    {
                        m_percentage = 0.F;
                    }
                    else
                    {
                        m_timer->set_duration(
                            std::max(
                                std::chrono::milliseconds(0),
                                std::chrono::milliseconds(10) - std::chrono::duration_cast<std::chrono::milliseconds>(
                                    std::chrono::system_clock::now() - m_last_step_time
                                )
                            )
                        );

                        m_last_step_time = std::chrono::system_clock::now();
                        m_timer->set_one_shot(true);
                        m_timer->start();
                    }
                });

            m_timer->set_duration(std::chrono::milliseconds(0));
            m_timer->set_one_shot(true);
            m_timer->start();
        }
        else
        {
            this->rotateCamera(camNode, destination);
        }

        m_current_position_idx = _idx;
        m_currentOrientation   = camNode->getOrientation();
    }
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::setParameter(ui::parameter_t _value, std::string _key)
{
    if(_key == "position")
    {
        const std::string predefined_position_name = std::get<std::string>(_value);

        const auto it = std::ranges::find_if(
            m_predefined_positions,
            [predefined_position_name](const predefined_position_t& _p)
            {
                return _p.name == predefined_position_name;
            });

        const auto index = std::size_t(it - m_predefined_positions.begin());

        if(index >= m_predefined_positions.size())
        {
            SIGHT_ERROR("Cannot move to position '" << index << " / " << m_predefined_positions.size() << "'");
            return;
        }

        this->toPredefinedPosition(static_cast<std::size_t>(index), m_animate);
    }
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::rotateCamera(Ogre::SceneNode* const _cam, const Ogre::Quaternion& _quat) const
{
    _cam->translate(Ogre::Vector3(0, 0, -m_lookAtZ), Ogre::Node::TS_LOCAL);
    _cam->setOrientation(_quat);
    _cam->translate(Ogre::Vector3(0, 0, m_lookAtZ), Ogre::Node::TS_LOCAL);
}

// ----------------------------------------------------------------------------

void PredefinedPositionInteractor::followTransform()
{
    Ogre::Camera* const camera     = m_layer.lock()->getDefaultCamera();
    Ogre::SceneNode* const camNode = camera->getParentSceneNode();

    // Initial rotation.
    const auto referentialR = this->transformQuaternion();
    // Initial translation
    const auto referentialT = m_transform.getTrans();

    // 1. Get to origin (remove zoom level)
    camNode->translate(Ogre::Vector3(0, 0, -m_lookAtZ), Ogre::Node::TS_LOCAL);
    const auto currentR = camNode->getOrientation();

    // 2. Apply transform rotation
    // Remove previous transform orientation and go to newest orientation.
    camNode->setOrientation(m_lastOrientation.Inverse() * referentialR * currentR);

    // 3. Translate to new target.
    camNode->setPosition(referentialT);

    // 4. Get back to same level of zoom
    camNode->translate(Ogre::Vector3(0, 0, m_lookAtZ), Ogre::Node::TS_LOCAL);

    m_currentOrientation = camNode->getOrientation();

    m_layer.lock()->requestRender();

    // Store transform orientation for next updates.
    m_lastOrientation = referentialR;
}

} // namespace sight::viz::scene3d::interactor
