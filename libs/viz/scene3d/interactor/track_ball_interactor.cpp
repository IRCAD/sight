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
#include "viz/scene3d/layer.hpp"
#include "viz/scene3d/registry/macros.hpp"

#include <core/com/signal.hxx>

#include <OGRE/Ogre.h>
#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::viz::scene3d::interactor
{

// ----------------------------------------------------------------------------

trackball_interactor::trackball_interactor(layer::sptr _layer, bool _layer_order_dependant) :
    base(_layer, _layer_order_dependant)
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

void trackball_interactor::mouse_move_event(
    mouse_button _button,
    modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/,
    int _dx,
    int _dy
)
{
    if(m_mouse_move)
    {
        if(_button == left)
        {
            camera_rotate(_dx, _dy);
        }
        else if(_button == middle)
        {
            camera_translate(_dx, _dy);
        }
        else if(_button == right)
        {
            Ogre::Camera* const camera = m_layer.lock()->get_default_camera();
            const Ogre::Vector3 trans_vec(0.F, 0.F, static_cast<float>(_dy));

            camera->getParentNode()->translate(trans_vec, Ogre::Node::TS_LOCAL);
        }

        m_layer.lock()->request_render();
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::button_press_event(base::mouse_button /*_button*/, modifier /*_mods*/, int _x, int _y)
{
    if(auto layer = m_layer.lock())
    {
        m_mouse_move = is_in_layer(_x, _y, layer, m_layer_order_dependant);
        layer->request_render();
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::button_release_event(
    base::mouse_button /*_button*/,
    modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/
)
{
    m_mouse_move = false;
}

// ----------------------------------------------------------------------------

void trackball_interactor::wheel_event(modifier /*_mods*/, double _delta, int _x, int _y)
{
    if(auto layer = m_layer.lock())
    {
        if(is_in_layer(_x, _y, layer, m_layer_order_dependant))
        {
            constexpr float mouse_scale = 0.01F;

            // The zoom factor is reduced when coming closer and increased when going away
            const float new_zoom = m_zoom * std::pow(0.85F, static_cast<float>(_delta) * mouse_scale);

            // Moreover we cannot pass through the center of the trackball
            const float z = (m_zoom - new_zoom) * 200.F / (m_mouse_scale);

            // Update the center of interest for future rotations
            m_look_at_z -= z;

            this->update_camera_focal_length();

            m_zoom = new_zoom;

            // Translate the camera.
            Ogre::Camera* const camera      = layer->get_default_camera();
            Ogre::SceneNode* const cam_node = camera->getParentSceneNode();
            cam_node->translate(Ogre::Vector3(0, 0, -1) * z, Ogre::Node::TS_LOCAL);

            layer->request_render();
        }
    }
}

//------------------------------------------------------------------------------

void trackball_interactor::pinch_gesture_event(double _scale_factor, int _center_x, int _center_y)
{
    // The pinch gesture is converted to a wheel event
    wheel_event({}, _scale_factor * 12, _center_x, _center_y);
}

//------------------------------------------------------------------------------

void trackball_interactor::pan_gesture_move_event(int _x, int _y, int _dx, int _dy)
{
    if(auto layer = m_layer.lock(); layer)
    {
        if(is_in_layer(_x, _y, layer, m_layer_order_dependant))
        {
            camera_translate(_dx, _dy);
        }
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::key_press_event(int _key, modifier /*_mods*/, int _mouse_x, int _mouse_y)
{
    if(auto layer = m_layer.lock())
    {
        if(is_in_layer(_mouse_x, _mouse_y, layer, m_layer_order_dependant))
        {
            if(_key == 'R' || _key == 'r')
            {
                layer->reset_camera_coordinates();
                layer->request_render();
            }

            if(_key == 'A' || _key == 'a')
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
                            this->camera_rotate(10, 0);
                            layer->request_render();
                        });
                    m_timer->set_duration(std::chrono::milliseconds(33));
                    m_timer->start();
                }
            }
        }
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::resize_event(int _width, int _height)
{
    const Ogre::SceneManager* const scene_manager = m_layer.lock()->get_scene_manager();
    Ogre::Camera* const camera                    =
        scene_manager->getCamera(viz::scene3d::layer::DEFAULT_CAMERA_NAME);

    SIGHT_ASSERT("Width and height should be strictly positive", _width > 0 && _height > 0);
    const float aspect_ratio = static_cast<float>(_width) / static_cast<float>(_height);
    camera->setAspectRatio(aspect_ratio);
}

// ----------------------------------------------------------------------------

void trackball_interactor::camera_rotate(int _dx, int _dy)
{
    auto w_delta = static_cast<Ogre::Real>(_dx);
    auto h_delta = static_cast<Ogre::Real>(_dy);

    Ogre::Camera* const camera      = m_layer.lock()->get_default_camera();
    Ogre::SceneNode* const cam_node = camera->getParentSceneNode();
    const Ogre::Viewport* const vp  = camera->getViewport();

    const auto height = static_cast<float>(vp->getActualHeight());
    const auto width  = static_cast<float>(vp->getActualWidth());

    // Current orientation of the camera
    const Ogre::Quaternion orientation = cam_node->getOrientation();
    const Ogre::Vector3 view_right     = orientation.xAxis();
    const Ogre::Vector3 view_up        = orientation.yAxis();

    // Rotate around the right vector according to the dy of the mouse
    {
        // 1 - Move to the center of the target
        cam_node->translate(Ogre::Vector3(0.F, 0.F, -m_look_at_z), Ogre::Node::TS_LOCAL);

        // 2 - Find rotation axis. We project the mouse movement onto the right and up vectors of the camera
        // We take the absolute to get a positive axis, and then we invert the angle when needed to rotate smoothly
        // Otherwise we would get a weird inversion
        Ogre::Vector3 vec_x(std::abs(h_delta), 0.F, 0.F);
        Ogre::Vector3 rotate_x = vec_x * view_right;
        rotate_x.normalise();

        // 3 - Now determine the rotation direction
        if(rotate_x.dotProduct(Ogre::Vector3(1.F, 0.F, 0.F)) < 0.F)
        {
            h_delta *= -1;
        }

        // 4 - Compute the angle so that we can rotate around 180 degrees by sliding the whole window
        const float angle = (h_delta * Ogre::Math::PI / height);

        // 5 - Apply the rotation on the scene node
        Ogre::Quaternion rotate(Ogre::Radian(angle), rotate_x);
        cam_node->rotate(rotate);

        // 6 - Go backward in the inverse direction
        cam_node->translate(Ogre::Vector3(0.F, 0.F, m_look_at_z), Ogre::Node::TS_LOCAL);
    }

    // Rotate around the up vector according to the dx of the mouse
    {
        // 1 - Move to the center of the target
        cam_node->translate(Ogre::Vector3(0.F, 0.F, -m_look_at_z), Ogre::Node::TS_LOCAL);

        // 2 - Find rotation axis. We project the mouse movement onto the right and up vectors of the camera
        // We take the absolute to get a positive axis, and then we invert the angle when needed to rotate smoothly
        // Otherwise we would get a weird inversion
        Ogre::Vector3 vec_y(0.F, std::abs(w_delta), 0.F);
        Ogre::Vector3 rotate_y = vec_y * view_up;
        rotate_y.normalise();

        // 3 - Now determine the rotation direction
        if(rotate_y.dotProduct(Ogre::Vector3(0.F, 1.F, 0.F)) < 0.F)
        {
            w_delta *= -1;
        }

        // 4 - Compute the angle so that we can rotate around 180 degrees by sliding the whole window
        const float angle = (w_delta * Ogre::Math::PI / width);

        // 5 - Apply the rotation on the scene node
        Ogre::Quaternion rotate(Ogre::Radian(angle), rotate_y);
        cam_node->rotate(rotate);

        // 6 - Go backward in the inverse direction
        cam_node->translate(Ogre::Vector3(0.F, 0.F, m_look_at_z), Ogre::Node::TS_LOCAL);
    }
}

// ----------------------------------------------------------------------------

void trackball_interactor::camera_translate(int _x_move, int _y_move)
{
    float dx = static_cast<float>(_x_move) / (m_mouse_scale * 10.F);
    float dy = static_cast<float>(-_y_move) / (m_mouse_scale * 10.F);

    const Ogre::SceneManager* const scene_manager = m_layer.lock()->get_scene_manager();
    Ogre::Camera* camera                          =
        scene_manager->getCamera(viz::scene3d::layer::DEFAULT_CAMERA_NAME);
    Ogre::SceneNode* cam_node = camera->getParentSceneNode();

    Ogre::Vector3 vec(dx, dy, 0.F);

    cam_node->translate(vec, Ogre::Node::TS_LOCAL);
}

// ----------------------------------------------------------------------------

void trackball_interactor::set_scene_length(float _scene_length)
{
    m_mouse_scale = static_cast<float>(MOUSE_SCALE_FACTOR) / _scene_length;
    m_look_at_z   = _scene_length;
    m_zoom        = 1.F;

    this->update_camera_focal_length();
}

// ----------------------------------------------------------------------------

void trackball_interactor::update_camera_focal_length()
{
    // Set the focal length using the point of interest of the interactor
    // This works well for the trackball but this would need to be adjusted for an another interactor type
    // For a FPS camera style for instance, we would fix the focal length once and for all according
    // to the scale of the world
    const float focal_length = std::max(0.001F, std::abs(m_look_at_z));

    const Ogre::SceneManager* const scene_manager = m_layer.lock()->get_scene_manager();
    Ogre::Camera* const camera                    =
        scene_manager->getCamera(viz::scene3d::layer::DEFAULT_CAMERA_NAME);
    camera->setFocalLength(focal_length);
}

// ----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::interactor
