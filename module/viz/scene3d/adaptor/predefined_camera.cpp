/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include "module/viz/scene3d/adaptor/predefined_camera.hpp"

#include "viz/scene3d/interactor/trackball.hpp"
#include "viz/scene3d/layer.hpp"
#include "viz/scene3d/utils.hpp"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreNode.h>
#include <OGRE/OgreSceneNode.h>

#include <algorithm>

// cspell:ignore Slerp

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

predefined_camera::predefined_camera() noexcept :
    m_timer(core::thread::get_default_worker()->create_timer()),
    m_view_up_vector(sight::viz::scene3d::interactor::DEFAULT_VIEW_UP)
{
    new_slot(slots::RESET, [this](){lazy_update(update_flags::reset);});
    new_slot(slots::SET_TRANSFORM, [this](){lazy_update(update_flags::set_transform);});
    new_slot(slots::SET_POSITION, [this](){lazy_update(update_flags::set_position);});
    new_slot(slots::NEXT_POSITION, [this](){lazy_update(update_flags::next_position);});
    new_slot(slots::PREV_POSITION, [this](){lazy_update(update_flags::prev_position);});
}

//-----------------------------------------------------------------------------

predefined_camera::~predefined_camera() noexcept
{
    // Join the animation thread if necessary
    if(m_timer)
    {
        m_timer->stop();
        m_timer.reset();
    }
}

//-----------------------------------------------------------------------------

void predefined_camera::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    m_priority = config.get(CONFIG + "priority", m_priority);

    m_manual_rotation = config.get(CONFIG + "mouseRotation", m_manual_rotation);

    m_animate = config.get(CONFIG + "animate", m_animate);
    m_zoom    = config.get(CONFIG + "zoom", m_zoom);

    const auto positions = config.get_child("positions");

    const auto pos_cfg = positions.equal_range("position");

    // Configure tracked tool
    for(auto t = pos_cfg.first ; t != pos_cfg.second ; ++t)
    {
        predefined_position_t position;

        const auto posattr = t->second.get_child("<xmlattr>");

        // 1. Name (should be unique).
        position.name = posattr.get<std::string>("name");

        position.rx = posattr.get<float>("rx", 0.F);
        position.ry = posattr.get<float>("ry", 0.F);
        position.rz = posattr.get<float>("rz", 0.F);

        m_camera_positions.push_back(position);
    }
}

//-----------------------------------------------------------------------------

void predefined_camera::starting()
{
    sight::viz::scene3d::adaptor::init();

    const auto layer = this->layer();

    Ogre::Vector3 view_up_axis = sight::viz::scene3d::interactor::DEFAULT_VIEW_UP;
    if(const auto& view_up = m_view_up.const_lock(); view_up)
    {
        const auto view_up_matrix = sight::viz::scene3d::utils::to_ogre_matrix(view_up.get_shared());
        view_up_axis = Ogre::Vector3(view_up_matrix[0][1], view_up_matrix[1][1], view_up_matrix[2][1]);
    }

    m_view_up_vector                = view_up_axis;
    m_animate_interactor            = m_animate;
    m_follow_orientation_interactor = *m_follow_orientation;
    m_zoom_config                   = m_zoom;

    auto interactor = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->add_interactor(interactor, m_priority);

    this->init();

    if(!(*m_position).empty())
    {
        if(const auto& found = std::ranges::find_if(
               m_camera_positions,

               [&](auto& _pos)
            {
                return _pos.name == *m_position;
            });
           found != m_camera_positions.end())
        {
            to_predefined_position(static_cast<std::size_t>(found - m_camera_positions.begin()), false);
        }
    }

    this->updating();
}

//------------------------------------------------------------------------------

void predefined_camera::reset()
{
    Ogre::Vector3 view_up_axis = sight::viz::scene3d::interactor::DEFAULT_VIEW_UP;
    if(const auto& view_up = m_view_up.const_lock(); view_up)
    {
        const auto view_up_matrix = sight::viz::scene3d::utils::to_ogre_matrix(view_up.get_shared());
        view_up_axis = Ogre::Vector3(view_up_matrix[0][1], view_up_matrix[1][1], view_up_matrix[2][1]);
    }

    m_view_up_vector = view_up_axis;

    if(auto layer = this->layer())
    {
        layer->reset_camera_coordinates();
        m_last_orientation = Ogre::Quaternion();
        auto current_position = m_current_position_idx;
        this->init();
        this->follow_transform();
        if(current_position.has_value())
        {
            this->to_predefined_position(current_position.value(), m_animate_interactor);
        }
    }
}

//-----------------------------------------------------------------------------

void predefined_camera::updating() noexcept
{
    if(update_needed(update_flags::reset))
    {
        this->reset();
    }

    // Do first the transform update, otherwise the position will not calculated on the right target
    if(update_needed(update_flags::set_transform))
    {
        if(const auto& transform = m_transform.const_lock(); transform)
        {
            const auto ogre_mat = sight::viz::scene3d::utils::to_ogre_matrix(transform.get_shared());

            this->set_transform(ogre_mat);
        }
    }

    if(update_needed(update_flags::set_position))
    {
        if(!(*m_position).empty())
        {
            this->set_position(*m_position);
        }
    }
    else if(update_needed(update_flags::next_position))
    {
        this->next_position();
    }
    else if(update_needed(update_flags::prev_position))
    {
        this->previous_position();
    }

    update_done();
    this->request_render();
}

//-----------------------------------------------------------------------------

void predefined_camera::stopping()
{
    const auto layer = this->layer();
    auto interactor  = std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->remove_interactor(interactor);

    sight::viz::scene3d::adaptor::deinit();
}

//-----------------------------------------------------------------------------

predefined_camera::connections_t predefined_camera::auto_connections() const
{
    service::connections_t connections = {
        {m_transform, sight::data::signals::MODIFIED, slots::SET_TRANSFORM},
        {m_position, sight::data::signals::MODIFIED, slots::SET_POSITION}
    };
    return connections + adaptor::auto_connections();
}

//-----------------------------------------------------------------------------

void predefined_camera::mouse_move_event(
    mouse_button _button,
    modifier /*_mods*/,
    int /*_x*/,
    int /*_y*/,
    int _dx,
    int _dy
)
{
    if(m_manual_rotation)
    {
        if(_button == left)
        {
            if(!m_timer->is_running())
            {
                this->camera_rotate_by_mouse(_dx, _dy);
                this->layer()->request_render();
            }
            else
            {
                this->layer()->cancel_further_interaction();
            }
        }
    }
}

//-----------------------------------------------------------------------------

void predefined_camera::wheel_event(modifier /*_mods*/, double _delta, int _x, int _y)
{
    if(auto layer = this->layer())
    {
        if(is_in_layer(_x, _y, layer))
        {
            constexpr float mouse_scale = 0.01F;

            // The zoom factor is reduced when coming closer and increased when going away
            const float new_zoom = m_zoom_factor * std::pow(0.85F, static_cast<float>(_delta) * mouse_scale);

            // Moreover we cannot pass through the center of the trackball
            const float z = (m_zoom_factor - new_zoom) * 200.F / (m_mouse_scale);

            // Update the center of interest for future rotations
            m_look_at_z -= z;

            this->update_camera_focal_length();

            m_zoom_factor = new_zoom;

            // Translate the camera.
            Ogre::Camera* const camera      = layer->get_default_camera();
            Ogre::SceneNode* const cam_node = camera->getParentSceneNode();
            cam_node->translate(Ogre::Vector3(0, 0, -1) * z, Ogre::Node::TS_LOCAL);

            layer->request_render();
        }
    }
}

//------------------------------------------------------------------------------

void predefined_camera::pinch_gesture_event(double _scale_factor, int _center_x, int _center_y)
{
    wheel_event({}, _scale_factor * 8, _center_x, _center_y);
}

//-----------------------------------------------------------------------------

void predefined_camera::key_press_event(int _key, modifier /*_mods*/, int _mouse_x, int _mouse_y)
{
    if(_key == 'R' || _key == 'r')
    {
        bool in_layer = false;
        if(auto layer = this->layer())
        {
            in_layer = is_in_layer(_mouse_x, _mouse_y, layer);
        }

        if(in_layer && !m_timer->is_running())
        {
            this->reset();
        }
    }
}

//-----------------------------------------------------------------------------

void predefined_camera::resize_event(int _width, int _height)
{
    const Ogre::SceneManager* const scene_manager = layer()->get_scene_manager();
    Ogre::Camera* const camera                    =
        scene_manager->getCamera(sight::viz::scene3d::layer::DEFAULT_CAMERA_NAME);

    SIGHT_ASSERT("Width and height should be strictly positive", _width > 0 && _height > 0);
    const float aspect_ratio = static_cast<float>(_width) / static_cast<float>(_height);
    camera->setAspectRatio(aspect_ratio);
    layer()->request_render();
}

//------------------------------------------------------------------------------

void predefined_camera::init()
{
    Ogre::Camera* const camera      = layer()->get_default_camera();
    Ogre::SceneNode* const cam_node = camera->getParentSceneNode();

    // Reset camera
    cam_node->setPosition(Ogre::Vector3(0, 0, m_look_at_z));
    cam_node->lookAt(Ogre::Vector3(0, 0, 1), Ogre::Node::TS_WORLD);

    // Rotate camera to its initial wanted orientation (180 on -X)
    this->rotate_camera(cam_node, m_camera_init_rotation);

    m_current_position_idx = std::nullopt;
}

//------------------------------------------------------------------------------

void predefined_camera::camera_rotate_by_mouse(int _dx, int _dy)
{
    Ogre::Camera* const camera = layer()->get_default_camera();
    sight::viz::scene3d::interactor::camera_rotate(camera, _dx, _dy, m_look_at_z, m_view_up_vector);
}

//------------------------------------------------------------------------------

void predefined_camera::set_scene_length(float _scene_length)
{
    m_mouse_scale = static_cast<float>(MOUSE_SCALE_FACTOR) / _scene_length;
    m_look_at_z   = _scene_length * 1.F / m_zoom_config;
    m_zoom_factor = 1.F;

    this->update_camera_focal_length();
}

//------------------------------------------------------------------------------

void predefined_camera::update_camera_focal_length()
{
    // Set the focal length using the point of interest of the interactor
    // This works well for the trackball but this would need to be adjusted for an another interactor type
    // For a FPS camera style for instance, we would fix the focal length once and for all according
    // to the scale of the world
    const float focal_length = std::max(0.001F, std::abs(m_look_at_z));

    const Ogre::SceneManager* const scene_manager = layer()->get_scene_manager();
    Ogre::Camera* const camera                    =
        scene_manager->getCamera(sight::viz::scene3d::layer::DEFAULT_CAMERA_NAME);
    camera->setFocalLength(focal_length);
}

//------------------------------------------------------------------------------

void predefined_camera::next_position()
{
    // Initialization and loop over all positions.
    if(!m_current_position_idx || m_current_position_idx.value() >= m_camera_positions.size() - 1)
    {
        m_current_position_idx = 0;
    }
    else
    {
        m_current_position_idx.value()++;
    }

    this->to_predefined_position(m_current_position_idx.value(), m_animate_interactor);
}

//------------------------------------------------------------------------------

void predefined_camera::previous_position()
{
    // Initialization and loop over all positions.
    if(!m_current_position_idx || m_current_position_idx.value() == 0)
    {
        m_current_position_idx = m_camera_positions.size() - 1;
    }
    else
    {
        m_current_position_idx.value()--;
    }

    this->to_predefined_position(m_current_position_idx.value(), m_animate_interactor);
}

//------------------------------------------------------------------------------

void predefined_camera::to_predefined_position(std::size_t _idx, bool _animate)
{
    SIGHT_ASSERT(
        "Cannot move to position '" << _idx << " / " << m_camera_positions.size() << "'",
        _idx < m_camera_positions.size()
    );

    if(auto layer = this->layer())
    {
        // 1. Stop timer if needed
        if(m_timer->is_running())
        {
            m_timer->stop();
        }

        // Reset the percentage
        m_percentage = 0.F;

        Ogre::Camera* const camera      = layer->get_default_camera();
        Ogre::SceneNode* const cam_node = camera->getParentSceneNode();

        // Get destination orientation.
        const auto& pos = m_camera_positions[_idx];

        // Relative destination in  regard of reference (this->init()).
        const Ogre::Quaternion rotate_x(Ogre::Degree(pos.rx), Ogre::Vector3(1, 0, 0));
        const Ogre::Quaternion rotate_y(Ogre::Degree(pos.ry), Ogre::Vector3(0, 1, 0));
        const Ogre::Quaternion rotate_z(Ogre::Degree(pos.rz), Ogre::Vector3(0, 0, 1));
        const auto destination = this->transform_quaternion() * m_camera_init_rotation * rotate_z * rotate_y * rotate_x;

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
            const float nb_step            = std::ceil(short_angle.valueDegrees() * 40.F / 180.F);

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
                        static_cast<Ogre::Real>(percentage_to_apply),
                        origin,
                        destination,
                        true
                    );

                    this->rotate_camera(cam_node, rotation);

                    layer->request_render();

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
            this->rotate_camera(cam_node, destination);
        }

        m_current_position_idx = _idx;
    }
}

//------------------------------------------------------------------------------

void predefined_camera::set_position(const std::string& _value)
{
    const auto it = std::ranges::find_if(
        m_camera_positions,
        [_value](const predefined_position_t& _p)
        {
            return _p.name == _value;
        });

    const auto index = static_cast<std::size_t>(it - m_camera_positions.begin());

    if(index >= m_camera_positions.size())
    {
        SIGHT_ERROR("Cannot move to position '" << index << " / " << m_camera_positions.size() << "'");
        return;
    }

    this->to_predefined_position(static_cast<std::size_t>(index), m_animate_interactor);
}

//------------------------------------------------------------------------------

void predefined_camera::rotate_camera(Ogre::SceneNode* const _cam, const Ogre::Quaternion& _quat) const
{
    _cam->translate(Ogre::Vector3(0, 0, -m_look_at_z), Ogre::Node::TS_LOCAL);
    _cam->setOrientation(_quat);
    _cam->translate(Ogre::Vector3(0, 0, m_look_at_z), Ogre::Node::TS_LOCAL);
}

//------------------------------------------------------------------------------

void predefined_camera::follow_transform()
{
    Ogre::Camera* const camera      = layer()->get_default_camera();
    Ogre::SceneNode* const cam_node = camera->getParentSceneNode();

    // Initial rotation.
    const auto referential_r = this->transform_quaternion();
    // Initial translation
    const auto referential_t = m_transform_matrix.getTrans();

    // 1. Get to origin (remove zoom level)
    cam_node->translate(Ogre::Vector3(0, 0, -m_look_at_z), Ogre::Node::TS_LOCAL);
    const auto current_r = cam_node->getOrientation();

    // 2. Apply transform rotation
    // Remove previous transform orientation and go to newest orientation.
    if(m_follow_orientation_interactor)
    {
        cam_node->setOrientation(m_last_orientation.Inverse() * referential_r * current_r);
    }

    // 3. Translate to new target.
    cam_node->setPosition(referential_t);

    // 4. Get back to same level of zoom
    cam_node->translate(Ogre::Vector3(0, 0, m_look_at_z), Ogre::Node::TS_LOCAL);

    layer()->request_render();

    // Store transform orientation for next updates.
    m_last_orientation = referential_r;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor
