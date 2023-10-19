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

#include "viz/scene3d/interactor/predefined_position_interactor.hpp"

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

predefined_position_interactor::predefined_position_interactor(
    layer::sptr _layer,
    bool _layer_order_dependant,
    std::vector<predefined_position_t> _positions,
    const std::optional<std::string>& _default_position,
    bool _animate
) :
    base(_layer, _layer_order_dependant),
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

predefined_position_interactor::~predefined_position_interactor()
{
    // Join the animation thread if necessary
    if(m_timer)
    {
        m_timer->stop();
        m_timer.reset();
    }
}

// ----------------------------------------------------------------------------

void predefined_position_interactor::mouseMoveEvent(
    MouseButton _button,
    Modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/,
    int _dx,
    int _dy
)
{
    if(m_mouseMove)
    {
        if(_button == LEFT)
        {
            if(!m_timer->is_running())
            {
                this->cameraRotateByMouse(_dx, _dy);
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

void predefined_position_interactor::wheelEvent(Modifier /*_mods*/, double _delta, int _x, int _y)
{
    if(auto layer = m_layer.lock())
    {
        if(isInLayer(_x, _y, layer, m_layerOrderDependant))
        {
            constexpr float mouse_scale = 0.01F;

            // The zoom factor is reduced when coming closer and increased when going away
            const float new_zoom = m_zoom * std::pow(0.85F, static_cast<float>(_delta) * mouse_scale);

            // Moreover we cannot pass through the center of the trackball
            const float z = (m_zoom - new_zoom) * 200.F / (m_mouseScale);

            // Update the center of interest for future rotations
            m_lookAtZ -= z;

            this->updateCameraFocalLength();

            m_zoom = new_zoom;

            // Translate the camera.
            Ogre::Camera* const camera      = layer->getDefaultCamera();
            Ogre::SceneNode* const cam_node = camera->getParentSceneNode();
            cam_node->translate(Ogre::Vector3(0, 0, -1) * z, Ogre::Node::TS_LOCAL);

            m_layer.lock()->requestRender();
        }
    }
}

//------------------------------------------------------------------------------

void predefined_position_interactor::pinchGestureEvent(double _scale_factor, int _center_x, int _center_y)
{
    wheelEvent({}, _scale_factor * 8, _center_x, _center_y);
}

// ----------------------------------------------------------------------------

void predefined_position_interactor::keyPressEvent(int _key, Modifier /*_mods*/, int _mouse_x, int _mouse_y)
{
    if(_key == 'R' || _key == 'r')
    {
        if(auto layer = m_layer.lock())
        {
            if(isInLayer(_mouse_x, _mouse_y, layer, m_layerOrderDependant) && !m_timer->is_running())
            {
                layer->resetCameraCoordinates();
                this->init();
                this->followTransform();
            }
        }
    }
}

// ----------------------------------------------------------------------------

void predefined_position_interactor::resizeEvent(int _width, int _height)
{
    const Ogre::SceneManager* const scene_manager = m_layer.lock()->getSceneManager();
    Ogre::Camera* const camera                    =
        scene_manager->getCamera(viz::scene3d::layer::s_DEFAULT_CAMERA_NAME);

    SIGHT_ASSERT("Width and height should be strictly positive", _width > 0 && _height > 0);
    const float aspect_ratio = static_cast<float>(_width) / static_cast<float>(_height);
    camera->setAspectRatio(aspect_ratio);
    m_layer.lock()->requestRender();
}

//------------------------------------------------------------------------------

void predefined_position_interactor::init()
{
    Ogre::Camera* const camera      = m_layer.lock()->getDefaultCamera();
    Ogre::SceneNode* const cam_node = camera->getParentSceneNode();

    // Reset camera
    cam_node->setPosition(Ogre::Vector3(0, 0, m_lookAtZ));
    cam_node->lookAt(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_WORLD);

    // Rotate camera to its initial wanted orientation (180 on -X)
    this->rotateCamera(cam_node, M_CAMERA_INIT_ROTATION);

    m_current_position_idx = std::nullopt;
    m_currentOrientation   = cam_node->getOrientation();
}

//------------------------------------------------------------------------------

void predefined_position_interactor::cameraRotateByMouse(int _dx, int _dy)
{
    auto w_delta = static_cast<Ogre::Real>(_dx);
    auto h_delta = static_cast<Ogre::Real>(_dy);

    Ogre::Camera* const camera      = m_layer.lock()->getDefaultCamera();
    Ogre::SceneNode* const cam_node = camera->getParentSceneNode();
    const Ogre::Viewport* const vp  = camera->getViewport();

    const auto height = static_cast<float>(vp->getActualHeight());
    const auto width  = static_cast<float>(vp->getActualWidth());

    // Current orientation of the camera
    const Ogre::Quaternion orientation = cam_node->getOrientation();
    const Ogre::Vector3 view_right     = orientation.xAxis();
    const Ogre::Vector3 view_up        = orientation.yAxis();

    // Computes the final position according to mouse displacement.

    // X
    const Ogre::Vector3 vec_x(std::abs(h_delta), 0.F, 0.F);
    Ogre::Vector3 rotate_x = vec_x * view_right;
    rotate_x.normalise();

    if(rotate_x.dotProduct(Ogre::Vector3(1.F, 0.F, 0.F)) < 0.F)
    {
        h_delta *= -1;
    }

    const float angle_x = (h_delta * Ogre::Math::PI / height);
    const Ogre::Quaternion rx(Ogre::Radian(angle_x), rotate_x);

    // Y
    const Ogre::Vector3 vec_y(0.F, std::abs(w_delta), 0.F);
    Ogre::Vector3 rotate_y = vec_y * view_up;
    rotate_y.normalise();

    if(rotate_y.dotProduct(Ogre::Vector3(0.F, 1.F, 0.F)) < 0.F)
    {
        w_delta *= -1;
    }

    const float angle_y = (w_delta * Ogre::Math::PI / width);
    const Ogre::Quaternion ry(Ogre::Radian(angle_y), rotate_y);

    const auto destination = orientation * ry * rx;

    // Apply
    this->rotateCamera(cam_node, destination);
}

// ----------------------------------------------------------------------------

void predefined_position_interactor::setSceneLength(float _scene_length)
{
    m_mouseScale = static_cast<float>(MOUSE_SCALE_FACTOR) / _scene_length;
    m_lookAtZ    = _scene_length;
    m_zoom       = 1.F;

    this->updateCameraFocalLength();
}

// ----------------------------------------------------------------------------

void predefined_position_interactor::updateCameraFocalLength()
{
    // Set the focal length using the point of interest of the interactor
    // This works well for the trackball but this would need to be adjusted for an another interactor type
    // For a FPS camera style for instance, we would fix the focal length once and for all according
    // to the scale of the world
    const float focal_length = std::max(0.001F, std::abs(m_lookAtZ));

    const Ogre::SceneManager* const scene_manager = m_layer.lock()->getSceneManager();
    Ogre::Camera* const camera                    =
        scene_manager->getCamera(viz::scene3d::layer::s_DEFAULT_CAMERA_NAME);
    camera->setFocalLength(focal_length);
}

// ----------------------------------------------------------------------------

void predefined_position_interactor::nextPosition()
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

void predefined_position_interactor::previousPosition()
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

void predefined_position_interactor::toPredefinedPosition(std::size_t _idx, bool _animate)
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

        Ogre::Camera* const camera      = layer->getDefaultCamera();
        Ogre::SceneNode* const cam_node = camera->getParentSceneNode();

        // Get destination orientation.
        const auto& pos = m_predefined_positions[_idx];

        // Relative destination in  regard of reference (this->init()).
        const Ogre::Quaternion rotate_x(Ogre::Degree(pos.rx), Ogre::Vector3(1, 0, 0));
        const Ogre::Quaternion rotate_y(Ogre::Degree(pos.ry), Ogre::Vector3(0, 1, 0));
        const Ogre::Quaternion rotate_z(Ogre::Degree(pos.rz), Ogre::Vector3(0, 0, 1));
        const auto destination = this->transformQuaternion() * M_CAMERA_INIT_ROTATION * rotate_z * rotate_y * rotate_x;

        // Get current orientation.
        const auto origin = cam_node->getOrientation();

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
            const Ogre::Degree short_angle = angle > Ogre::Degree(180) ? Ogre::Degree(360) - angle : angle;
            const float nb_step            = std::ceil(short_angle.valueDegrees() * 100.F / 180.F);

            // Avoid to have gigantic step.
            const float step = (nb_step > 0.001F) ? 1.F / nb_step : 1.F;

            m_timer->set_function(
                [this, layer, origin, destination, cam_node, step]()
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

                    this->rotateCamera(cam_node, rotation);

                    m_currentOrientation = cam_node->getOrientation();

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
            this->rotateCamera(cam_node, destination);
        }

        m_current_position_idx = _idx;
        m_currentOrientation   = cam_node->getOrientation();
    }
}

// ----------------------------------------------------------------------------

void predefined_position_interactor::setParameter(ui::parameter_t _value, std::string _key)
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

void predefined_position_interactor::rotateCamera(Ogre::SceneNode* const _cam, const Ogre::Quaternion& _quat) const
{
    _cam->translate(Ogre::Vector3(0, 0, -m_lookAtZ), Ogre::Node::TS_LOCAL);
    _cam->setOrientation(_quat);
    _cam->translate(Ogre::Vector3(0, 0, m_lookAtZ), Ogre::Node::TS_LOCAL);
}

// ----------------------------------------------------------------------------

void predefined_position_interactor::followTransform()
{
    Ogre::Camera* const camera      = m_layer.lock()->getDefaultCamera();
    Ogre::SceneNode* const cam_node = camera->getParentSceneNode();

    // Initial rotation.
    const auto referential_r = this->transformQuaternion();
    // Initial translation
    const auto referential_t = m_transform.getTrans();

    // 1. Get to origin (remove zoom level)
    cam_node->translate(Ogre::Vector3(0, 0, -m_lookAtZ), Ogre::Node::TS_LOCAL);
    const auto current_r = cam_node->getOrientation();

    // 2. Apply transform rotation
    // Remove previous transform orientation and go to newest orientation.
    cam_node->setOrientation(m_lastOrientation.Inverse() * referential_r * current_r);

    // 3. Translate to new target.
    cam_node->setPosition(referential_t);

    // 4. Get back to same level of zoom
    cam_node->translate(Ogre::Vector3(0, 0, m_lookAtZ), Ogre::Node::TS_LOCAL);

    m_currentOrientation = cam_node->getOrientation();

    m_layer.lock()->requestRender();

    // Store transform orientation for next updates.
    m_lastOrientation = referential_r;
}

} // namespace sight::viz::scene3d::interactor
