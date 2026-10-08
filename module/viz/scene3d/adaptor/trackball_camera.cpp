/************************************************************************
 *
 * Copyright (C) 2019-2026 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include "module/viz/scene3d/adaptor/trackball_camera.hpp"

#include "viz/scene3d/interactor/trackball.hpp"
#include "viz/scene3d/layer.hpp"
#include "viz/scene3d/utils.hpp"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneNode.h>

namespace sight::module::viz::scene3d::adaptor
{

namespace interactor_3d = sight::viz::scene3d::interactor;

//-----------------------------------------------------------------------------

trackball_camera::~trackball_camera() noexcept
{
    // Join the animation thread if necessary
    if(m_timer)
    {
        m_animate = false;
        m_timer->stop();
        m_timer.reset();
    }
}

//-----------------------------------------------------------------------------

void trackball_camera::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    m_priority = config.get<int>(CONFIG + "priority", m_priority);
}

//-----------------------------------------------------------------------------

void trackball_camera::starting()
{
    sight::viz::scene3d::adaptor::init();

    Ogre::Vector3 view_up_axis = interactor_3d::DEFAULT_VIEW_UP;
    if(const auto& view_up = m_view_up_matrix.const_lock(); view_up)
    {
        const auto view_up_matrix_data = sight::viz::scene3d::utils::to_ogre_matrix(view_up.get_shared());
        view_up_axis = Ogre::Vector3(view_up_matrix_data[0][1], view_up_matrix_data[1][1], view_up_matrix_data[2][1]);
    }

    m_view_up = view_up_axis;

    const auto layer = this->layer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->add_interactor(interactor, m_priority);
}

//-----------------------------------------------------------------------------

void trackball_camera::updating() noexcept
{
}

//-----------------------------------------------------------------------------

void trackball_camera::stopping()
{
    const auto layer = this->layer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->remove_interactor(interactor);

    sight::viz::scene3d::adaptor::deinit();
}

//-----------------------------------------------------------------------------

void trackball_camera::mouse_move_event(
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
            Ogre::Camera* const camera = this->layer()->get_default_camera();
            const Ogre::Vector3 trans_vec(0.F, 0.F, static_cast<float>(_dy));

            camera->getParentNode()->translate(trans_vec, Ogre::Node::TS_LOCAL);
        }

        this->layer()->request_render();
    }
}

//-----------------------------------------------------------------------------

void trackball_camera::button_press_event(mouse_button /*_button*/, modifier /*_mods*/, int _x, int _y)
{
    if(auto layer = this->layer())
    {
        m_mouse_move = is_in_layer(_x, _y, layer);
        layer->request_render();
    }
}

//-----------------------------------------------------------------------------

void trackball_camera::button_release_event(
    mouse_button /*_button*/,
    modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/
)
{
    m_mouse_move = false;
}

//-----------------------------------------------------------------------------

void trackball_camera::wheel_event(modifier /*_mods*/, double _delta, int _x, int _y)
{
    if(auto layer = this->layer())
    {
        if(is_in_layer(_x, _y, layer))
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

void trackball_camera::pinch_gesture_event(double _scale_factor, int _center_x, int _center_y)
{
    // The pinch gesture is converted to a wheel event
    wheel_event({}, _scale_factor * 12, _center_x, _center_y);
}

//------------------------------------------------------------------------------

void trackball_camera::pan_gesture_move_event(int _x, int _y, int _dx, int _dy)
{
    if(auto layer = this->layer(); layer)
    {
        if(is_in_layer(_x, _y, layer))
        {
            camera_translate(_dx, _dy);
        }
    }
}

// ---------------------------------------------------------------------------

void trackball_camera::key_press_event(int _key, modifier /*_mods*/, int _mouse_x, int _mouse_y)
{
    if(auto layer = this->layer())
    {
        if(is_in_layer(_mouse_x, _mouse_y, layer))
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

// ---------------------------------------------------------------------------

void trackball_camera::resize_event(int _width, int _height)
{
    const Ogre::SceneManager* const scene_manager = this->layer()->get_scene_manager();
    Ogre::Camera* const camera                    =
        scene_manager->getCamera(sight::viz::scene3d::layer::DEFAULT_CAMERA_NAME);

    SIGHT_ASSERT("Width and height should be strictly positive", _width > 0 && _height > 0);
    const float aspect_ratio = static_cast<float>(_width) / static_cast<float>(_height);
    camera->setAspectRatio(aspect_ratio);
}

// ---------------------------------------------------------------------------

void trackball_camera::camera_rotate(int _dx, int _dy)
{
    Ogre::Camera* const camera = this->layer()->get_default_camera();
    sight::viz::scene3d::interactor::camera_rotate(camera, _dx, _dy, m_look_at_z, m_view_up);
}

// ---------------------------------------------------------------------------

void trackball_camera::camera_translate(int _x_move, int _y_move)
{
    float dx = static_cast<float>(_x_move) / (m_mouse_scale * 10.F);
    float dy = static_cast<float>(-_y_move) / (m_mouse_scale * 10.F);

    const Ogre::SceneManager* const scene_manager = this->layer()->get_scene_manager();
    Ogre::Camera* camera                          =
        scene_manager->getCamera(sight::viz::scene3d::layer::DEFAULT_CAMERA_NAME);
    Ogre::SceneNode* cam_node = camera->getParentSceneNode();

    Ogre::Vector3 vec(dx, dy, 0.F);

    cam_node->translate(vec, Ogre::Node::TS_LOCAL);
}

// ---------------------------------------------------------------------------

void trackball_camera::set_scene_length(float _scene_length)
{
    m_mouse_scale = static_cast<float>(MOUSE_SCALE_FACTOR) / _scene_length;
    m_look_at_z   = _scene_length;
    m_zoom        = 1.F;

    this->update_camera_focal_length();
}

// ---------------------------------------------------------------------------

void trackball_camera::update_camera_focal_length()
{
    // Set the focal length using the point of interest of the interactor
    // This works well for the trackball but this would need to be adjusted for an another interactor type
    // For a FPS camera style for instance, we would fix the focal length once and for all according
    // to the scale of the world
    const float focal_length = std::max(0.001F, std::abs(m_look_at_z));

    const Ogre::SceneManager* const scene_manager = this->layer()->get_scene_manager();
    Ogre::Camera* const camera                    =
        scene_manager->getCamera(sight::viz::scene3d::layer::DEFAULT_CAMERA_NAME);
    camera->setFocalLength(focal_length);
}

// ---------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor
